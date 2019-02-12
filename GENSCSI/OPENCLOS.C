/****************************************************************************/
/* Routines for processing the Open and Close commands from OS/2            */
/****************************************************************************/
/*                                                                          */
/* We keep a record of all the device handles given by OS2SCSI and what     */
/* file handle (from OPEN) is the owner of each device handle.  The reason  */
/* for this is if an application ends without doing a Deallocate after it   */
/* has done an Allocate, we need to do the Deallocate ourself.  This way,   */
/* when another application wants to use the device, it will be available.  */
/*                                                                          */
/* There are a couple of things to watch out for, here.  The first is that  */
/* a single file handle is allowed to allocate up to 8 devices.  This is    */
/* an arbitrary number set by the MAX_DHANDS define.  The second is that    */
/* can get multiple opens by a single handle.  If a program spawns a child  */
/* process with inheritance and it has an outstanding OPEN to your DD, you  */
/* will see a second OPEN with the same file handle.  It is up to you to    */
/* keep track of the OPENS and CLOSES and only release the reserved         */
/* resources when the open count on a given handle reaches zero.  That is   */
/* why there is an open_count field in the handle information array.        */
/*                                                                          */
/* Finally, we have an arbitrary limit of 50 different file handles.  If    */
/* more than 50 different processes do an open to us, we will fail the 51st */
/* because we have no place to store the information.  This value is set    */
/* by the MAX_OPENS define.                                                 */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* open_init() initializes the array of file handles/Device handles to      */
/* empty.  It is called at INIT time.                                       */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* open() processes the OPEN command from OS/2.  It searches the array for  */
/* a matching file handle.  If it finds one, it increments the open_count   */
/* field for that array element and exits, returning a successful return    */
/* code to the caller.  If it can't find a matching file handle (this is    */
/* the first OPEN to our DD by this process), it searches for an empty slot */
/* in the array.  If it finds an empty slot, it stores the handle in that   */
/* slot and sets the open_count field to 1 and returns success to the       */
/* caller.                                                                  */
/*                                                                          */
/* If it cannot find a matching file handle or an open slot, it returns     */
/* FAIL to the caller, indicating that we don't have any more room to hold  */
/* the handle information.                                                  */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* close() processes the CLOSE command from OS/2.  It searches the open     */
/* handle array looking for a matching file handle.  When it finds one, it  */
/* decrements the open count.  If the count hasn't reached zero, it then    */
/* returns success to the caller.  If the count has reached zero, it calls  */
/* OS2SCSI to deallocate any devices that were still allocated.  These are  */
/* indicated in the array of device handles by a non-zero value.            */
/*                                                                          */
/* If it can't find a matching file handle, something is obviously wrong -  */
/* how can we get a CLOSE when we didn't get a matching OPEN.  It really    */
/* means that our records have been hosed.  In this case, we return FAIL    */
/* to the caller.                                                           */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* get_avail_devh() returns a count of the empty device handle slots for a  */
/* given file handle.  It is used when an application is doing an Allocate  */
/* to make sure we have room to store the device handle.                    */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* set_new_devh() stores a device handle in the array record associated     */
/* with a given file handle.  If the device handle array is full, it        */
/* returns an error to the caller.                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* delete_devh() removes a device handle from the array record for a given  */
/* file handle.  If it doesn't find the record or doesn't find the device   */
/* handle in the record, it retuns an error.  If it does find the device    */
/* handle in the record, it sets the entry to zero and decrements the       */
/* device handle counter for this file handle.                              */
/*                                                                          */
/****************************************************************************/
#include "genscsi.h"

#define MAX_OPENS 50
#define MAX_DHANDS 8

/* Define the file handle record */
/*   there is a slot for the handle itself, now many opens we've seen for */
/*   this handle, how many device handles we've been given and the handles */
/*   thmselves.  */
typedef struct {
   word file_handle;
   word open_count;
   word alloc_count;
   word dhandle[MAX_DHANDS];
   } open_info_type;

/* Create an array of file handle records */
open_info_type open_info[MAX_OPENS];

void far open_init(void)
{
   word i;
   word j;

   for (i=0; i<MAX_OPENS; i++) {
      open_info[i].file_handle = 0;
      open_info[i].open_count  = 0;
      open_info[i].alloc_count  = 0;
      for (j=0; j<MAX_DHANDS; j++) { open_info[i].dhandle[j]  = 0; }
      }
}

int far open(reqhdr_type *in_req)
{
   open_hdr     *req;
   word          fhandle;
   word          found;
   word i;
   word j;

   /* Cast the request header */
   req = (open_hdr *)in_req;

   /* Mark it good for now */
   req->rh_stat = DONE;

   /* Hunt for the file handle in the open array */
   fhandle = req->file_number;
   found = 0;
   for (i=0; i<MAX_OPENS; i++) {
      if (open_info[i].file_handle == fhandle) {
         found = 1;
         break;
         }
      }

   /* If we found it, just increment the count */
   if (found) { open_info[i].open_count++; }

   /* Else we didn't, so hunt for an open slot to put this one */
   else {
      found = 0;
      for (i=0; i<MAX_OPENS; i++) {
         if (open_info[i].file_handle == 0) {
            found = 1;
            break;
            }
         }
      }


   /* If we found an open slot, put this one here and set the count to 1 */
   if (found) {
      open_info[i].open_count = 1;
      open_info[i].alloc_count = 1;
      open_info[i].file_handle = fhandle;
      for (j=0; j<MAX_DHANDS; j++) { open_info[i].dhandle[j]  = 0; }
      }

   /* Else there are no more open slots, so return an error */
   else {
      req->rh_stat = DONE+ERROR+255;
      }

   /* Devdone the packet  */
   return(req->rh_stat);

}

int far close(reqhdr_type *in_req)
{
   open_hdr     *req;
   word          fhandle;
   word          dhandle;
   word          found;
   word i;
   word j;

   /* Cast the request header */
   req = (open_hdr *)in_req;

   /* Mark it good for now */
   req->rh_stat = DONE;

   /* Hunt for the file handle in the open array */
   fhandle = req->file_number;
   found = 0;
   for (i=0; i<MAX_OPENS; i++) {
      if (open_info[i].file_handle == fhandle) {
         found = 1;
         break;
         }
      }

   /* If we found it, just decrement the count */
   if (found) {
      open_info[i].open_count--;

      /* if the count went to 0, empty the slot, freeing any device handles */
      /* still allocated                                                    */
      if (open_info[i].open_count) {}
      else {
         open_info[i].alloc_count = 0;
         open_info[i].file_handle = 0;
         for (j=0; j<MAX_DHANDS; j++) {
            dhandle = open_info[i].dhandle[j];
            if (dhandle) {
               free_dhand(dhandle);
               open_info[i].dhandle[j]  = 0;
               }
            }
         }
      }

   /* Else we didn't find it, something broke */
   else {
      req->rh_stat = DONE+ERROR+255;
      }

   /* Devdone the packet  */
   return(req->rh_stat);

}

/* Get the number of available device handles */
word get_avail_devh(word fhandle)
{
   int i;
   int found;

   /* Search for the matching record in the open array */
   found = 0;
   for (i=0; i<MAX_OPENS; i++) {
      if (open_info[i].file_handle == fhandle) {
         found = 1;
         break;
         }
      }
   if (found) { return(MAX_DHANDS - open_info[i].alloc_count); }
   else       { return(MAX_DHANDS); }
}

/* Set a new device handle */
word set_new_devh(word fhandle, word dhandle)
{
   int i;
   int j;
   int found;

   /* Search for the matching record in the open array */
   found = 0;
   for (i=0; i<MAX_OPENS; i++) {
      if (open_info[i].file_handle == fhandle) {
         found = 1;
         break;
         }
      }

   /* If we found a matching file handle */
   if (found) {

      /* Check to make sure that the device handle array isn't full */
      if (open_info[i].alloc_count == MAX_DHANDS) { return(-1); }

      /* Look for an empty element in the device handle array */
      found = 0;
      for (j=0; i<MAX_DHANDS; j++) {
         if (open_info[i].dhandle[j] == 0) {
            found = 1;
            break;
            }
         }

      /* If we found one, store the device handle there, and git */
      if (found) {
         open_info[i].dhandle[j] = dhandle;
         open_info[i].alloc_count++;
         return(0);
         }

      /* Else we didn't find one, so return an error */
      else       { return(-1); }

      }

   /* If we didn't find a matching file handle, return an error */
   else {
      return(-1);
      }
}

/* Delete a device handle */
word delete_devh(word fhandle, word dhandle)
{
   int i;
   int j;
   int found;

   /* Search for the matching record in the open array */
   found = 0;
   for (i=0; i<MAX_OPENS; i++) {
      if (open_info[i].file_handle == fhandle) {
         found = 1;
         break;
         }
      }

   /* If we found a matching file handle */
   if (found) {

      /* Check to make sure that the device handle array isn't empty */
      if (open_info[i].alloc_count == 0) { return(-1); }

      /* Look for an matching element in the device handle array */
      found = 0;
      for (j=0; i<MAX_DHANDS; j++) {
         if (open_info[i].dhandle[j] == dhandle) {
            found = 1;
            break;
            }
         }

      /* If we found one, clear the device handle there, and git */
      if (found) {
         open_info[i].dhandle[j] = 0;
         open_info[i].alloc_count--;
         return(0);
         }

      /* Else we didn't find one, so return an error */
      else       { return(-1); }

      }

   /* If we didn't find a matching file handle, return an error */
   else {
      return(-1);
      }
}
