              PAGE      60,131
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                                                                           ³
;³ Generic SCSI Device Driver                                                ³
;³                                                                           ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
              .seq                         ;    Use segments in order listed
              .386c                        ;    Must use 386 mode if OS/2
              TITLE     Generic SCSI Device Driver
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                  ATTRIBUTE EQUATES                                        ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
CHR           equ       8000H              ;    Bit set if character device
IDC           equ       4000H              ;    Inter DD communications enabled
NIBM          equ       2000H              ;    Bit set if NON-IBM Block Format
SHAR          equ       1000H              ;    Set to support shrd dev. access
RM            equ       0800H              ;    Set if Removable Media (Blk)
OPN           equ       0800H              ;    Set if Device Open/Close (Char)
OS2           equ       0080H              ;    OS/2 Function Level DD
CLK           equ       0008H              ;    Clock Device
NUL           equ       0004H              ;    Null Device
SCR           equ       0002H              ;    Std Output Device (SCREEN)
KBD           equ       0001H              ;    Std Input Device (KEYBOARD)
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                  EXTERNAL DECLARATIONS                                    ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
              EXTRN     _strategy_c:near
;
;
; Automatic Data Grouping
DGROUP        GROUP     NULL,_DATA,CONST,_BSS,LAST_D,FAR_BSS

; Automatic Code Grouping
CGROUP        GROUP     MAINSEG,END_TEXT,_TEXT
;
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                  DATA SEGMENT                                             ³
;³                                                                           ³
;³ This is the start of the DD's data segment.  The very first thing is the  ³
;³ device driver header.  Our DD is a character DD that supports OPEN/CLOSE. ³
;³ All of our work will be via IOCtls.                                       ³
;³                                                                           ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
; Device Driver Header must be the first item in the DATA group
NULL          SEGMENT   WORD PUBLIC 'BEGDATA' USE16
;             ASSUME    CS:CGROUP, DS:DGROUP, ES:nothing
              public    header
;             public    __acrtused
              public    _devhlp
;
header        dd        -1                 ; Pointer to next dd header
attrib        dw        OS2+CHR+OPN        ; OS/2 Dev,
                                           ; Char. Device,
                                           ; Open/Close,
                                           ; No IDC support

              dw        offset CGROUP:_strat_entry       ; point to Strategy Routine
              dw        0                  ; No IDC
              db        '$GENSCSI'         ; Name Field (Must be 8 bytes)
              dq        (0)                ; Reserved for OS/2
              PAGE
_devhlp       dd        ?                  ; DevHlp entry, recv'd on Init Call
;
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ These next empty segments are here to let the .seq command above work like³
;³ we want.  We want it to put segment LAST_D at the end of the data group.  ³
;³ We do this by saying .seq which says to group them as encountered.  Then  ³
;³ we tell it which ones to group together by using the GROUP command.  Then ³
;³ we put in empty ones for it to encounter.  These have the same names and  ³
;³ attributes as the segments the C compiler will produce.  After we have    ³
;³ shown the assembler and linker all the C procuced segments, we show him   ³
;³ our end of segment marker (LAST_D).  The linker will put all segments with³
;³ the same name and attributes together and in the order we specify here.   ³
;³                                                                           ³
;³ We then do the same thing with the code segments.  The GROUP command tells³
;³ which segments to group together, and the order they appear here is the   ³
;³ order they will appear in the .SYS file.                                  ³
;³                                                                           ³
;³ The final 'trick' is to specify the object file from this source file     ³
;³ first when linking.  My method is to put ALL others in a library, and just³
;³ specify this module's .OBJ file in the linker Automatic Response File     ³
;³ (.ARF).                                                                   ³
;³                                                                           ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
NULL          ENDS
_DATA         SEGMENT   WORD PUBLIC 'DATA' USE16
_DATA         ENDS
CONST         SEGMENT   WORD PUBLIC 'CONST' USE16
CONST         ENDS
FAR_BSS       SEGMENT   WORD PUBLIC 'FAR_BSS' USE16
FAR_BSS       ENDS
_BSS          SEGMENT   WORD PUBLIC 'BSS' USE16
_BSS          ENDS
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ The next segment MUST be the last data segment to appear.  It marks the   ³
;³ end of the data segment, allowing INIT to calculate how big it is, and    ³
;³ free the rest.                                                            ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
LAST_D        SEGMENT   WORD PUBLIC 'LAST_DATA' USE16
              public    _last_d
_last_d       equ       $                       ; Marks the end of the data
LAST_D        ENDS
              PAGE
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                  CODE SEGMENT                                             ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
MAINSEG       SEGMENT   WORD PUBLIC 'CODE' USE16
;
              ASSUME    CS:MAINSEG, DS:DGROUP, ES:nothing
              public    _strategy
              public    __chkstk
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ The STRATEGY Entry Point.  (ta da)  It pushes the pointer to the request  ³
;³ packet and calls the Strategy routine written in C. This lets the C       ³
;³ function see the pointer as a parameter passed in the usual manner.       ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
_strategy     PROC      FAR
_strat_entry  equ       $
;
              push      es
              push      bx
              call      _strategy_c           ; call C strategy routine
              pop       bx
              pop       es
              ret
;
_strategy     ENDP
;
;
              PAGE
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ This is a replacement for the C compiler's stack checking routine.  It    ³
;³ is declared as external by the compiler, but it should never be called    ³
;³ as we compile with all stack probes removed.  If, however, it should get  ³
;³ called, it allocates the local variables like the compiler's __chkstk,    ³
;³ without generating an error if the stack is not big enough.               ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
__chkstk      proc      far
              pop       cx
              pop       dx
              mov       bx,sp
              sub       bx,ax
              mov       sp,bx
              push      dx
              push      cx
              ret
__chkstk      endp
;
MAINSEG       ENDS
;
_TEXT         SEGMENT   WORD PUBLIC 'CODE' USE16
_TEXT         ends
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³ The next segment marks the end of the code to be kept.  The C compiler    ³
;³ will call all its segments _TEXT, so they will be combined with this one  ³
;³ Our discardable segments will be tacked on after END_TEXT, and will be    ³
;³ discarded after INIT is done.                                             ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
END_TEXT      SEGMENT   WORD PUBLIC 'CODE' USE16
              public    _last_c
_last_c       equ       $
END_TEXT      ends
;
INITSEG       SEGMENT   WORD PUBLIC 'CODE' USE16
INITSEG       ends
              END
