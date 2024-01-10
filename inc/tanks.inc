' Code to handle enimy tanks
' Objects GAME, SCREEN and CObj must be initialized first
' Needs: Screen.GndLv, Screen.VPx, Game.VPw

CONST MAX_TANKS=25
CONST SPEED_TANKS=100
CONST MAX_TANKSHOTS=3
CONST MAX_SHOTLIFE=120

DIM Integer Tank.W=60
DIM Integer Tank.H=28
DIM Integer Tank.rate=4000

DIM Integer Tanks.cnt=0
DIM Integer Tanks.dead(MAX_TANKS)
DIM Float   Tanks.X(MAX_TANKS)
DIM Float   Tanks.Y(MAX_TANKS)
DIM Integer Tanks.seq(MAX_TANKS)
DIM Integer Tanks.idx(MAX_TANKS)
DIM Integer Tanks.dir(MAX_TANKS)
DIM Integer Tanks.Shots.life(MAX_TANKS,MAX_TANKSHOTS)
DIM Float   Tanks.Shots.X(MAX_TANKS,MAX_TANKSHOTS)
DIM Float   Tanks.Shots.Y(MAX_TANKS,MAX_TANKSHOTS)
DIM Float   Tanks.Shots.dX(MAX_TANKS,MAX_TANKSHOTS)
DIM Float   Tanks.Shots.dY(MAX_TANKS,MAX_TANKSHOTS)


sub Tanks.init(tanks%, rate%)
  LOCAL Integer t,n
  LOCAL Float sector
  if (tanks% > MAX_TANKS) then tanks%=MAX_TANKS 
  Tanks.cnt=tanks%

  Tank.rate=rate%  ' rate of fire

  sector=Screen.W * PARALAX_LV3 / tanks%
  for t=0 to tanks%-1
    Tanks.X(t)=t*sector+sector/2
    Tanks.Y(t)=Screen.TankLv - Tank.H
    Tanks.seq(t)=0
    Tanks.idx(t)=0
    Tanks.dir(t)=0
    Tanks.dead(t)=0

    for n=0 to MAX_TANKSHOTS-1
      Tanks.Shots.life(t,n)=0
    next
  next
end sub

sub Tanks.draw()
  STATIC Float interval=TIMER
  STATIC Float nextShot=TIMER
  LOCAL Integer n,x,action,dist,idx,fire
  LOCAL Float minTankDist
  LOCAL Integer sndON

  if (TIMER-interval>SPEED_TANKS) then interval=TIMER:action=1
  if (TIMER-nextShot>Tank.rate)   then nextShot=TIMER:fire=1

  sndON=0
  for n=0 to Tanks.cnt-1
    if (Tanks.dead(n)=1) then
      Tanks.seq(n)=4
      Tanks.idx(n)=0
    else
      dist=(Tanks.X(n)+Tank.W/2)-(Screen.VPx*PARALAX_LV3+Player.X+Heli.W/2)
      if (action=1) then
        if (dist>0) then
          Tanks.dir(n)=1
        elseif (dist<0) then
          Tanks.dir(n)=0
        endif

        minTankDist=(Tank.W/2+n*Tank.W)-Tanks.cnt*Tank.W/2
        if (dist>minTankDist) then
          Tanks.X(n)=Tanks.X(n)-1
          Tanks.idx(n)=(Tanks.idx(n)+1) AND 1
        elseif (dist<minTankDist) then
          Tanks.X(n)=Tanks.X(n)+1
          Tanks.idx(n)=(Tanks.idx(n)+1) AND 1
        endif
      
        dist=abs(dist)
        if (dist<50) then
          Tanks.seq(n)=3
        elseif (dist<100) then
          Tanks.seq(n)=2
        elseif (dist<200) then
          Tanks.seq(n)=0
          if (Player.Y<Screen.GndLv-100) then Tanks.seq(n)=1
        else
          Tanks.seq(n)=1
          if (Player.Y>Screen.GndLv-100) then Tanks.seq(n)=0
        endif
      endif
    endif

    idx=2*Tanks.seq(n)+Tanks.idx(n)
    x = Tanks.X(n) - Screen.VPx * PARALAX_LV3

    if Tanks.dead(n)=0 and x>-100-Tank.W and x<Game.VPw+100 then sndON=1
    if x>-Tank.W and x<Game.VPw then
      blit idx*Tank.W,182,x,Tanks.Y(n),Tank.W,Tank.H,4,&B100+Tanks.dir(n)
      if (Game.State=STATE_GAME and fire=1 and Tanks.dead(n)=0) then Tanks.fire n
    endif
  next
  Tanks.Sound sndON
end sub

sub Tanks.fire(t) 
  LOCAL Integer s,x
  LOCAL Float dx,dy,a

  for s=0 to MAX_TANKSHOTS-1
    if (Tanks.Shots.life(t,s)=0) then
      Tanks.Shots.life(t,s)=MAX_SHOTLIFE
      Tanks.Shots.X(t,s)=Tanks.X(t)+Tank.W/2
      Tanks.Shots.Y(t,s)=Tanks.Y(t)+Tank.H/2

      x=Tanks.Shots.X(t,s) - Screen.VPx * PARALAX_LV3
      dx=Player.X+Heli.W/2-x
      dy=Player.Y+Heli.H/2-Tanks.Shots.Y(t,s)
      a=atan2(dy,dx)

      Tanks.Shots.dX(t,s)=4*cos(a)
      Tanks.Shots.dY(t,s)=4*sin(a)
      PlaySample 6,22050
      exit for
    endif
  next s
end sub

sub Tanks.moveAndDrawShots
  LOCAL Integer s,t,x

  for t=0 to Tanks.cnt-1
    for s=0 to MAX_TANKSHOTS-1
      if Tanks.Shots.life(t,s)>0 then
        Tanks.Shots.life(t,s)=Tanks.Shots.life(t,s)-1
        Tanks.Shots.X(t,s)=Tanks.Shots.X(t,s)+Tanks.Shots.dX(t,s)
        Tanks.Shots.Y(t,s)=Tanks.Shots.Y(t,s)+Tanks.Shots.dY(t,s)
        Tanks.Shots.dY(t,s)=Tanks.Shots.dY(t,s)+((MAX_SHOTLIFE-Tanks.shots.life(t,s))/250)^2

        x=Tanks.Shots.X(t,s) - Screen.VPx * PARALAX_LV3
        if (x>Game.VPx AND x<Game.VPX+Game.VPw) then
          pixel x,Tanks.Shots.Y(t,s),rgb(white)
          pixel x,Tanks.Shots.Y(t,s)+1,rgb(white)
          if (Heli.isHit(x,Tanks.Shots.Y(t,s))) then
            Tanks.Shots.life(t,s)=0
            Player.destroy
          endif
        else
          Tanks.Shots.life(t,s)=0
        endif
      endif
    next s
  next t
end sub

'testX and testY in ViewPort coordinates
function Tanks.isHitAndDestroy(testX,testY)
  LOCAL Float x1,x2,y1,y2
  LOCAL Integer t

  Tanks.isHitAndDestroy=0
  for t=0 to Tanks.cnt-1
    x1=Tanks.X(t)-Screen.VPx*PARALAX_LV3
    x2=x1+Tank.W
    y1=Tanks.Y(t)+20
    y2=y1+Tank.W-20

    if (testX>x1 AND TestX<x2 AND testY>y1 AND testY<y2) then
      Explosion.add Tanks.X(t)-Screen.VPx*PARALAX_LV3+Tank.W/2, Tanks.Y(t)+Tank.H/2
      Tanks.isHitAndDestroy=1
      Tanks.dead(t)=1
      exit for
    endif
  next t
end function

sub Tanks.Sound arg%
  STATIC Float Interval=TIMER
  STATIC Float limit=2000
  STATIC Integer state=0 '=off

  if (state=0 and arg%=1) then
    state=1 '=on
    playSample 12,22050,2
    Interval=TIMER
    limit=2000

  elseif (state=1 and arg%=0) then
    state=0 '=off
    playSample 13,22050,2

  else if (state=1 and TIMER-Interval>limit) then
    playSample 5,22050,2
    Interval=TIMER
    limit=5000
  endif
end sub



