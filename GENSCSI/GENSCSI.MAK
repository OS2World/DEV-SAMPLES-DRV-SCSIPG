#make file for genscsi DD
#bin=d:\bin
src=.\src
lst=.\lst
obj=.\obj
msg=.\msg
lib=.
h=.\src
optmain=/Fc$(lst)\$(*F).cod /c /NTMAINSEG /W3 /Gs /Alfw /Od /Zp /Fo$(obj)\$(*F).obj >$(msg)\$(*F).msg
optsize=/Fc$(lst)\$(*F).cod /c /NTMAINSEG /W3 /Gs /Alfw /Os /Zp /Fo$(obj)\$(*F).obj >$(msg)\$(*F).msg
optkeep=/Fc$(lst)\$(*F).cod /c /NTKEEPSEG /W3 /Gs /Alfw /Od /Zp /Fo$(obj)\$(*F).obj >$(msg)\$(*F).msg
optinit=/Fc$(lst)\$(*F).cod /c /NTINITSEG /W3 /Gs /Alfw /Od /Zp /Fo$(obj)\$(*F).obj >$(msg)\$(*F).msg
hfiles=$(h)\externs.h $(h)\const.h $(h)\struc.h $(h)\genscsi.h

genscsi.sys : $(lib)\genscsi.lib
   link @genscsi.arf > $(msg)\link.lst
   copy genscsi.dll genscsi.sys
   erase genscsi.dll

$(lib)\genscsi.lib : genscsi.arf                        \
                            $(obj)\ASMUTILS.OBJ  \
                            $(obj)\brkpoint.obj  \
                            $(obj)\DDUTILS.OBJ   \
                            $(obj)\DEVHLP.OBJ    \
                            $(obj)\GDTMEM.OBJ    \
                            $(obj)\genscsi.OBJ   \
                            $(obj)\INIT.OBJ      \
                            $(obj)\LDTMEM.OBJ    \
                            $(obj)\IOCTL.OBJ     \
                            $(obj)\LINMEM.OBJ    \
                            $(obj)\LOCK.OBJ      \
                            $(obj)\openclos.OBJ  \
                            $(obj)\PRTMSG.OBJ    \
                            $(obj)\scsi.OBJ      \
                            $(obj)\STRATEGY.OBJ  \
                            genscsi.MSG;

$(obj)\openclos.obj : $(src)\openclos.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\ldtmem.obj : $(src)\ldtmem.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\ioctl.obj : $(src)\ioctl.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\linmem.obj : $(src)\linmem.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\gdtmem.obj : $(src)\gdtmem.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\init.obj : $(src)\init.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\strategy.obj : $(src)\strategy.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\scsi.obj : $(src)\scsi.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\ddutils.obj : $(src)\ddutils.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\lock.obj : $(src)\lock.c $(hfiles)
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\prtmsg.obj : $(src)\prtmsg.c $(hfiles) $(src)\version.h
   cl $(optmain) $(src)\$(*F).c
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\asmutils.obj : $(src)\asmutils.asm
   masm $(src)\$(*F),$(obj)\$(*F).obj,$(lst)\$(*F).lst > $(msg)\$(*F).msg;
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\brkpoint.obj : $(src)\brkpoint.asm
   masm $(src)\$(*F),$(obj)\$(*F).obj,$(lst)\$(*F).lst > $(msg)\$(*F).msg;
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\devhlp.obj : $(src)\devhlp.asm
   masm $(src)\$(*F),$(obj)\$(*F).obj,$(lst)\$(*F).lst > $(msg)\$(*F).msg;
   lib $(lib)\genscsi-+$(obj)\$(*F);

$(obj)\genscsi.obj : $(src)\genscsi.asm
   masm $(src)\genscsi,$(obj)\genscsi.obj,$(lst)\genscsi.lst > $(msg)\$(*F).msg;

genscsi.msg : genscsi.txt
   mkmsgf genscsi.txt genscsi.msg

