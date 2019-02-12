/*****************************************************************************/
/* Routines for managing LINEAR accessed memory                              */
/*                                                                           */
/*****************************************************************************/
#include "genscsi.h"

/* Create a Linear address in the current process from a 16:16 bit pointer */
_32bits far virt_to_lin(_32bits ptr_16)
{
  union cpu_regs32 in_regs,out_regs;
  _32bits          ret_val;

  in_regs.D.EAX = 0L;
  in_regs.W.AX = ptr_16._segadr.segment;
  in_regs.D.ESI = 0L;
  in_regs.W.SI = ptr_16._segadr.offset;
  in_regs.D.EDX = 0L;
  in_regs.B.DL = devhlp_VirtToLin;           /* DevHlp command               */
  in_regs.W.es_valid = FALSE;                /* Use current ES and DS regs   */
  in_regs.W.ds_valid = FALSE;
  dev_help32(&in_regs,&out_regs);
                                             /* If failure                   */
  if ((out_regs.W.flags & 0x0001) != 0) {    /*    Return -1                 */
     ret_val.phys = 0;
     }                                       /* Else                         */
  else
     {
     ret_val.phys = out_regs.D.EAX;
     }
  return(ret_val);                           /*    Return 0                  */
}


/* Creates a linear address from a page list (scatter/gather list) */
void far lin_to_pages(_32bits addr, _32bits pages, _32bits count)
{
  union cpu_regs32 in_regs,out_regs;

  in_regs.D.EAX = addr.phys;
  in_regs.D.ECX = count.phys;
  in_regs.D.EDI = pages.phys;
  in_regs.B.DL = devhlp_LinToPageList;       /* DevHlp command               */
  in_regs.W.es_valid = FALSE;                /* Use current ES and DS regs   */
  in_regs.W.ds_valid = FALSE;
  dev_help32(&in_regs,&out_regs);
                                             /* If failure                   */
  return;                                   /*    Return 0                  */
}


