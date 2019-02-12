scsitest.exe : scsitest.obj
   link386 @scsitest.arf > scsitest.lnk

scsitest.obj : scsitest.c
   icc -c -Ti+ -Gm -Ms -Sp1 -Ti+ scsitest.c >  scsitest.msg
