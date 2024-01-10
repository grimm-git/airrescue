' Code to handle camps that contain 5-10 people
' Objects GAME, SCREEN and CObj must be initialized first
' Needs: Screen.GndLv, Screen.VPx, Game.VPw

CONST MAX_CAMPS=10
CONST CAMPS_PEOPLE=10

DIM Integer Camp.W=120
DIM Integer Camp.H=61
DIM Integer Camp.doorX=70
DIM Integer Camp.waveX=116

DIM Integer Camps.cnt
DIM Float   Camps.X(MAX_CAMPS)
DIM Float   Camps.Y(MAX_CAMPS)
DIM Integer Camps.People(MAX_CAMPS)
DIM Integer Camps.destroyed(MAX_CAMPS)
DIM Integer Camps.Ambassador(MAX_CAMPS)

sub Camps.init(level$)
  LOCAL Integer t,cnt
  LOCAL Float sector=Screen.W/len(level$)
  
  for t=1 to len(level$)
    if (mid$(level$,t,1)="C") then
      Camps.X(cnt)=(t-1)*sector+sector/2
      Camps.Y(cnt)=Screen.GndLv - Camp.H
      Camps.People(cnt)=int(rnd*CAMPS_PEOPLE+5)
      Camps.destroyed(cnt)=0
      Camps.Ambassador(cnt)=-1
      addCollisionObj Camps.X(cnt),Camps.Y(cnt)+28,121,33
      addCollisionObj Camps.X(cnt)+95,Camps.Y(cnt),21,28
      cnt=cnt+1
    endif
  next
  Camps.cnt=cnt
end sub

function Camps.cntHumans()
  LOCAL Integer n,cnt

  for n=0 to Camps.cnt-1
    cnt=cnt+Camps.People(n)
  next
  Camps.cntHumans=cnt
end function

sub Camps.draw()
  STATIC Float interval=TIMER
  LOCAL Integer n,x,dist,dummy

  for n=0 to Camps.cnt-1
    x = Camps.X(n) - Screen.VPx
    dist=x+Camp.doorX-Player.X-Heli.W/2
    if x>-Camp.W and x<Game.VPw then
      if DEBUG then Text x+48,Camps.Y(n)+20,str$(Camps.People(n)),"C",7,,rgb(white)
      if Camps.destroyed(n) = 0 then
        blit 298,120,x,Camps.Y(n),Camp.W,Camp.H,4,&B100
      else
        blit 420,120,x,Camps.Y(n),Camp.W,Camp.H,4,&B100

        if (abs(dist)<150 and Camps.People(n)>0) then
          if (Heli.isLanded()) then
            if(TIMER-interval>MAX_HUMANS_FREQ) then
              dummy=Humans.add(Camps.X(n)+Camp.doorX)
              Camps.People(n)=Camps.People(n)-1
              interval=TIMER
            endif
          elseif (Camps.Ambassador(n)=-1) then
            Camps.Ambassador(n)=Humans.add(Camps.X(n)+Camp.doorX)
            Humans.runToWave(Camps.Ambassador(n),Camps.X(n)+Camp.waveX)
            Camps.People(n)=Camps.People(n)-1
          endif
        endif
      endif
    endif

    if (Camps.Ambassador(n)<>-1) then
      if(Humans.isDead(Camps.Ambassador(n))) then Camps.Ambassador(n)=-1
      if (abs(dist)<150) then
        Humans.runToWave(Camps.Ambassador(n),Camps.X(n)+Camp.waveX)
      else        
        Humans.runToEnter(Camps.Ambassador(n),Camps.X(n)+Camp.doorX, "Camps._callback")
      endif
    endif
  next
end sub

'testX and testY in ViewPort coordinates
function Camps.isHitAndDestroy(testX,testY)
  LOCAL Float x1,x2,y1,y2
  LOCAL Integer t

  Camps.isHitAndDestroy=0
  for t=0 to Camps.cnt-1
    x1=Camps.X(t)-Screen.VPx+95
    x2=x1+21
    y1=Camps.Y(t)
    y2=y1+Camp.H

    if (testX>x1 AND TestX<x2 AND testY>y1 AND testY<y2) then
      Explosion.add Camps.X(t)-Screen.VPx+105, Camps.Y(t)+Camp.H-4
      Camps.isHitAndDestroy=1
      Camps.destroyed(t)=1
      exit for
    endif
  next t
end function

sub Camps._callback(idx)
  LOCAL Integer n

  for n=0 to Camps.cnt-1
    if (Camps.Ambassador(n)=idx) then
      Camps.People(n)=Camps.People(n)+1
      Camps.Ambassador(n)=-1
      exit sub
    endif
  next
end sub



