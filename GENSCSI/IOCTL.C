/****************************************************************************/
/* IOCtl command handler                                                    */
/****************************************************************************/
/* We really only handle 1 IOCtl ourself - the breakpoint command.  This    */
/* IOCtl is a debugging tool.  It allows our DD to load and then, we can    */
/* get control with a debugger (ASDT32 or the kernel debugger, for example) */
/* to set real breakpoints.  All other IOCtls are passed to the OS2SCSI.DMD */
/* through the IDC interface.                                               */
/****************************************************************************/
#define INCL_DOSDEVICES
#define INCL_DOSMISC
#include <os2.h>
#include "genscsi.h"


word far ioctl(reqhdr_type *in_req)
{
   ioctl_hdr2    *req;

   /* Cast the request header to the right kind of pointer */
   req = (ioctl_hdr2 *)in_req;

   /* Handle breakpoint */
   if ((req->funct_cat == 0x81) && (req->funct_cod == 0x40)) {
       breakpoint();
       req->request_hdr.rh_stat = DONE;
       return(DONE);
       }

   /* otherwise, pass it to the SCSI DD */
   return(call_scsi(in_req));

}
