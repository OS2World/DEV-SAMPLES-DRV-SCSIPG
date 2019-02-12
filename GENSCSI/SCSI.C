/****************************************************************************/
/* Routines for interfacing to the SCSI DD                                  */
/* The functions in this file are the heart of GENSCSI.SYS.  These are the  */
/* routines that interface to OS2SCSI.DMD.                                  */
/****************************************************************************/
/*                                                                          */
/* A general note - neither of the functions called by an application -     */
/* call_scsi() and transfer_scb() verify access to the buffers.  This can   */
/* cause kernel crashes of the application passes a bogus pointer.  It is   */
/* left as an exercise for the student to put buffer verification in.       */
/*                                                                          */
/****************************************************************************/
/* scsi_init()                                                              */
/*                                                                          */
/* This function does all the things that are needed at INIT time.  They    */
/* are:                                                                     */
/*                                                                          */
/* 1.  Allocate 4 GDT selectors for use when calling OS2SCSI.DMD            */
/*                                                                          */
/* 2.  Get the IDC addresses of OS2SCSI.DMD.  It does this via the AttachDD */
/*     Device Helper function.                                              */
/*                                                                          */
/* 3.  Get a file handle for OS2SCSI.  It does this via DosOpen.  This      */
/*     gives us a valid handle to put in the request packets we send to     */
/*     OS2SCSI.                                                             */
/*                                                                          */
/* If any of these steps fail, we return with a General Failure indication. */
/* Otherwise, we return success.                                            */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* call_scsi()                                                              */
/*                                                                          */
/* This function handles all the calls to OS2SCSI except the Transfer SCB   */
/* commands.  Since these require a bit more processing, they are handled   */
/* separately.                                                              */
/*                                                                          */
/* The basic idea here is to build a new request packet with GDT based      */
/* pointers where the request packet from the application has LDT based     */
/* pointers.  The reason for the LDT to GDT switch is that OS2SCSI may have */
/* to access the buffer pointed to at INIT time and any LDT based pointer   */
/* will be invalid.  After the packet is built, it is given to OS2SCSI.     */
/* When OS2SCSI returns, the results are stored in the application's        */
/* request packet as if OS2SCSI put them there and we exit.                 */
/*                                                                          */
/* The actual steps taken are:                                              */
/* 1. Make sure it is a valid request.  If the category isn't 0x80, fail it */
/*    right here; there is no sense passing it to OS2SCSI.                  */
/*                                                                          */
/* 2. If it is Transfer SCB, call the Transfer SCB handler.                 */
/*                                                                          */
/* 3. If it is Allocate Device, make sure we have room in the open array to */
/*    store the resulting handle.  If we don't, return with an appropriate  */
/*    error code.                                                           */
/*                                                                          */
/* 4. Allocate a request packet, returning an error if it fails             */
/*                                                                          */
/* 5. Copy all the parameters except the parm buffer pointer, the data      */
/*    buffer pointer and the system file handle from the application's      */
/*    request packet to the newly created request packet.                   */
/*                                                                          */
/* 6. Create an GDT based pointer to the buffer pointed to by the parm      */
/*    buffer pointer in the application's request packet.  This is done     */
/*    by first getting the physical address of the buffer and then doing    */
/*    a PhysToGDT devhelper call with the passed length as the length of    */
/*    the new selector.  This pointer is stored in the Parm Buffer Pointer  */
/*    of the new request packet.                                            */
/*                                                                          */
/* 7. Do the same thing with the Data buffer pointer, using a different GDT */
/*    selector.                                                             */
/*                                                                          */
/* 8. Store the file handle we got from the DOsOpen at INIT time in the     */
/*    new request packet.                                                   */
/*                                                                          */
/* 9. Lock the parm and data buffers.                                       */
/*                                                                          */
/* 10. Call OS2SCSI with the new request packet.                            */
/*                                                                          */
/* 10. When we return from SO2SCSI, copy the following 3 values from the    */
/*    new request packet to the application's request packet:               */
/*    1. The request packet status.                                         */
/*    2. The parm buffer length                                             */
/*    3. The data buffer length                                             */
/*                                                                          */
/* 11. Free the allocated request packet                                    */
/*                                                                          */
/* 12. If the request succeeded and it was an Allocate Device, add the new  */
/*    device handle to the open array.  If it was a Deallocate device, take */
/*    the device handle from the open array.                                */
/*                                                                          */
/* 13. Unlock the parm and data buffers.                                    */
/*                                                                          */
/* 14. Return back to OS/2.                                                 */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* transfer_scb()                                                           */
/*                                                                          */
/* This function is very similar to call_scsi(), except it has a bit more   */
/* processing to do on converting pointers.  First, the data buffer and     */
/* parm buffer pointers are handled just like in the regular call.  Next    */
/* comes the SCB Chain Header pointer.                                      */
/*                                                                          */
/* Inside the Transfer SCB block (pointed to by the parm buffer) are 2      */
/* pointers - a virtual pointer to the SCB Chain header and a physical      */
/* pointer to the first SCB in the chain.  (see the OEMBASE.TXT file for    */
/* more detail).  The virtual pointer must be changed from an LDT based     */
/* pointer to a GDT based pointer; the usual tricks are done.  In addition, */
/* however, we save away the original virtual pointer so we can replace it  */
/* in the Transfer SCB block when we are done.  This way, the caller can    */
/* continue to use the same block without modification.  Then, we store the */
/* physical address of the first SCB in the Transfer SCB block.             */
/*                                                                          */
/* Next comes the pointers in the SCB Header.  There is one here - the LDT  */
/* based pointer to the TSB.  The usual Lock/get physical address/save LDT  */
/* based pointer/make GDT based pointer steps are done.                     */
/*                                                                          */
/* The last to have this treatment is the System Buffer within the SCB.     */
/* For more detail about SCBs, see the IBM Microchannel SCSI Adapter Tech   */
/* Reference or the document within this package.  We need to store the     */
/* physical address here, not a virtual pointer.  This is because the IBM   */
/* SCSI adapter is a bus master and uses that address as the target for     */
/* DMA.                                                                     */
/*                                                                          */
/* Finally, we need to deal with Scatter/Gather.  Since OS/2 V2 uses a      */
/* paging system, the data buffer may consist of physical pages scattered   */
/* all over.  We need to indicate that to OS2SCSI.  To do that, we convert  */
/* the original virtual address of the system buffer in the SCB to a page   */
/* list.  This is a 2 step process as the only DevHelper function to create */
/* a page list requires a linear address as input, so we have to convert    */
/* the Virtual address to a linear address first.  Next, we see if the      */
/* buffer has more than 1 block of pages.  If it doesn't, we can leave the  */
/* GDT based pointer in place.  If not, we must replace it with the page    */
/* list and indicate in the SCB Enable word that the system buffer address  */
/* is a page list, not a GDT based pointer.                                 */
/*                                                                          */
/* From here on out, it's the same as the normal call SCSI, except for the  */
/* replacement of the original virtual addresses and more unlocks.          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* free_dhand()                                                             */
/*                                                                          */
/* This function calls OS2SCSI to deallocaet a device handle.  It is used   */
/* when we get a close on a file handle that has at least 1 device handle   */
/* still allocated.  This can happen if the application ends without doing  */
/* the deallocate or maybe it crashed.  No matter why it happened, we need  */
/* to explicitly free the devices it owns so other programs can reserve     */
/* them.                                                                    */
/*                                                                          */
/* The operation is failry simple - a Deallocate request packet is built    */
/* and sent to OS2SCSId handle to put in the request packets we send to     */
/* OS2SCSI.  After OS2SCSI returns, we free the request packet and return   */
/* to the caller.                                                           */
/*                                                                          */
/****************************************************************************/
#define INCL_DOSDEVICES
#define INCL_DOSMISC
#include <os2.h>
#include "genscsi.h"

idc_type near scsi_idc;
void far * near idc_entry;
byte near scsi_name[] = "SCSI-02$";
HFILE near  handle;
word  near  gdt_selector[4];

word far transfer_scb(reqhdr_type *);

word far scsi_init(void)
{
  _32bits temp;
  union cpu_regs in_regs;
  union cpu_regs out_regs;
  word        oflag;
  word        omode;
  USHORT      action;
  word        rc;

  /* Get 4 GDT selectors */
  temp.fptr = (void far *)gdt_selector;
  if (get_gdt_slots(4,temp)) { return(ERROR+DONE+GEN_FAIL); }

  /* Do the Attach_DD call */
  temp.fptr = (void far *)scsi_name;
  in_regs.W.BX = temp._segadr.offset;
  temp.fptr = (void far *)&scsi_idc;
  in_regs.W.DI = temp._segadr.offset;
  in_regs.W.es_valid = FALSE;
  in_regs.W.ds_valid = FALSE;
  in_regs.B.DL = devhlp_AttachDD;
  dev_help(&in_regs,&out_regs);
  if ((out_regs.W.flags & 0x0001) != 0) { return(ERROR+DONE+GEN_FAIL); }

  /* Save the entry point values */
  temp._segadr.segment = scsi_idc.prot_CS;
  temp._segadr.offset  = scsi_idc.prot_IP;
  idc_entry = temp.fptr;

  /* Get a handle to the SCSI DD */
  omode = 0x0012;
  oflag = 0x0001;
  rc = DosOpen(scsi_name,                   /* File name */
               &handle,                     /* Handle    */
               &action,                     /* Action taken */
               0L,                          /* File len */
               0,                           /* Attribute - normal file */
               oflag,                       /* Open flag */
               omode,                       /* Open mode */
               0L);

  /* If it failed, return to the caller */
  if (rc) { return(ERROR+DONE+GEN_FAIL); }

  return(0);

}

word far call_scsi(reqhdr_type *in_req)
{
   ioctl_hdr2    *newreq;
   ioctl_hdr2    *req;
   _32bits        temp;
   unsigned long  p_lock;
   unsigned long  d_lock;
   _32bits        sense_phys;
   _32bits        xfer_phys;
   word          *dhandle;

   /* Cast the request header */
   req = (ioctl_hdr2 *)in_req;

   /* Make sure it is one of ours */
   if (req->funct_cat != 0x80) {
       in_req->rh_stat = ERROR+INV_CMD;

       /* Devdone the packet  */
       return(ERROR+INV_CMD);
       }

   /* Handle Transfer SCB separately */
   if ((req->funct_cat == 0x80) && (req->funct_cod == 0x52)) {
       return(transfer_scb(in_req));
       }

   /* If it is an ALLOCATE request */
   if ((req->funct_cat == 0x80) && (req->funct_cod == 0x55)) {

      /* Check to see that we haven't maxed out the allocs for this handle */
      if (0 == get_avail_devh(req->file_number)) {

         /* If we did max them out, return an error */
         req->request_hdr.rh_stat   = ERROR+DONE+0x90;
         return(req->request_hdr.rh_stat);
         }
      }

   /* Make a request packet */
   temp = alloc_req();

   /* if it worked, cast the pointer */
   if (temp.phys)  { newreq = (ioctl_hdr2 *)temp.fptr; }

   /* if it failed, exit with an error */
   else { return(ERROR+DONE+GEN_FAIL); }

   /* Copy the common parms from the first to the new one */
   newreq->request_hdr.rh_len    = req->request_hdr.rh_len;
   newreq->request_hdr.rh_unit   = req->request_hdr.rh_unit;
   newreq->request_hdr.rh_cmd    = req->request_hdr.rh_cmd;
   newreq->request_hdr.rh_stat   = req->request_hdr.rh_stat;
   newreq->request_hdr.rh_resrvd = req->request_hdr.rh_resrvd;
   newreq->request_hdr.rh_next   = req->request_hdr.rh_next;
   newreq->funct_cat             = req->funct_cat;
   newreq->funct_cod             = req->funct_cod;
   newreq->pbuf_len              = req->pbuf_len;
   newreq->dbuf_len              = req->dbuf_len;

   /* Convert the Parm buffer pointer to a GDT based address and store in */
   /* the new request packet, after locking it */
   p_lock = lock(newreq->pbuffer._segadr.segment);
   xfer_phys       = get_phys_addr(req->pbuffer);
   newreq->pbuffer = phys_to_gdt(xfer_phys,req->pbuf_len,gdt_selector[2]);

   /* Convert the Data buffer pointer to a GDT based address and store in */
   /* the new request packet, after locking it */
   d_lock = lock(newreq->dbuffer._segadr.segment);
   sense_phys      = get_phys_addr(req->dbuffer);
   newreq->dbuffer = phys_to_gdt(sense_phys,req->dbuf_len,gdt_selector[3]);

   /* Set the File handle to the SCSI DD's handle */
   newreq->file_number           = handle;

   /* Call the SCSI DD */
   call_idc(idc_entry,(void *)newreq, scsi_idc.prot_DS);

   /* Copy the request packet status to the original status */
   req->request_hdr.rh_stat   = newreq->request_hdr.rh_stat;
   req->pbuf_len              = newreq->pbuf_len;
   req->dbuf_len              = newreq->dbuf_len;

   /* Return the request packet to the OS */
   free_req(temp);

   /* If it worked */
   if (req->request_hdr.rh_stat == DONE) {

      /* If it was an ALLOCATE DEVICE */
      if ((req->funct_cat == 0x80) && (req->funct_cod == 0x55)) {

         /* Add the device handle to the OPEN record for this handle */
         dhandle = (word *)req->dbuffer.fptr;
         set_new_devh(req->file_number,*dhandle);
         }

      /* If it was a FREE DEVICE */
      if ((req->funct_cat == 0x80) && (req->funct_cod == 0x54)) {

         /* Remove the device handle from the OPEN record for this handle */
         dhandle = (word *)req->pbuffer.fptr;
         delete_devh(req->file_number,*dhandle);
         }
      }

   /* Unlock the buffers */
   unlock(p_lock);
   unlock(d_lock);

   /* Devdone the packet  */
   return(req->request_hdr.rh_stat);

}

word far transfer_scb(reqhdr_type *in_req)
{
   ioctl_hdr2    *newreq;
   ioctl_hdr2    *req;
   _32bits        temp;
   unsigned long  p_lock;
   unsigned long  d_lock;
   unsigned long  scb_lock;
   _32bits        scb_hdr_phys;
   _32bits        sense_phys;
   _32bits        xfer_phys;
   unsigned long  sys_lock;
   unsigned long  tsb_lock;
   _32bits        sys_virt;
   _32bits        tsb_virt;
   _32bits        scb_virt;
   xfer_scb_type *xfer;
   scb_hdr_type  *hdr;
   scb_type      *scb;
   pagelist_type  pagelist[16];
   _32bits        sys_lin;
   _32bits        page_lin;
   int            i,end_found,last_page;
   int            command;

   /* Cast the request header */
   req = (ioctl_hdr2 *)in_req;

   /* Make a request packet, exiting if failure */
   temp = alloc_req();
   if (temp.phys) newreq = (ioctl_hdr2 *)temp.fptr;
   else { return(ERROR+DONE+GEN_FAIL); }

   /* Copy the parms from the first to the new one */
   newreq->request_hdr.rh_len    = req->request_hdr.rh_len;
   newreq->request_hdr.rh_unit   = req->request_hdr.rh_unit;
   newreq->request_hdr.rh_cmd    = req->request_hdr.rh_cmd;
   newreq->request_hdr.rh_stat   = req->request_hdr.rh_stat;
   newreq->request_hdr.rh_resrvd = req->request_hdr.rh_resrvd;
   newreq->request_hdr.rh_next   = req->request_hdr.rh_next;
   newreq->funct_cat             = req->funct_cat;
   newreq->funct_cod             = req->funct_cod;
   newreq->pbuf_len              = req->pbuf_len;
   newreq->dbuf_len              = req->dbuf_len;
   newreq->file_number           = handle;

   /* Deal first with the data buffer pointer.  It points to the Sense data */
   /* First, lock it, then get the physical address, then make a GDT pointer */
   /* to it and store that pointer in the new request packet */
   d_lock          = lock(req->dbuffer._segadr.segment);
   sense_phys      = get_phys_addr(req->dbuffer);
   newreq->dbuffer = phys_to_gdt(sense_phys,req->dbuf_len,gdt_selector[3]);

   /* Now, do the same for the PARM buffer pointer - it is the Transfer SCB */
   p_lock          = lock(req->pbuffer._segadr.segment);
   xfer_phys       = get_phys_addr(req->pbuffer);
   newreq->pbuffer = phys_to_gdt(xfer_phys,req->pbuf_len,gdt_selector[2]);

   /* Make some pointers for speed */
   xfer = (xfer_scb_type *)req->pbuffer.fptr;
   hdr =  (scb_hdr_type *)xfer->scb_header.fptr;
   scb = (scb_type *)&hdr->scb;

   /* SCB header pointer in the transfer SCB - do the same trick as with  */
   /* the parm buffer and data buffer - lock, get physical address and    */
   /* replace the the pointer in the transfer SCB with the GDT based      */
   /* equivalent.  The only difference this time is that we save the LDT  */
   /* based pointer for replacement after we are done.                    */
   scb_lock         = lock(xfer->scb_header._segadr.segment);
   scb_virt         = xfer->scb_header;
   scb_hdr_phys     = get_phys_addr(xfer->scb_header);
   xfer->scb_header = phys_to_gdt(scb_hdr_phys,
                                  sizeof(long_scb_hdr_type),
                                  gdt_selector[1]);

   /* Now, store a physical pointer to the SCB in the header */
   temp.fptr                 = (void far *)&hdr->scb;
   xfer->scb_phys            = get_phys_addr(temp);

   /* Now do it for the TSB                                              */
   tsb_lock         = lock(hdr->scb.tsb_adr._segadr.segment);
   tsb_virt         = hdr->scb.tsb_adr;
   hdr->scb.tsb_adr = get_phys_addr(tsb_virt);
   hdr->tsb_adr     = phys_to_gdt(hdr->scb.tsb_adr,100,gdt_selector[0]);

   /* System buffer - don't need virtual, but physcal address here */
   sys_lock                = lock(hdr->scb.system_buf_adr._segadr.segment);
   sys_virt                = hdr->scb.system_buf_adr;
   hdr->scb.system_buf_adr = get_phys_addr(sys_virt);

   /* Finally, deal with the scatter/gather problem.                         */
   /* If we are allowed to (the command is right), and we might be able to   */
   /* (there is some data to transfer) and we need to (the buffer spans      */
   /* more than 1 page block), set the scatter/gather list indicator in the  */
   /* SCB and store a physical pointer to the page list in the system byffer */
   /* address.  This meand converting the virtual address first to a linear  */
   /* address and then to a pagelist.  If the pagelist is more than 1 page   */
   /* long, we then tell OS2SCSI that the system buffer is the physical      */
   /* address of the page list instead of the physical addr of the buffer.   */
   command = hdr->scb.command & 0x3f;
   if (scb->system_buf_cnt.phys != 0L) {
      switch (command) {
         case  1 :
         case  2 :
         case  4 :
         case 31 : {
            sys_lin = virt_to_lin(sys_virt);
            temp.fptr = (void far *)pagelist;
            page_lin = virt_to_lin(temp);
            for (i=0; i<16; i++) { pagelist[i].page_size = 0; }
            lin_to_pages(sys_lin, page_lin,scb->system_buf_cnt);
            end_found = 0;
            for (i=0; i<16; i++) {
               if (pagelist[i].page_size == 0) {
                  end_found = 1;
                  break;
                  }
               }
            if (end_found) last_page = i;
            else           last_page = 16;

            if (i != 1) {
               hdr->scb.system_buf_adr = get_phys_addr(temp);
               hdr->scb.system_buf_cnt.phys = last_page * sizeof(pagelist_type);
               hdr->scb.enable |= 0x1000;
               }
            break;
            }
         }
      }

   /* Call the SCSI DD */
   call_idc(idc_entry,(void *)newreq, scsi_idc.prot_DS);

   /* Copy the request packet status to the original status */
   req->request_hdr.rh_stat   = newreq->request_hdr.rh_stat;
   req->pbuf_len              = newreq->pbuf_len;
   req->dbuf_len              = newreq->dbuf_len;

   /* Return the request packet to the OS */
   free_req(temp);

   /* Put the virtual addresses back */
   hdr->scb.system_buf_adr = sys_virt;
   hdr->scb.tsb_adr        = tsb_virt;
   xfer->scb_header        = scb_virt;

   /* Unlock the buffers */
   unlock(p_lock);
   unlock(d_lock);
   unlock(scb_lock);
   unlock(sys_lock);
   unlock(tsb_lock);

   /* Devdone the packet  */
   return(newreq->request_hdr.rh_stat);

}

word far free_dhand(word dhandle)
{
   ioctl_hdr2    *newreq;
   _32bits        temp;
   _32bits        temp1;
   _32bits        pbuf_phys;
   word           pbuf;

   /* Make a request packet */
   temp = alloc_req();
   if (temp.phys) newreq = (ioctl_hdr2 *)temp.fptr;

   /* Get some physical addresses */
   temp1.fptr =(void far *)&pbuf;
   pbuf_phys = get_phys_addr(temp1);

   /* Copy the parms from the first to the new one */
   newreq->request_hdr.rh_len    = 0x19;
   newreq->request_hdr.rh_unit   = 0;
   newreq->request_hdr.rh_cmd    = 0x10;
   newreq->request_hdr.rh_stat   = 0;
   newreq->request_hdr.rh_resrvd = 0L;
   newreq->request_hdr.rh_next   = 0L;
   newreq->funct_cat             = 0x80;
   newreq->funct_cod             = 0x54;
   newreq->pbuffer               = phys_to_gdt(pbuf_phys,2,gdt_selector[2]);
   newreq->dbuffer.phys          = 0L;
   newreq->pbuf_len              = 2;
   newreq->dbuf_len              = 0;

   /* Change the File handle to the SCSI DD's handle */
   newreq->file_number           = handle;

   /* Store the device handle in the pbuffer */
   pbuf = dhandle;

   /* Call the SCSI DD */
   call_idc(idc_entry,(void *)newreq, scsi_idc.prot_DS);

   /* Return the request packet to the OS */
   free_req(temp);

   /* Devdone the packet  */
   return(0);

}
