' (c) 2024, Program by "Matthias Grimm"
' This program was published under the GPL3 license

OPTION BASE 0
OPTION EXPLICIT

CONST DEBUG=0
CONST MAX_GAMEOVERTIME=10000

#Include "inc/game.inc"
#Include "inc/controls.inc"
#Include "inc/collision.inc"
#Include "inc/sound.inc"

mode 2,16,RGB(0,0,0)

DIM Float   Player.X=0
DIM Float   Player.Y=0
DIM Float   Player.dX=0
DIM Float   Player.dY=0
DIM Integer Player.lives=3
DIM Integer Player.Score

#Include "inc/base.inc"
#Include "inc/heli.inc"
#Include "inc/trees.inc"
#Include "inc/huts.inc"
#Include "inc/camps.inc"
#Include "inc/tanks.inc"
#Include "inc/jets.inc"
#Include "inc/humans.inc"

' After crash
sub Player.reset()
  Screen.VPx=(Screen.W-Game.VPw)/2

  Player.X=Game.W/2-10
  Player.Y=Screen.GndLv-Heli.H
  Player.dX=0
  Player.dY=0

  Heli.reset()
  Explosion.init()
  Game.State=STATE_GAME
end sub

sub Player.destroy()
  Player.lives=Player.lives-1
  Heli.Humans=0
  Explosion.add Player.X+Heli.W/2, Player.Y+Heli.H,1
  Game.State=STATE_DEAD
end sub

' ********************************************************************************
'                                Main Game Loop
' ********************************************************************************
DIM Integer lock.fire   ' single fire only
DIM Integer lock.turn
DIM Integer ctrl
DIM Integer ctrlTime
DIM Integer key
DIM Float   a           ' Title colorcycling
DIM Integer color        ' Title colorcycling
DIM Float   GameOverTimer ' Time from GameOver to Intro

Game.init
Game.loadAssets
Game.setupLevel

page write 2  ' drawing buffer

do ' Main Loop
  Game.clearScr

  ctrl=Controls.read()
  if (ctrl=0) then Heli.toHover()

  if (Game.State=STATE_GAME) then
    if (ctrl and 15) = 0 then Heli.toHover()  ' joystick in center position

    if (ctrl and 1) > 0 then   ' right
      Heli.toRight(lock.turn)
      if (Player.dX < 3) then Player.dX = Player.dX + corrFPS(0.02)
    endif

    if (ctrl and 2) > 0 then   ' left
      Heli.toLeft(lock.turn)
      if (Player.dX > -3) then Player.dX = Player.dX - corrFPS(0.02)
    endif

    if (ctrl and 4) > 0 then   ' up
      if Player.Y > 50 then
        Player.Y = Player.Y - corrFPS(1)
      endif
    endif

    if (ctrl and 8) > 0 then   ' down
      if Player.Y < (Screen.GndLv - Heli.H) then
        Player.Y = Player.Y + corrFPS(1)
      else
        if (Player.dX > 0) then
          Player.dX = Player.dX - corrFPS(0.1)
          if (Player.dX < 0) then Player.dX = 0
        else
          Player.dX = Player.dX + corrFPS(0.1)
          if (Player.dX > 0) then Player.dX = 0
        endif
      endif
    endif

    if (ctrl and 16) > 0 then   ' fire
      if (lock.fire=0) then
        Heli.fire
        lock.fire=1
      endif

      ctrlTime=Controls.getTime(CTRL_FIRE)
      if (ctrlTime>10) then lock.turn=1
      if (ctrlTime>20) then lock.turn=2
    else
      lock.fire=0
      lock.turn=0
    endif

    if (isHeliCollided()=1) then Player.destroy
  endif

  key=Controls.readKey()
  if key = asc("Q") or key = asc("q") then EXIT DO

  ' Object Movements
  Screen.VPx = Screen.VPx + Player.dX
  if Screen.VPx < 0 then
     Screen.VPx = 0
     Player.dX = 0
  else if Screen.VPx > Screen.W - Game.VPw then
     Screen.VPx = Screen.W-Game.VPw
     Player.dX = 0
  endif
  if (Player.Y<(Screen.GndLv - Heli.H)) then Player.Y=Player.Y+corrFPS(1/32)

  ' Change game state
  if (Basis.Humans=Level.HumansAlife) then
    if (Game.State<>STATE_LEVEL) then PlaySample 8,22050,1
    Game.State=STATE_LEVEL

  elseif (Game.State=STATE_OVER) then
    if (TIMER-GameOverTimer>MAX_GAMEOVERTIME) then
      Game.State=STATE_INTRO
      Player.lives=3
      playMod "intro"
    endif
  elseif (Player.lives=0) then
    if (Game.State<>STATE_OVER) then
      PlaySample 10,22050,1
      Player.Score=Player.Score+Basis.Humans*100-(Level.HumansTotal-Level.HumansAlife)*500
    endif
    Game.State=STATE_OVER
    GameOverTimer=TIMER
  endif

  if (Game.State=STATE_LEVEL) then
    Level.Score=Basis.Humans*100-(Level.HumansTotal-Level.HumansAlife)*500
    printShadowText -3,"CONGRATULATIONS - LEVEL "+str$(Level+1)+" COMPLETED!",rgb(200,200,32)
    printShadowText -2,"HUMANS RESCUED "+str$(Basis.Humans),rgb(200,200,32)
    printShadowText -1,"HUMANS KILLED  "+str$(Level.HumansTotal-Level.HumansAlife),rgb(200,200,32)
    printShadowText 0,"LEVEL SCORE    "+str$(Level.Score),rgb(200,200,32)
    printShadowText 2,"PRESS 'SPACE' TO CONTINUE",rgb(200,200,32)
    if (key=asc(" ")) then Game.nextLevel()

  elseif (Game.State=STATE_DEAD) then
    printShadowText -1,"HELICOPTER DESTROYED, ALL PASSENGERS DEAD",rgb(200,32,32)
    printShadowText 0,"PRESS 'SPACE' TO CONTINUE",rgb(200,32,32)
    if (key=asc(" ")) then Player.reset

  elseif (Game.State=STATE_OVER) then
    printShadowText -1,"GAME OVER!",rgb(yellow),3
    printShadowText  2,"FINAL SCORE: "+str$(Player.Score),rgb(yellow)
    if (key=asc(" ")) then Game.new()

  elseif (Game.State=STATE_INTRO) then
    a=a+0.02:if (a>PI) then a=0
    color=rgb(255*abs(sin(a)),255*abs(sin(a+PI/4)),255*abs(sin(a+PI/2)))
    printShadowText -8,"AIR RESCUE 2024",color,3
    printShadowText -5,"PROGRAM BY MATTHIAS GRIMM",rgb(white)
    printShadowText -4,"GRAPHICS BY MATTHIAS GRIMM",rgb(white)
    printShadowText -3,"SFX BY DOMINIK BRAUN & MATTHIAS GRIMM",rgb(white)
    printShadowText -2,"MUSIC BY UNKNOWN ARTIST",rgb(white)
    printShadowText -1,"(c) 2024",rgb(white)
    printShadowText  1,"CONTROL WITH "+choice(Game.Controls=1,"NUNCHUK","KEYBOARD"),rgb(32,200,32)
    printShadowText  2,"PRESS 'SPACE' TO START GAME",rgb(32,200,32)
    if (key=asc(" ")) then Game.new()
  endif

  if (DEBUG) then Game.showData
  Game.draw  ' Draw all objects to screen
  if key = 147 then save image "screenshot.bmp" ' F3 for screen shot
  Game.swapPage  
loop

sub printShadowText(ln,msg$,color,scale)
  LOCAL Integer lf

  font 8
  lf=MM.INFO(Fontheight)
  if (scale=0) then scale=1
  text Game.W/2+1,Game.H/2+lf*ln+1,msg$,"C",8,scale,rgb(black),-1
  text Game.W/2,Game.H/2+lf*ln,msg$,"C",8,scale,color,-1
end sub


