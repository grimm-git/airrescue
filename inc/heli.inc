' Include heli.inc
' Additional code to control player figure

CONST MAX_HELISHOTS=10
CONST MAX_HELILOAD=12
CONST SPEED_HELI_FLIGHT=40
CONST SPEED_HELI_LANDED=100

DIM Integer Heli.W=96
DIM Integer Heli.H=38
DIM Integer Heli.State=3
DIM Integer Heli.trs=0
DIM Integer Heli.seq=31
DIM Integer Heli.idx
DIM Integer Heli.Shots.life(MAX_HELISHOTS)
DIM Float   Heli.Shots.X(MAX_HELISHOTS)
DIM Float   Heli.Shots.Y(MAX_HELISHOTS)
DIM Float   Heli.Shots.dX(MAX_HELISHOTS)
DIM Float   Heli.Shots.dY(MAX_HELISHOTS)
DIM Integer Heli.Humans

sub Heli.reset()
  LOCAL Integer t

  Heli.State=3
  Heli.trs=0 
  Heli.seq=31
  Heli.idx=0

  for t=0 to MAX_HELISHOTS-1
    Heli.Shots.life(t)=0
  next
end sub

function Heli.boardHuman()
  Heli.boardHuman=0
  if (Heli.Humans<MAX_HELILOAD) then
    Heli.boardHuman=1
    Heli.Humans=Heli.Humans+1
    PlaySample 1,22050
  end if
end function

' States                                                     
'                     |<- drift left <->| hover |<-> drift right ->|
' forward |<->| hover |---------------->| nose  |<-----------------| hover |<->| forward
'  left   |   | left  |-> backw. left -------------> drift right ->| right |   |  right
'                     |<- drift left <------------- backw. right <-|
'
' State Name           Anim  Transistions
'  0 A  forward right   21   B:25
'  1 B  hover right     16   A:25, C:20
'  2 C  drift right     41   D:0, I:5 
'  3 D  hover nose      31   C:0, E:0, B:20, F:5
'  4 E  drift left      36   D:0, H:20
'  5 F  hover left       1   E:5, G:10
'  6 G  forward left     6   F:10
'  7 H  backward right  26   B:30
'  8 I  backward left   11   F:15
sub Heli.setState(newState)
  LOCAL oldState = Heli.State

  if (newState <> oldState) then
    Heli.trs = 0  ' default
    select case newState
      case 0:
        Heli.seq = 21
        if (oldState = 1) then Heli.trs = 25
      case 1:
        Heli.seq = 16
        if (oldState = 0) then Heli.trs = 25
        if (oldState = 2) then Heli.trs = 20
      case 2:
        Heli.seq = 41
        if (oldState = 8) then Heli.trs = 5
      case 3:
        Heli.seq = 31
        if (oldState = 1) then Heli.trs = 20
        if (oldState = 5) then Heli.trs = 5 
      case 4:
        Heli.seq = 36
        if (oldState = 7) then Heli.trs = 20
      case 5:
        Heli.seq = 1
        if (oldState = 4) then Heli.trs = 5
        if (oldState = 6) then Heli.trs = 10
      case 6:
        Heli.seq = 6
        if (oldState = 5) then Heli.trs = 10
      case 7:
        Heli.seq = 26
        if (oldState = 1) then Heli.trs = 30
      case 8:
        Heli.seq = 11
        if (oldState = 5) then Heli.trs = 15
    end select
    Heli.State = newState
  end if
end sub

sub Heli.toHover()       '0 1 2 3 4 5 6 7 8
  LOCAL hoverStates(8) = (1,1,3,3,3,5,5,1,5)

  if Heli.trs = 0 then Heli.setState(hoverStates(Heli.State))

  if (Player.dX > 0) then Player.dX = Player.dX - corrFPS(0.01) ' Air Drag
  if (Player.dX < 0) then Player.dX = Player.dX + corrFPS(0.01)
end sub

sub Heli.toLeft(enable) '0 1 2 3 4 5 6 7 8
  LOCAL leftStates(8) = (1,7,3,4,5,6,6,3,3)
  STATIC old=0
  
  if (Heli.State=4 and old=enable) then exit sub
  if (Heli.State=4 and enable=0) then exit sub
  if (Heli.State=7 and enable=0) then exit sub
  if (Heli.trs=0) then Heli.setState(leftStates(Heli.State))
  old=enable
end sub

sub Heli.toRight(enable) '0 1 2 3 4 5 6 7 8
  LOCAL rightStates(8) = (0,0,1,2,3,8,5,1,3)
  STATIC old=0
 
  if (Heli.State=2 and old=enable) then exit sub
  if (Heli.State=2 and enable=0) then exit sub
  if (Heli.State=8 and enable=0) then exit sub
  if (Heli.trs=0) then Heli.setState(rightStates(Heli.State))
  old=enable
end sub

sub Heli.fire      '  0  1  2  3  4  5  6  7  8 
  LOCAL offsetX(8)= (70,73,47,48,49,25,27,74,22)
  LOCAL offsetY(8)= (33,28,28,28,28,28,28,23,23)
  LOCAL fireDX(8) = ( 4, 4,-1, 0, 1,-4,-4, 4,-4)
  LOCAL fireDY(8) = ( 2, 0, 4, 4, 4, 0, 2,-2,-2)
  LOCAL Integer z

  for z=0 to MAX_HELISHOTS-1
    if Heli.Shots.life(z)=0 then
      Heli.Shots.life(z)=100
      Heli.Shots.X(z)=Player.X+offsetX(Heli.State)
      Heli.Shots.dX(z)=fireDX(Heli.State)
      Heli.Shots.Y(z)=Player.Y+offsetY(Heli.State)
      Heli.Shots.dY(z)=fireDY(Heli.State)
      PlaySample 4,22050
      exit sub
    endif
  next z
end sub

sub Heli.draw
  STATIC Float interval=TIMER
  STATIC Float unboarding=TIMER
  STATIC Float test=TIMER
  STATIC Float speed=SPEED_HELI_LANDED
  LOCAL Integer idx,c,r

  if (TIMER-interval>speed) then
    Heli.idx=(Heli.idx+1) AND 3
    if (Heli.trs>0) then Heli.trs=0
    interval=TIMER
  endif

  if (TIMER-test>speed+100) then
    if (Heli.isLanded()=1) then
      if (speed<SPEED_HELI_LANDED) then speed=speed+5
    else
      if (speed>SPEED_HELI_FLIGHT) then speed=speed-5
    endif
    test=TIMER
  endif

  Heli.Sound speed

  if (Heli.trs>0) then
    idx=Heli.trs
  else
    idx=Heli.seq+Heli.idx
  endif

  c = (idx-1) mod 5
  r = int((idx-1) / 5)
  blit c*Heli.W,r*Heli.H,int(Player.X),int(Player.Y),Heli.W,Heli.H,3,&B100
  if (Heli.isOnBase()=1) then
    text Game.W/2,45,"THE EAGLE HAS LANDED"),"C",8,,rgb(32,200,32),-1
    if (TIMER-unboarding>MAX_HUMANS_FREQ) then
      if (Heli.Humans>0) then
        idx=Humans.add(Player.X+Heli.W/2+Screen.VPx)
        Humans.runToEnter(idx,Basis.X+85,"Basis.rescueHuman")
        Heli.Humans=Heli.Humans-1
        unboarding=TIMER
      endif
    endif
  elseif (Heli.Humans=MAX_HELILOAD) then
    text Game.W/2,45,"CHOPPER FULLY LOADED"),"C",8,,rgb(200,32,32),-1
  endif
end sub

sub Heli.moveAndDrawShots()
  LOCAL Integer t,x,c

  for t=0 to MAX_HELISHOTS-1
    if Heli.Shots.life(t)>0 then
      Heli.Shots.life(t)=Heli.Shots.life(t)-1
      Heli.Shots.X(t)=Heli.Shots.X(t)+Heli.Shots.dX(t)
      Heli.Shots.Y(t)=Heli.Shots.Y(t)+Heli.Shots.dY(t)

      x=Heli.Shots.X(t)
      if (x>Game.VPx AND x<Game.VPX+Game.VPw) then
        pixel x,Heli.Shots.Y(t),rgb(white)
        pixel x,Heli.Shots.Y(t)+1,rgb(white)
        if (Tanks.isHitAndDestroy(x,Heli.Shots.Y(t))) then
          Heli.Shots.life(t)=0
        elseif (Camps.isHitAndDestroy(x,Heli.Shots.Y(t))) then
          Heli.Shots.life(t)=0
        elseif (Humans.isHitAndKilled(x,Heli.Shots.Y(t))) then
          Heli.Shots.life(t)=0
        endif
      else
        Heli.Shots.life(t)=0
      endif
      if (Heli.Shots.Y(t)>Screen.TankLv) then
        pixel x-1,Heli.Shots.Y(t)-1,rgb(white)
        pixel x+1,Heli.Shots.Y(t)-1,rgb(white)
        pixel x-2,Heli.Shots.Y(t)-3,rgb(white)
        pixel x+2,Heli.Shots.Y(t)-3,rgb(white)
        Heli.Shots.life(t)=0
      endif
    endif
  next t
end sub

'testX and testY in ViewPort coordinates
function Heli.isHit(x,y,w,h) AS INTEGER
  LOCAL rectX(8)= ( 7, 5,41,41,41,17,21, 4,16)
  LOCAL rectY(8)= ( 5,18,19,19,19,17, 5,18,18)
  LOCAL rectW(8)= (68,73,17,17,17,75,69,76,74)
  LOCAL rectH(8)= (31,17,15,15,15,17,31,20,20)
  LOCAL Float x1,x2,y1,y2
  
  x1=Player.X+rectX(Heli.State)
  x2=x1+rectW(Heli.State)
  y1=Player.Y+rectY(Heli.State)
  y2=y1+rectH(Heli.State)

  Heli.isHit=0
  if not (x+w<x1 or x>x2 or y+h<y1 or y>y2) then
'  if (x>x1 AND x<x2 AND y>y1 AND y<y2) then
    Heli.isHit=1
  endif
end function

function Heli.isLanded()
  Heli.isLanded=0
  if (Screen.GndLv-Player.Y-Heli.H<5) then Heli.isLanded=1
end function

function Heli.isOnBase()
  LOCAL Integer x,y,px

  x=Basis.X-Screen.VPx
  y=Screen.GndLv-Heli.H-5
  px=Player.X+Heli.W/2

  Heli.isOnBase=0
  if (x<px and px<x+85 and Player.Y>y) then Heli.isOnBase=1
end function

sub Heli.Sound speed%
  STATIC Float interval=TIMER
  STATIC Integer oldSpeed
  LOCAL Float factor
  LOCAL Float maxInterval
  
  if (Game.State<>STATE_GAME) then exit sub  ' Heli is dead

  maxInterval=choice(speed%=SPEED_HELI_LANDED,9900,5000)
  factor=1-(speed%-SPEED_HELI_FLIGHT)/(SPEED_HELI_LANDED-SPEED_HELI_FLIGHT)
  if (speed%=0) then
    playSample 14,22050,1  ' stop sound

  elseif (TIMER-interval>maxInterval or speed%<>oldSpeed) then
    interval=TIMER
    playSample 3,11025+11025*factor,1
    oldSpeed=speed%
  endif
end sub


