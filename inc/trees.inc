' Code to handle trees, just obstacles for the helicopter
' Objects GAME, SCREEN and CObj must be initialized first
' Needs: Screen.GndLv, Screen.VPx, Game.VPw

CONST MAX_TREES=50

DIM Integer Trees.cnt
DIM Integer Trees.W=24
DIM Integer Trees.H=51
DIM Integer Trees.O(MAX_TREES)
DIM Float   Trees.X(MAX_TREES)
DIM Float   Trees.Y(MAX_TREES)


sub Trees.init(level$)
  LOCAL Integer t
  LOCAL Float sector=Screen.W/len(level$)

  Trees.cnt=0
  for t=1 to len(level$)
    if (mid$(level$,t,1)="T") then
      Trees.X(Trees.cnt)=(t-1)*sector+sector/2
      Trees.Y(Trees.cnt)=Screen.GndLv - Trees.H
      Trees.O(Trees.cnt)=&B100+int(rnd+0.5)
      addCollisionObj Trees.X(Trees.cnt),Trees.Y(Trees.cnt),Trees.W,Trees.H
      Trees.cnt=Trees.cnt+1
    endif
  next
end sub

sub Trees.draw()
  LOCAL Integer n
  LOCAL Integer x

  for n=0 to Trees.cnt-1
    x = Trees.X(n) - Screen.VPx
    if x>-Trees.W and x<Game.VPw then
      blit 597,120,x,Trees.Y(n),Trees.W,Trees.H,4,Trees.O(n)
    endif
  next
end sub
