/***************************************************************************/
/* strategy_c(req_hdr)                                                     */
/*  Strategy function. Called by assembler portion of strategy entry point */
/*  of DD, this is functionally the strategy entry point.  It looks at the */
/*  command portion of the request packet an calls the proper function.    */
/*                                                                         */
/*  There is only 4 command that really do anything - INIT, OPEN, CLOSE,   */
/*  and IOCtl.  The others just return an error signifying that we don't   */
/*  accept that command.                                                   */
/*                                                                         */
/*  All commands return a value that will be ORed with the DONE bit and    */
/*  set into the request packet's STATUS field via DEV_DONE, except INIT   */
/*  which sets the status field directly as the DevDone Device Helper is   */
/*  not valid at INIT time.                                                */
/*                                                                         */
/***************************************************************************/
#include "genscsi.h"

int near strategy_c(req_hdr)
struct reqhdr *req_hdr;
{
  word rc;

  /* Check to see if the command is outside the range of valid commands */
  if (req_hdr->rh_cmd > MAXCMD) { rc = ERROR+INV_CMD; }
  else {

  /* Different commands call for different functions */
     switch (req_hdr->rh_cmd) {
        case  0 :                          /* Init */
                  rc = init_mod((struct init_hdr_in *)req_hdr);
                  break;

        case 13 :
                  rc = open(req_hdr);      /* Open */
                  break;

        case 14 :
                  rc = close(req_hdr);     /* Close */
                  break;

        case 16 :
                  rc = ioctl(req_hdr);     /* IOCtl */
                  break;

        default :                          /* We don't do no mo' commands */
                  rc = ERROR+INV_CMD;
                  break;
     } /* switch */
   } /* else */

  /* Unblock the waiting app */
  if (req_hdr->rh_cmd) { dev_done(req_hdr,rc); }
  else { req_hdr->rh_stat = rc; }

  return(SUCCESS);

}
