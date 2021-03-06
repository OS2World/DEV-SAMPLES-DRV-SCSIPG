              PAGE      60,131
              TITLE     GENERIC Device Driver Assembler Level Utilities
;
              .386P     ;    We amy want to use some 386 instructions
;
;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
;�                  CONSTANT DEFINITIONS                                     �
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
;
TRUE          equ       1      ; Boolean values
FALSE         equ       0
REAL_TO_PROT  equ       02fh   ; This is the DevHlp cmd to go to PROTECT mode
PROT_TO_REAL  equ       030h   ; This is the DevHlp cmd to go to REAL mode
;
              PAGE
;
;
;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
;�                  CODE SEGMENT                                             �
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
;�                                                                           �
;�  in_port - Get a byte from an input port                                  �
;�                                                                           �
;�  Syntax - word far in_port(word)                                          �
;�                                                                           �
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
;�                                                                           �
;�  out_port - Write a byte to an output port                                �
;�                                                                           �
;�  Syntax - void far out_port(word, word)                                   �
;�                             port  data                                    �
;�                                                                           �
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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
;旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
;�                                                                           �
;�  call_idc - make a call to another DD                                     �
;�                                                                           �
;�  Syntax - void far call_idc(void *target, void *req_hdr, word ds_val)     �
;�                             IDC entry     Request header DD's DS          �
;�                             +6            +10            +14              �
;읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
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