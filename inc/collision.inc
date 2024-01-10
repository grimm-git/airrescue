' Collision detection code

CONST MAX_COBJS=100
CONST MAX_EXPLOSIONS=5
CONST SPEED_EXPLOSIONS=20

DIM Integer CObjs.used(MAX_COBJS)
DIM Float   CObjs.X(MAX_COBJS)
DIM Float   CObjs.Y(MAX_COBJS)
DIM Float   CObjs.W(MAX_COBJS)
DIM Float   CObjs.H(MAX_COBJS)

sub Collision.init()
  LOCAL Integer t

  for t=0 to MAX_COBJS
    CObjs.used(t)=0
  next  
end sub

' x,y in world coordinates
sub addCollisionObj(x,y,w,h)
  LOCAL Integer t

  for t=0 to MAX_COBJS
    if CObjs.used(t)=0 then
      CObjs.used(t)=1
      CObjs.X(t)=x
      CObjs.Y(t)=y
      CObjs.W(t)=w
      CObjs.H(t)=h
      exit sub
    endif
  next
end sub

function numCollisionObjs()
  LOCAL Integer cnt=0
  LOCAL Integer t

  for t=0 to MAX_COBJS
    if CObjs.used(t)=1 then cnt=cnt+1
  next
  numCollisionObjs=cnt
end function

sub remCollisionObj(x,y)
  LOCAL Integer t

  for t=0 to MAX_COBJS
    if (CObjs.used(t)=1 AND CObjs.X(t)=x AND CObjs.Y(t)=y) then
      CObjs.used(t) = 0
      exit sub
    endif
  next
end sub

' return 0=no collision, 1=collision with obstacle
function isHeliCollided() AS INTEGER
  LOCAL rectX(8)= ( 7, 5,41,41,41,17,21, 4,16)
  LOCAL rectY(8)= ( 5,18,19,19,19,17, 5,18,18)
  LOCAL rectW(8)= (68,73,17,17,17,75,69,76,74)
  LOCAL rectH(8)= (31,17,15,15,15,17,31,20,20)
  LOCAL Float x1,x2,y1,y2,x,y
  LOCAL Integer t,w,h
  
  x1=Player.X+rectX(Heli.State)
  x2=x1+rectW(Heli.State)
  y1=Player.Y+rectY(Heli.State)
  y2=y1+rectH(Heli.State)

  isHeliCollided=0
  for t=0 to MAX_COBJS
    if (CObjs.used(t)=1) then
      x=CObjs.X(t)-Screen.VPx: y=CObjs.Y(t)-Screen.VPy: w=Cobjs.W(t): h=CObjs.H(t)
      if not (x+w<x1 or x>x2 or y+h<y1 or y>y2) then
        isHeliCollided=1
        exit function
      endif
    endif
  next
end function

' ------------------------------------------------------
DIM Integer Explosion.W=int(400/8)
DIM Integer Explosion.H=int(196/4)
DIM Integer Explosion.frame(MAX_EXPLOSIONS)
DIM Float   Explosion.X(MAX_EXPLOSIONS)
DIM Float   Explosion.Y(MAX_EXPLOSIONS)

sub Explosion.init()
  LOCAL Integer t
  
  for t=0 to MAX_EXPLOSIONS-1
    Explosion.frame(t)=0
    Explosion.X(t)=0
    Explosion.Y(t)=0
  next
end sub

sub Explosion.add(x,y,ch)
  LOCAL Integer t

  for t=0 to MAX_EXPLOSIONS-1
    if (Explosion.frame(t)=0) then
      Explosion.frame(t)=1
      Explosion.X(t)=x-Explosion.W/2+Screen.VPx
      Explosion.Y(t)=y-Explosion.H/2
      PlaySample 11,22050,ch
      exit sub
    endif
  next
end sub

sub Explosion.draw
  STATIC interval=TIMER
  LOCAL Integer t,c,r,x,y,newFrame

  if (TIMER-interval>SPEED_EXPLOSIONS) then
    interval=TIMER
    newFrame=1
  endif

  for t = 0 to MAX_EXPLOSIONS-1
    if (Explosion.frame(t) > 0) then

      x = Explosion.X(t)-Screen.VPx
      y = Explosion.Y(t)-Explosion.frame(t)

      c = Explosion.frame(t) mod 8
      r = int(Explosion.frame(t) / 8)
      blit c*Explosion.W,r*Explosion.H,x,y,Explosion.W,Explosion.H,5,&B100
      Explosion.frame(t)=(Explosion.frame(t)+newFrame) AND 31
    endif
  next
end sub


