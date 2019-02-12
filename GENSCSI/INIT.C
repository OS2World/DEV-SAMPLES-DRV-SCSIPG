/*****************************************************************************/
/* init_mod(req_pkt)                                                         */
/*                                                                           */
/* Called by Strategy, this function executes the INIT command from OS/2.    */
/*                                                                           */
/* Store devhlp entry point for use later by dev_help                        */
/* Display the information about this DD (version, copywrite, etc)           */
/* Do whatever initialization your particular DD needs                       */
/* Set the endpoints of the default CODE and DATA segments                   */
/* At any of these steps, if an error is detected (some steps don't do any   */
/* error checking because there is none to do), the INIT ends, and the DD    */
/* returns a FAIL error.  In essence, it fails to install.                   */
/*                                                                           */
/* Finally, it sets up a pointer to Global Information Segment.  This has    */
/* the Milliseconds since IPL value and the current PID.  These are useful   */
/* things to know.                                                           */
/*                                                                           */
/* There is a bit of razzle-dazzle in this function.  At the very beginning, */
/* the DevHlp entry point must be stored in a place where the DevHlp caller  */
/* can get to it.  Due to the nature of DevHlp calls, the caller needs to    */
/* have it addressable by CS.  To do this, the entry point must be stored in */
/* the CODE segment.  But, the CODE segment is not writable.  Therefore, an  */
/* alias selector (Data, Writeable) must be created.  Another problem is     */
/* that to do this uses for calls to DevHlp. So, the basic steps are:        */
/* 1. Store the DevHlp entry point in a temporary location in the Data Seg   */
/* 2. Get the physical address of the DevHlp storage location in the Code    */
/*    segment.  This uses the temporary DevHlp caller that uses the DevHlp   */
/*    entry point stored in the DATA segment                                 */
/* 3. Make an LDT based pointer to the physical address gotten in step 2.    */
/*    This two uses the temporary DevHlp caller                              */
/* 4. Store the DevHlp entry point in the address gotten in step 3.  From    */
/*    this point on, we can use the CS based DevHlp caller.                  */
/* 5. Free the LDT selector slot used in steps 2 and 3.                      */
/*                                                                           */
/* The reason for using CS as the segment pointer to the DevHlp entry point  */
/* is that CS is the only segment always available.  Some DevHlp calls use   */
/* DS and others use ES, the SS and CS are the only ones left.  I chose to   */
/* pay the price at INIT time and store the entry in CS, and not always      */
/* pass it in the stack (which, by the way is a perfectly valid way to do    */
/* it).                                                                      */
/*                                                                           */
/*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "genscsi.h"

extern unsigned far   last_c;
extern _32bits  near  devhlp;
extern unsigned far  *dev_hlpCS();
extern unsigned near  last_d;
       boolean  near  initialized = FALSE;

unsigned far init_mod( struct init_hdr_in *req_hdr)
{
   _32bits             *temp;
   _32bits              pointer;
   _32bits              tempaddr;
   struct init_hdr_out *req_hdr_out;
   word                 rc;

   /* Store the DevHlp entry point in our DS                             */
   /* This one is used only for the next section, which gets the devhlp  */
   /* entry point stored in the DD's CS.  This lets me have 1 devhlper   */
   /* that does not depend on if DS or ES is holding a pointer.  devhelp */
   /* will use CS to access the devhlp entry                             */
   devhlp = req_hdr->pointer1;

   /* Store the DevHlp entry point in our CS */
   /*    Get the phys addr of dev_hlpCS */
   pointer.fptr = (void far *)dev_hlpCS;
   tempaddr = get_phys_addr1(pointer);

   /*    Make an LDT selector point to it */
   pointer = phys_to_ldt1(tempaddr,32);
   temp = (_32bits *)pointer.fptr;

   /*    Store the devhlp entry point in our CS */
   temp->_segadr.segment = req_hdr->pointer1._segadr.segment;
   temp->_segadr.offset  = req_hdr->pointer1._segadr.offset;

   /*    Free the LDT selector slot used */
   pointer.phys = (physaddr)temp;
   free_virt(pointer._segadr.segment);

   /* Set the return state to fail, if it passes, we will fix before exit */
   req_hdr_out = (struct init_hdr_out *)req_hdr;
   req_hdr_out->code_end = 0;
   req_hdr_out->data_end = 0;
   req_hdr_out->data1 = 0;

   /* Make the message file name */
   make_msg_fname(req_hdr->pointer2);

   /* Print the loading commercial */
   prt_msg(1);

   /* Do your initialisation  */
   open_init();
   rc = scsi_init();
   if (rc) return(rc);

   /* Set the Code and Data segment sizes in the request header */
   pointer.phys = (physaddr)&last_c;
   req_hdr_out->code_end = pointer._segadr.offset;
   pointer.fptr = (void far *)&last_d;
   req_hdr_out->data_end = pointer._segadr.offset;

   return(DONE);

}
