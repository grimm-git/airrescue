' Module to control the Base and other background objects
' Code by "Matthias Grimm"

CONST PARALAX_LV1=1     '8/8
CONST PARALAX_LV2=1.25  '10/8
CONST PARALAX_LV3=1.5   '12/8
CONST SPEED_BASIS=100
CONST MAX_MISSILELIFE=100

DIM Integer Basis.W=149
DIM Integer Basis.H=45
DIM Integer Basis.X=Screen.W/2
DIM Integer Basis.Y=Screen.GndLv-Basis.H
DIM Integer Basis.idx
DIM Integer Basis.Humans

sub Basis.init()
  Basis.Humans=0
  addCollisionObj Basis.X+84, Basis.Y+12, 56, 32  ' base building
end sub

sub Basis.draw()
  STATIC interval=TIMER
  STATIC idx=0
  LOCAL Integer x

  if (TIMER-interval>SPEED_BASIS) then
    idx=int(rnd+0.5)
    interval=TIMER
  endif

  x = Basis.X - Screen.VPx
  if x < -Basis.W or x > Game.VPw then exit sub

  blit 0+idx*Basis.W,120,x,Basis.Y,Basis.W,Basis.H,4,&B100
end sub

sub Basis.rescueHuman(idx)
  Basis.Humans=Basis.Humans+1
  PlaySample 1,22050
end sub

'--------------------------------------------------------
DIM Integer Pebbles.cnt = 40
DIM Float   Pebbles.X(3, Pebbles.cnt)
DIM Float   Pebbles.Y(3, Pebbles.cnt)

sub Pebbles.init()
  LOCAL Integer t
  LOCAL Integer i

  for t = 0 to 2
    for i = 0 to Pebbles.cnt
      select case t
      case 0
        Pebbles.X(0,i) = 8000 * PARALAX_LV1 * RND
        Pebbles.Y(0,i) = Screen.GndLv - 5 + int(rnd*2)
      case 1
        Pebbles.X(1,i) = 8000 * PARALAX_LV2 * RND
        Pebbles.Y(1,i) = Screen.GndLv + 10 + int(rnd*18)
      case 2
        Pebbles.X(2,i) = 8000 * PARALAX_LV3 * RND
        Pebbles.Y(2,i) = Screen.GndLv + 30 + int(rnd*15)
      end select
    next
  next
end sub

sub Pebbles.draw()
  LOCAL Integer i
  LOCAL Integer x
  LOCAL Integer cnt = 0

  for i = 0 to Pebbles.cnt
    x = Pebbles.X(0,i) - Screen.VPx * PARALAX_LV1
    if x > -5 and x < Game.VPw then
      blit 550,171,x,Pebbles.Y(0,i),5,3,4,&B100
      cnt = cnt +1
    endif

    x = Pebbles.X(1,i) - Screen.VPx * PARALAX_LV2
    if x > -5 and x < Game.VPw then
      blit 545,171,x,Pebbles.Y(1,i),5,3,4,&B100
      cnt = cnt +1
    endif

    x = Pebbles.X(2,i) - Screen.VPx * PARALAX_LV3
    if x > -5 and x < Game.VPw then
      blit 540,171,x,Pebbles.Y(2,i),5,3,4,&B100
      cnt = cnt +1
    endif
  next
end sub

'--------------------------------------------------------
DIM Integer Sam.W=34
DIM Integer Sam.H=19
DIM Integer Sam.state

DIM Integer Sams.X(1)
DIM Integer Sams.Y(1)

DIM Integer Missile.life
DIM Float   Missile.X
DIM Float   Missile.Y
DIM Float   Missile.dX
DIM Float   Missile.dY

sub Sams.init()
  Sams.X(0)=200
  Sams.Y(0)=Screen.GndLv-Sam.H
  Sams.X(1)=Screen.W-Sam.W-200
  Sams.Y(1)=Screen.GndLv-Sam.H
  Sam.state=0
  Missile.life=0
end sub

sub Sams.draw()
  LOCAL Integer t,x
  LOCAL Float dist

  for t=0 to 1
    x = Sams.X(t) - Screen.VPx
    if x>-Sam.W AND x<Game.VPw then
      blit Sam.state*Sam.W,210,x,Sams.Y(t),Sam.W,Sam.H,4,&B100+t
      dist=abs(Player.X-x)-Heli.W/2
      if (dist<Game.VPw/3 and Missile.life=0) then Sams.fire t
    endif
  next
end sub

sub Sams.fire(sam)
  if (Missile.life=0) then
    Missile.life=MAX_MISSILELIFE
    Missile.X=Sams.X(sam)+Sam.W/2
    Missile.Y=Sams.Y(sam)+Sam.H/2
    if (sam=0) then
      Missile.dX=0.2
    else
      Missile.dX=-0.2
    endif
    Missile.dY=-4
    Sam.state=1
    PlaySample 7,22050
  endif
end sub

sub Sams.moveAndDrawMissile
  STATIC Float interval=TIMER
  LOCAL Integer x
  LOCAL Float dx,dy,a1,a2

  if (Missile.life>0) then
    Missile.life=Missile.life-1

    x=Missile.X - Screen.VPx
    dx=Player.X+Heli.W/2-x
    dy=Player.Y+Heli.H/2-Missile.Y
    a1=atan2(dy,dx)
    a2=atan2(Missile.dY,Missile.dX)

    a1=a1+(a2-a1)/100
    Missile.dX=4*cos(a1)
    Missile.dY=4*sin(a1)        
    Missile.X=Missile.X+Missile.dX
    Missile.Y=Missile.Y+Missile.dY

    if (x>Game.VPx AND x<Game.VPX+Game.VPw) then
      line  x-Missile.dX,Missile.Y-Missile.dY,x,Missile.Y,,rgb(white)
      pixel x,Missile.Y,rgb(red)
      if (Heli.isHit(x,Missile.Y)) then Player.destroy
    else
      Missile.life=0
    endif
  else
    Sam.state=0
  endif
end sub

'--------------------------------------------------------
sub Hills.draw()
  LOCAL Integer range
  LOCAL float scale
  LOCAL Integer ofs

  range = 1280 - Game.VPw
  if range < 0 then exit sub

  scale = range / (Screen.W - Game.VPw)
  ofs = int(scale * Screen.VPx)

  ' width = 640, height = 60, in two rows on page 4
  blit ofs,0,0,274,640-ofs,60,4,&B100
  blit 0,60,640-ofs,274,640,60,4,&B100
end sub


