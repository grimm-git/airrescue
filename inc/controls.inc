' Include controls.inc
' Contains code to  read player's input

CONST CTRL_RIGHT=0
CONST CTRL_LEFT =1
CONST CTRL_UP   =2
CONST CTRL_DOWN =3
CONST CTRL_FIRE =4
CONST CTRL_AUX  =5

DIM Float Controls.cnt(5)

sub Controls.init()
  On Error Skip
  Controller Nunchuk Open
  if MM.ERRNO = 0 then Game.Controls=1
end sub

sub Controls.measure(ctrl)
  LOCAL Integer t

  for t=0 to 5
    if (ctrl and (2^t)) then
      Controls.cnt(t)=Controls.cnt(t)+1
    else
      Controls.cnt(t)=0
    endif
  next
end sub

function Controls.getTime(idx)
  Controls.getTime=Controls.cnt(idx)
end function

' Returns player's input as bitcoded integer
' %0000 0000 - no input
' %0000 0001 - right
' %0000 0010 - left
' %0000 0100 - up
' %0000 1000 - down
' %0001 0000 - fire button
' %0010 0000 - auxillary button
function Controls.read()
  LOCAL Integer x,y,b,t
  LOCAL Integer ctrl=0
  LOCAL Integer keyCount=0

  if Game.Controls = 1 then
    x=Nunchuk(JX)
    if x<64 then ctrl=ctrl or 2
    if x>192 then ctrl=ctrl or 1
    y=Nunchuk(JY)
    if y<64 then ctrl=ctrl or 8
    if y>192 then ctrl=ctrl or 4
    b=Nunchuk(Z)
    if b=1 then ctrl=ctrl or 16 ' fire button
    b=Nunchuk(C)
    if b=1 then ctrl=ctrl or 32 ' auxillary button
  else
    keyCount=keydown(0)
    for t=1 to keyCount
      if keydown(t)=130 then ctrl=ctrl or 2
      if keydown(t)=131 then ctrl=ctrl or 1
      if keydown(t)=128 then ctrl=ctrl or 4
      if keydown(t)=129 then ctrl=ctrl or 8
      if keydown(t)=97 or keydown(t)=65 then ctrl=ctrl or 16  ' A key = fire button
      if keydown(t)=32  then ctrl=ctrl or 32 ' SPACE = auxillary button
    next t
  endif

  Controls.measure(ctrl)
  Controls.read = ctrl
end function

function Controls.readKey()
  Controls.readKey=0
  if keydown(0)<>0 then Controls.readKey=keydown(1)
end function



