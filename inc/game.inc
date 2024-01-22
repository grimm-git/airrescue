' Game objects
' Code by "Matthias Grimm"

CONST WE.PROG_DIR$=we.get_parent$(Mm.Info$(Current))
CONST MAX_LEVELS=9

CONST EVENT_JETS=60000

CONST STATE_INTRO=0
CONST STATE_DEMO=1
CONST STATE_GAME=2
CONST STATE_DEAD=3
CONST STATE_LEVEL=4
CONST STATE_OVER=5

DIM Float frameTimer

DIM Integer Game.W
DIM Integer Game.H
DIM Integer Game.CM
DIM Integer Game.VPx      ' Position of ViewPort in visible Game screen
DIM Integer Game.VPy
DIM Integer Game.VPw      ' Size of ViewPort, can't be bigger than Game Screen
DIM Integer Game.VPh
DIM Integer Game.Controls  ' 0=keyboard, 1=Nunchuk
DIM Float   Game.FrameTime=1
DIM Float   Game.UpdateTime
DIM Integer Game.State

DIM Integer Screen.W=8000   ' Virtual Game Screen
DIM Integer Screen.H=400    ' /
DIM Float   Screen.VPx      ' Position of ViewPort in virtual Screen
DIM Float   Screen.VPy
DIM Integer Screen.GndLv=350
DIM Integer Screen.TankLv=390

DIM Integer Level
DIM Integer Level.Score
DIM Integer Level.HumansTotal
DIM Integer Level.HumansAlife
DIM String  Level.Levels$(MAX_LEVELS)


sub Game.init()
  Game.W=MM.HRes
  Game.H=MM.VRes
  Game.CM=getCMode()
  Game.VPw=Game.W
  Game.VPh=Game.H

  Screen.VPx=(Screen.W-Game.VPw)/2
  Screen.VPy=(Screen.H-Game.VPh)/2
  Controls.init()
  Explosion.init()
  playMod "intro"
end sub

sub Game.new()
  Level=0
  Player.lives=3
  Player.Score=0
  Player.reset()
  Game.State=STATE_GAME
  Game.setupLevel()
  PlayMOD "soundfx"
end sub

sub Game.nextLevel()
  Player.reset()
  Player.Score=Player.Score+Level.Score
  if (Level<MAX_LEVELS) then Level=Level+1
  Game.setupLevel()
  Game.State=STATE_GAME
end sub

sub Game.setupLevel()
  LOCAL Integer LvTanks(MAX_LEVELS)=(4,4,6,6,8,8,10,10,12,12)
  LOCAL Integer LvTankFireRate(MAX_LEVELS)=(3000,2500,2500,2500,2500,2000,2000,2000,1000,1000)
  LOCAL Integer LvFreeHumans(MAX_LEVELS)=(4,4,5,5,8,8,12,12,15,15)
  LOCAL Integer LvJets(MAX_LEVELS)=(1,1,1,1,2,2,2,2,3,3)

  Collision.init()
  Basis.init
  Pebbles.init
  Sams.init
  Trees.init Level.Levels$(Level)
  Huts.init  Level.Levels$(Level)
  Camps.init Level.Levels$(Level)
  Tanks.init LvTanks(Level),LvTankFireRate(Level)
  Jets.init  LvJets(Level)
  Humans.init LvFreeHumans(Level)
  
  Level.HumansTotal=Humans.count()
  Level.HumansAlife=Level.HumansTotal
  Level.Score=0
end sub

' Page Setup
' 0 : display foreground page
' 1 : display background page
' 2 : drawing page
' 3 : helicopter sprites
' 4 : assets sprites
' 5 : explosion sprites
sub Game.loadAssets()
  LOCAL ASSETDIR$=WE.PROG_DIR$+"/img"
 
  page write 1
  if (Game.CM=12) then page write 0
  load png ASSETDIR$+"/background.png",0,0,0
  page write 3
  load png ASSETDIR$ + "/helicopter.png",0,0,4
  page write 4
  load png ASSETDIR$ + "/assets.png",0,0,4
  page write 5
  load png ASSETDIR$ + "/explosion.png",0,0,4

  load font WE.PROG_DIR$ + "/army.fnt"
end sub

sub Game.clearScr
  frameTimer=TIMER
  if (Game.CM=12) then
    cls
  else
    page copy 1 to 2,I   ' Background, faster than blit
  endif
end sub

sub Game.draw()
  STATIC Float eventTimer=TIMER
  STATIC Float jetTimer=TIMER

  Hills.draw
  Pebbles.draw
  Sams.draw
  Trees.draw
  Huts.draw
  Camps.draw
  Humans.draw
  Basis.draw
  Jets.draw

  if (Game.State=STATE_GAME) then
    Heli.moveAndDrawShots
    Heli.draw
    Tanks.moveAndDrawShots
    Jets.moveAndDrawShots
    Sams.moveAndDrawMissile
    
    if (TIMER-eventTimer>EVENT_JETS) then
      if (TIMER-jetTimer>SPEED_JETNEXT) then
        Jets.new
        jetTimer=TIMER
        if (Jets.State(Jets.cnt-1)<>JET_NONE) then eventTimer=TIMER
      endif
    endif
  endif

  Tanks.draw
  Explosion.draw
  Level.HumansAlife=Humans.count()

  Text Game.W/2,22,"AIR RESCUE 2024","C",8,,rgb(yellow),-1
  Text 40,22,"SCORE: "+str$(Player.Score)+"  LIVES: "+str$(Player.Lives),"L",8,,rgb(yellow),-1
  Text 600,22,"HUMANS L:"+str$(Heli.Humans)+" R:"+str$(Basis.Humans)+" T:"+str$(Level.HumansAlife),"R",8,,rgb(yellow),-1
end sub

sub Game.swapPage
  Game.updateTime=TIMER-frameTimer

  if (Game.CM=12) then
    page copy 2 to 1,B
  else
    page copy 2 to 0,B
  endif

  Game.frameTime=TIMER-frameTimer
end sub

' Gets the parent directory of 'f$', or the empty string if it does not have one.
Function we.get_parent$(f$)
  Local i%

  For i% = Len(f$) To 1 Step -1
    If InStr("/\", Mid$(f$, i%, 1)) > 0 Then Exit For
  Next i%

  If i% = 0 Then
    we.get_parent$ = ""
  Else
    we.get_parent$ = Left$(f$, i% - 1)
  EndIf
End Function

function getCMode()
  LOCAL cm$=mid$(str$(MM.INFO(MODE)),3)
  getCMode=val(cm$)
end function

function corrFPS(value)
  LOCAL Float fps=1000/Game.frameTime

  if fps<1 then
    corrFPS=value
  else
    corrFPS=75/fps*value
  endif
end function

Sub Game.showData
  LOCAL Integer lf

  font 7
  lf = MM.INFO(Fontheight)
  text Game.W-130,50+0*lf, "FPS       : " + str$(int(1000/Game.frameTime))
  text Game.W-130,50+1*lf, "FPS Corr  : " + str$(int(corrFPS(1)*100)/100))
  text Game.W-130,50+2*lf, "UpdateTime: " + str$(int(Game.updateTime*10)/10)+"ms"
  text Game.W-130,50+3*lf, "FrameTime : " + str$(int(Game.frameTime*10)/10)+"ms"
  text Game.W-130,50+4*lf, "Time Fire : " + str$(Controls.getTime(CTRL_FIRE))
  text Game.W-130,50+5*lf, "Nunchuk   : " + str$(Game.Controls)
end Sub

' Level definition: H = Hut, C = Camp, T = Tree, B = Base, S = SAM (fix)
'
'                    6         5         4         3         2         1         0
'                 3210987654321098765432109876543210987654321098765432109876543210
'                 0000                       00000000                         0000
Level.Levels$(0)="S_T__T__T____T__T__T__T___T_T_BB___T____TT____T__TT__T__T___T__S"
Level.Levels$(1)="S______HT__________T_______H__BB___T____H_____T_______H_T______S"
Level.Levels$(2)="S______HT_____HT___T_______H__BB___T____H_____TH______H_T______S"
Level.Levels$(3)="S_____THTT____HT_TTT_______HT_BB___T__T_H__T__TH__TT__H_T__TT__S"
Level.Levels$(4)="S_____THTT____HT_T_T_CT____HT_BB_T___T__H__T__TH___TT_H_T___TT_S"
Level.Levels$(5)="S_TT__H__T____HT_T_T_CT____HT_BB_T___T__H__TH_TH___TT_C_T___TT_S"
Level.Levels$(6)="S_TT__HC_T____HT_T_T_CT____HT_BB_T___T__H__TH_TH___TT_C_T___TT_S"
Level.Levels$(7)="S__T__HC_T____HT_T_T_CT____HT_BB_TH__T__T__T_TH___TT_C_TC___TT_S"
Level.Levels$(8)="S__T__TC_T____CT_T_T_CT____TT_BB_TH__T__TC_T__TH__TT_C_TC___TT_S"
Level.Levels$(9)="S__T__TC_T____CT_T_T___CT__TT_BB_TC__TC_T__T__TC__TT_C_TC___TT_S"









