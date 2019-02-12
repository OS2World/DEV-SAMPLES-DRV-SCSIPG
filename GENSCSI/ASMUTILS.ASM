              PAGE      60,131
              TITLE     GENERIC Device Driver Assembler Level Utilities
;
              .386P     ;    We amy want to use some 386 instructions
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                  CONSTANT DEFINITIONS                                     ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
TRUE          equ       1      ; Boolean values
FALSE         equ       0
REAL_TO_PROT  equ       02fh   ; This is the DevHlp cmd to go to PROTECT mode
PROT_TO_REAL  equ       030h   ; This is the DevHlp cmd to go to REAL mode
;
              PAGE
;
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                  CODE SEGMENT                                             ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
MAINSEG       SEGMENT   WORD PUBLIC 'CODE' USE16
;
              ASSUME    CS:MAINSEG
              public    _in_port
              public    _out_port
              public    _enable_irpt
              public    _disable_irpt
              public    _call_idc
              extrn     _dev_hlpCS:dword
;
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                                                                           ³
;³  in_port - Get a byte from an input port                                  ³
;³                                                                           ³
;³  Syntax - word far in_port(word)                                          ³
;³                                                                           ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
_in_port      proc      far
              push      bp
              mov       bp,sp
;
; Get the address of the port
;
              mov       dx,[bp+6]
;
; Do the input
;
              in        al,dx
;
; Clear the high half of the integer
;
              xor       ah,ah
;
; and back we go, with the value in AX (MS integer return convention)
;
              pop       bp
              ret
;
_in_port      endp
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                                                                           ³
;³  out_port - Write a byte to an output port                                ³
;³                                                                           ³
;³  Syntax - void far out_port(word, word)                                   ³
;³                             port  data                                    ³
;³                                                                           ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
_out_port     proc      far
              push      bp
              mov       bp,sp
;
; Get the address of the port
;
              mov       dx,[bp+6]
;
; and the data to output
;
              mov       ax,[bp+8]
;
; Output it (surprise!)
;
              out       dx,al
;
; All done
;
              pop       bp
              ret
;
_out_port     endp
              PAGE
;
;
;*****************************************************************************
;*                                                                           *
;*  enable_irpt() - Function to Enable CPU interrupts                        *
;*                                                                           *
;*  Syntax - void far enable_irpt(void)                                      *
;*                                                                           *
;*****************************************************************************
;
_enable_irpt  proc      far
              sti
              ret
;
_enable_irpt  endp
;
;
;*****************************************************************************
;*                                                                           *
;*  Disable_irpt() - Function to Disable CPU interrupts                      *
;*                                                                           *
;*  Syntax - void far enable_irpt(void)                                      *
;*                                                                           *
;*****************************************************************************
;
_disable_irpt proc      far
              cli
              ret
;
_disable_irpt endp
              PAGE
;
;
;ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
;³                                                                           ³
;³  call_idc - make a call to another DD                                     ³
;³                                                                           ³
;³  Syntax - void far call_idc(void *target, void *req_hdr, word ds_val)     ³
;³                             IDC entry     Request header DD's DS          ³
;³                             +6            +10            +14              ³
;ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
;
_call_idc     proc      far
              push      bp
              mov       bp,sp
              push      ds
;
; Load the Request header pointer
;
              les       bx,[bp+10]
;
; Set the target's DS
;
              mov       ax,[bp+14]
              mov       ds,ax
;
; Do the call
;
              call      dword ptr ss:[bp+6]
;
; All done
;
              pop       ds
              pop       bp
              ret
;
_call_idc     endp
              PAGE
;
MAINSEG       ENDS
;
              END
