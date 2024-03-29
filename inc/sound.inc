
Sub PlaySample(N,S,ch)
  LOCAL t

  if (Game.State=STATE_INTRO) then exit sub

  if (ch>0) then
    play MODSAMPLE N,ch,64,S
    exit sub
  else
    for t=1 to 4
      if MM.INFO(SAMPLE PLAYING t)=0 THEN PLAY MODSAMPLE N,t,64,S: exit for
    next t
  endif
End Sub

' -- PLAY DIGITAL SOUND EFFECTS WITH MOD MUSIC --
Sub playWAV(sndname$,priority)
 if priority>=PrWav then 
  PrWav=priority  
  if MM.INFO$(SOUND)="MODFILE" then
    play Effect WE.PROG_DIR+"/sfx/"+sndname$+".wav",StopWAV
  else
    play stop
    play WAV WE.PROG_DIR+"/sfx/"+sndname$+".wav",StopWAV
  endif
 endif
End Sub

' -- PLAY MOD FILE WHEN ENABLED --
Sub playMOD name$
  play Stop
  play MODFile WE.PROG_DIR+"/"+name$+".mod", 22050
End Sub


' -- NOTIFY PRIORITY DIGITAL SFX AS ZERO --
Sub StopWAV
  PrWav=0
End Sub



