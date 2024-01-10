' Code to handle humans that should be rescued

CONST MAX_HUMANS=32
CONST MAX_HUMANS_FREQ=1000
CONST SPEED_HUMANS=80

CONST HUMAN_CMD_NONE=0
CONST HUMAN_CMD_WAVE=1
CONST HUMAN_CMD_VANISH=2

DIM Integer Human.W=13
DIM Integer Human.H=16

DIM Integer Humans.visible(MAX_HUMANS)
DIM Integer Humans.state(MAX_HUMANS)
DIM Float   Humans.X(MAX_HUMANS)
DIM Float   Humans.Y(MAX_HUMANS)
DIM Integer Humans.idx(MAX_HUMANS)
DIM Integer Humans.dir(MAX_HUMANS)

DIM Float   Humans.endX(MAX_HUMANS)
DIM Integer Humans.endCmd(MAX_HUMANS)
DIM String  Humans.callback(MAX_HUMANS)

sub Humans.init(cnt)
  LOCAL Integer n
  
  for n=0 to MAX_HUMANS-1
    Humans.Y(n)=Screen.GndLv-Human.H-1
    Humans.dir(n)=&B100
    if (n<cnt) then
      Humans.visible(n)=1
      Humans.X(n)=Screen.W/8+Screen.W*0.8*rnd
    else
      Humans.visible(n)=0
    endif
  next
end sub

function Humans.count()
  LOCAL Integer n,cnt

  for n=0 to MAX_HUMANS-1
    if (Humans.visible(n)=1 and Humans.state(n)<>3) then cnt=cnt+1
  next
  cnt=cnt+Huts.cntHumans()
  cnt=cnt+Camps.cntHumans()
  cnt=cnt+Heli.Humans
  cnt=cnt+Basis.Humans
  Humans.count=cnt
end function

function Humans.isDead(idx)
  Humans.isDead=0
  if (Humans.state(idx)=3 or Humans.visible(idx)=0) then Humans.isDead=1
end function

function Humans.getFree()
  LOCAL Integer n

  for n=0 to MAX_HUMANS-1
    if (Humans.visible(n)=0) then
      Humans.visible(n)=1
      Humans.getFree=n
      exit function
    endif
  next
  Humans.getFree=-1
end function

function Humans.add(posX)
  LOCAL Integer idx

  idx=Humans.getFree()
  Humans.add=idx
  if (idx=-1) then exit sub

  Humans.X(idx)=posX
  Humans.endX(idx)=0
  Humans.endCmd(idx)=HUMAN_CMD_NONE
  Humans.callback$(idx)=""
  Humans.state(idx)=0
  Humans.idx(idx)=0
end function

sub Humans.runToWave(idx, endX)
  if (idx=-1) then exit sub

  Humans.endX(idx)=endX
  Humans.endCmd(idx)=HUMAN_CMD_WAVE
  Humans.callback$(idx)=""
end sub

sub Humans.runToEnter(idx, endX, callback$)
  if (idx=-1) then exit sub

  Humans.endX(idx)=endX
  Humans.endCmd(idx)=HUMAN_CMD_VANISH
  Humans.callback$(idx)=callback$
end sub

sub Humans.draw()
  STATIC Float interval=TIMER
  LOCAL Integer HeliDoor(8)=(60,60,48,48,48,36,36,60,36)
  LOCAL Integer n,x,action,idx
  LOCAL Integer dist,landed
  LOCAL String cmd$

  if (TIMER-interval>SPEED_HUMANS) then interval=TIMER:action=1

  landed=Heli.isLanded() and not Heli.isOnBase()
  for n=0 to MAX_HUMANS-1
    if (Humans.visible(n)=0) then continue for

    if (action=1) then
      if (Humans.state(n)<>3) then
        if (Game.State<>STATE_GAME) then
          Humans.state(n)=0  ' standing
        else if (landed=1) then
          Humans.state(n)=2  ' running
          dist=(Humans.X(n)+Human.W/2)-(Screen.VPx+Player.X+HeliDoor(Heli.State))
        elseif (Humans.endCmd(n)<>HUMAN_CMD_NONE) then
          Humans.state(n)=2  ' running
          dist=Humans.X(n)-Humans.endX(n)
        else
          Humans.state(n)=1
        endif

        if (Humans.state(n)=2) then
          if (dist>0) then 
            Humans.X(n)=Humans.X(n)-1
            Humans.dir(n)=&B101
          else if (dist<0) then
            Humans.X(n)=Humans.X(n)+1
            Humans.dir(n)=&B100
          endif

          if (abs(dist)<2) then  ' target position reached
            if (landed=1) then
              if (Heli.boardHuman()=1) then Humans.visible(n)=0
            else
              select case Humans.endCmd(n)
                case HUMAN_CMD_WAVE:
                  Humans.state(n)=1
                case HUMAN_CMD_VANISH:
                  Humans.visible(n)=0
                  call Humans.callback$(n),n
              end select
              Humans.endCmd(n)=HUMAN_CMD_NONE
            endif
          endif
        endif
      endif

      Humans.idx(n)=Humans.idx(n)+1 ' animation housekeepping
    endif

    select case Humans.state(n)
    case 1:       ' waving
      if (Humans.idx(n)>3) then Humans.idx(n)=0
      idx = 1+Humans.idx(n)
    case 2:       ' running
      if (Humans.idx(n)>4) then Humans.idx(n)=0
      idx = 5+Humans.idx(n)
    case 3:      ' dying
      if (Humans.idx(n)>2) then Humans.idx(n)=2
      idx = 10+Humans.idx(n)
    case else: ' standing
      Humans.idx(n)=0
      idx = 0
    end select

    x = Humans.X(n) - Screen.VPx
    if x>-Human.W and x<Game.VPw then
      blit idx*Human.W,166,x,Humans.Y(n),Human.W,Human.H,4,Humans.dir(n)
    elseif (Humans.state(n)=3) then ' dead
      Humans.visible(n)=0
    endif
  next
end sub

'testX and testY in ViewPort coordinates
function Humans.isHitAndKilled(testX,testY)
  LOCAL Float x1,x2,y1,y2
  LOCAL Integer n

  Humans.isHitAndKilled=0
  for n=0 to MAX_HUMANS-1
    if (Humans.state(n)=3) then continue for
    if (Humans.visible(n)=1) then
      x1=Humans.X(n)-Screen.VPx
      x2=x1+Human.W
      y1=Humans.Y(n)
      y2=y1+Human.W

      if (testX>x1 AND TestX<x2 AND testY>y1 AND testY<y2) then
        Humans.state(n)=3
        PlaySample 2,22050
        Humans.isHitAndKilled=1
        exit for
      endif
    endif
  next
end function


