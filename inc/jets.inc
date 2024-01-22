' Code to handle enemy jets

CONST MAX_JETS=3
CONST SPEED_JETS=150
CONST SPEED_JETFIRE=500
CONST SPEED_JETNEXT=2000

CONST JET_NONE=0
CONST JET_FLY=1
CONST JET_TURN=2
CONST JET_ATTACK=3
CONST JET_FLYOUT=4

DIM Integer Jet.W=70
DIM Integer Jet.H=51
DIM Integer Jets.cnt

DIM Integer Jets.State(MAX_JETS)
DIM Float   Jets.X(MAX_JETS)
DIM Float   Jets.Y(MAX_JETS)
DIM Float   Jets.dX(MAX_JETS)
DIM Float   Jets.dY(MAX_JETS)
DIM Integer Jets.idx(MAX_JETS)
DIM Float   Jets.rate(MAX_JETS)
DIM Integer Jets.Shots.life(MAX_JETS,2)
DIM Float   Jets.Shots.X(MAX_JETS,2)
DIM Float   Jets.Shots.Y(MAX_JETS,2)
DIM Float   Jets.Shots.dX(MAX_JETS,2)
DIM Float   Jets.Shots.dY(MAX_JETS,2)

sub Jets.init(cnt%)
  LOCAL Integer t
  
  if (cnt%>MAX_JETS) then cnt%=MAX_JETS
  Jets.cnt=cnt%    

  for t=0 to MAX_JETS-1
    Jets.State(t)=JET_NONE
  next
end sub

sub Jets.new()
  LOCAL Integer t

  for t=0 to Jets.cnt-1
    if (Jets.State(t)=JET_NONE) then
      Jets.State(t)=JET_FLY
      Jets.X(t)=Player.X+Screen.VPx-Game.VPw
      Jets.Y(t)=100+(RND()-0.5)*100
      Jets.dX(t)=4
      Jets.dY(t)=0
      Jets.idx(t)=0
      Jets.Shots.life(t,0)=0
      Jets.Shots.life(t,1)=0
      exit sub
    endif
  next
end sub

sub Jets.draw()          ' 0,1,2,3, 4, 5, 6, 7, 8
  LOCAL Float JetTurnX(8)=(0,2,1,0,-1,-2,-3,-4,-6)
  LOCAL Float JetTurnY(8)=(0,2,2,3, 3, 3, 2, 2, 0)

  STATIC Float interval=TIMER
  LOCAL Integer n,x,action,idx

  if (TIMER-interval>SPEED_JETS) then interval=TIMER:action=1

  for n=0 to MAX_JETS-1
    if (action=1) then
      select case Jets.State(n)
        case JET_NONE:
          Jets.dX(n)=0
          Jets.dY(n)=0
        case JET_FLY:
          Jets.idx(n)=0
          if (Jets.X(n)-Screen.VPx>Game.VPw-120) then
            Jets.State(n)=JET_TURN
          endif
        case JET_TURN:
          idx = Jets.idx(n)+1
          Jets.idx(n)=idx
          Jets.dX(n)=JetTurnX(idx)
          Jets.dY(n)=JetTurnY(idx)*RND()
          if (idx=2) then playSample 15,22500,3
          if (idx=8) then Jets.State(n)=JET_ATTACK
        case JET_ATTACK:
          if (Jets.rate(n)=0) then
            Jets.rate(n)=TIMER
            Jets.fire(n)
          elseif (TIMER-Jets.rate(n)>SPEED_JETFIRE) then
            Jets.rate(n)=0
            Jets.fire(n)
            Jets.State(n)=JET_FLYOUT
          endif
        case JET_FLYOUT:
          Jets.dY(n)=-1
      end select
    endif

    if (Jets.State(n)<>JET_NONE) then
      Jets.X(n)=Jets.X(n)+Jets.dX(n)+Player.dX
      Jets.Y(n)=Jets.Y(n)+Jets.dY(n)

      x = Jets.X(n) - Screen.VPx
      if x>-Jet.W and x<Game.VPw then
        blit Jets.idx(n)*Jet.W,230,x,Jets.Y(n),Jet.W,Jet.H,4,&B100
        if (Jets.State(n)=JET_ATTACK or Jets.State(n)=JET_FLYOUT) then
          if (Heli.isHit(x,Jets.Y(n)+20,Jet.W,Jet.H-30)) then 
            Player.destroy
            Jets.State(n)=JET_NONE
            Explosion.add(x,Jets.Y(n),3)
          endif
        endif
      elseif Jets.State(n)=JET_FLYOUT then
        Jets.State(n)=JET_NONE
      endif
    endif
  next
end sub

sub Jets.fire(t) 
  LOCAL Integer s,x

  for s=0 to 1
    if (Jets.Shots.life(t,s)=0) then
      Jets.Shots.life(t,s)=MAX_SHOTLIFE
      Jets.Shots.X(t,s)=Jets.X(t)+Jet.W/2
      Jets.Shots.Y(t,s)=Jets.Y(t)+Jet.H/2
      Jets.Shots.dX(t,s)=Jets.dX(t)-2
      Jets.Shots.dY(t,s)=Jets.dY(t)+0.4
      PlaySample 7,22050
      exit for
    endif
  next
end sub

sub Jets.moveAndDrawShots
  LOCAL Integer s,t
  LOCAL Float x,y

  for t=0 to MAX_JETS-1
    if (Jets.State(t)=JET_NONE) then continue for

    for s=0 to 1
      if Jets.Shots.life(t,s)>0 then
        Jets.Shots.life(t,s)=Jets.Shots.life(t,s)-1
        Jets.Shots.X(t,s)=Jets.Shots.X(t,s)+Jets.Shots.dX(t,s)
        Jets.Shots.Y(t,s)=Jets.Shots.Y(t,s)+Jets.Shots.dY(t,s)

        x=Jets.Shots.X(t,s)-Screen.VPx
        y=Jets.Shots.Y(t,s)

        if (x>Game.VPx AND x<Game.VPX+Game.VPw) then
          line x,y,x-Jets.Shots.dX(t,s),y-Jets.Shots.dY(t,s),,rgb(white)
          pixel x,y,rgb(red)
          if (Heli.isHit(x,y)) then
            Jets.Shots.life(t,s)=0
            Player.destroy
          endif
        else
          Jets.Shots.life(t,s)=0
        endif
      endif
    next s
  next t
end sub



