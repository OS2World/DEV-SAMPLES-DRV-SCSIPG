/*****************************************************************************/
/* General Device Driver Utilities                                           */
/*****************************************************************************/
/* This file contains a bunch of general utility functions all DDs need.     */
/*****************************************************************************/
/*                                                                           */
/* void far dev_done(req,status)                                             */
/* reqhdr_type *req;                                                         */
/* word status;                                                              */
/*                                                                           */
/* dev_done - Set the status in the request packet, set the done bit and     */
/*            unblock the process.                                           */
/*                                                                           */
/* Only return code is SUCCESS.  This is the only return code from the       */
/* devhlp function that is called.                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* _32bits far alloc_req(void)                                               */
/*                                                                           */
/* alloc_req - Allocate a request packet.  This request packet will be       */
/*             passed to another process and freed (via free_req) when it    */
/*             is done.                                                      */
/*                                                                           */
/* The return value is a far pointer to the allocted request packet.  If the */
/* request failed, the return value is 0000:0000.                            */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* void far free_req(void)                                                   */
/*                                                                           */
/* free_req - Free a request packet allocated by alloc_req (above).          */
/*                                                                           */
/* There is no return code.  We don't care if it worked or not.              */
/*                                                                           */
/*****************************************************************************/
#include <string.h>
#include "genscsi.h"

void far dev_done(req,status)
reqhdr_type *req;
word status;
{
  union cpu_regs in_regs,out_regs;
  _32bits pointer;

  /* Set the status field in the request packet to the requested value */
  req->rh_stat = status;

  pointer.phys = (physaddr)req;
  in_regs.W.BX = pointer._segadr.offset;   /* BX = offset of request packet  */
  in_regs.W.ES = pointer._segadr.segment;  /* ES = segment of request packet */
  in_regs.B.DL = devhlp_DevDone;           /* DL = Dev_done command          */
  in_regs.W.es_valid = TRUE;               /* ES has a valid selector        */
  in_regs.W.ds_valid = FALSE;              /* DS is not a valid selector     */
  dev_help(&in_regs,&out_regs);            /* Do it!                         */
  return;                                  /* We don't care what the return  */
                                           /*    code is because DevHlp does */
                                           /*    not return anything but OK  */
}

_32bits far alloc_req(void)
{
  union cpu_regs in_regs,out_regs;
  _32bits pointer;

  in_regs.B.DH = 0;                        /* Wait for one                   */
  in_regs.B.DL = devhlp_AllocReqPacket;    /* DL = Dev_done command          */
  in_regs.W.es_valid = FALSE;              /* ES is not a valid selector     */
  in_regs.W.ds_valid = FALSE;              /* DS is not a valid selector     */
  dev_help(&in_regs,&out_regs);            /* Do it!                         */
  if ((out_regs.W.flags & 0x0001) != 0) {  /* If it failed           */
     pointer.phys = 0L;
     return(pointer);                      /*    Return 0            */
     }                                     /* Endif                  */
  pointer._segadr.segment = out_regs.W.ES;
  pointer._segadr.offset  = out_regs.W.BX;
  return(pointer);                         /* Return packet pointer          */
}

void far free_req(_32bits ptr)
{
  union cpu_regs in_regs,out_regs;

  in_regs.W.BX = ptr._segadr.offset;       /* BX = offset of request packet  */
  in_regs.W.ES = ptr._segadr.segment;      /* ES = segment of request packet */
  in_regs.B.DL = devhlp_FreeReqPacket;     /* DL = Dev_done command          */
  in_regs.W.es_valid = TRUE;               /* ES has a valid selector        */
  in_regs.W.ds_valid = FALSE;              /* DS is not a valid selector     */
  dev_help(&in_regs,&out_regs);            /* Do it!                         */
  return;                                  /* We don't care what the return  */
                                           /*    code is because DevHlp does */
                                           /*    not return anything but OK  */
}
