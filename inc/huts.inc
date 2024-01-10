' Code to handle huts that contain 1-5 people
' Objects GAME, SCREEN and CObj must be initialized first
' Needs: Screen.GndLv, Screen.VPx, Game.VPw

CONST MAX_HUTS=30
CONST HUTS_PEOPLE=5

DIM Integer Hut.W=57
DIM Integer Hut.H=51

DIM Integer Huts.cnt
DIM Float   Huts.X(MAX_HUTS)
DIM Float   Huts.Y(MAX_HUTS)
DIM Integer Huts.People(MAX_HUTS)
DIM Integer Huts.Ambassador(MAX_HUTS)

sub Huts.init(level$)
  LOCAL Integer t,cnt
  LOCAL Float sector=Screen.W/len(level$)
  
  for t=1 to len(level$)
    if (mid$(level$,t,1) = "H") then
      Huts.X(cnt)=(t-1)*sector+sector/2
      Huts.Y(cnt)=Screen.GndLv - Hut.H
      Huts.People(cnt)=int(rnd*HUTS_PEOPLE+1)
      Huts.Ambassador(cnt)=-1
      addCollisionObj Huts.X(cnt),Huts.Y(cnt)+17,53,33
      addCollisionObj Huts.X(cnt)+34,Huts.Y(cnt),19,17
      cnt=cnt+1
    endif
  next
  Huts.cnt=cnt
end sub

function Huts.cntHumans()
  LOCAL Integer n,cnt

  for n=0 to Huts.cnt-1
    cnt=cnt+Huts.People(n)    
  next  
  Huts.cntHumans=cnt
end function

sub Huts.draw()
  STATIC Float interval=TIMER
  LOCAL Integer n,x,dist,dummy

  for n=0 to Huts.cnt-1
    x = Huts.X(n) - Screen.VPx
    dist=x+25-Player.X-Heli.W/2
    if x>-Hut.W and x<Game.VPw then
      if DEBUG then Text x+22,Huts.Y(n)+10,str$(Huts.People(n)),"C",7,,rgb(white)
      blit 540,120,x,Huts.Y(n),Hut.W,Hut.H,4,&B100
      
      if (abs(dist)<150 and Huts.People(n)>0) then
        if (Heli.isLanded()) then
          if(TIMER-interval>MAX_HUMANS_FREQ) then
            dummy=Humans.add(Huts.X(n)+25)
            Huts.People(n)=Huts.People(n)-1
            interval=TIMER
          endif
        elseif (Huts.Ambassador(n)=-1) then
          Huts.Ambassador(n)=Humans.add(Huts.X(n)+25)
          Humans.runToWave(Huts.Ambassador(n),Huts.X(n)+60)
          Huts.People(n)=Huts.People(n)-1
        endif
      endif
    endif

    if (Huts.Ambassador(n)<>-1) then
      if(Humans.isDead(Huts.Ambassador(n))) then Huts.Ambassador(n)=-1
      if (abs(dist)<150) then
        Humans.runToWave(Huts.Ambassador(n),Huts.X(n)+60)
      else        
        Humans.runToEnter(Huts.Ambassador(n),Huts.X(n)+25, "Huts._callback")
      endif
    endif
  next
end sub

sub Huts._callback(idx)
  LOCAL Integer n

  for n=0 to Huts.cnt-1
    if (Huts.Ambassador(n)=idx) then
      Huts.People(n)=Huts.People(n)+1
      Huts.Ambassador(n)=-1
      exit sub
    endif
  next
end sub

