/*****************************************************************************/
/*  prt_msg(msg,val)                                                         */
/*          - Print a selected message, possibly with a parameter value      */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* This function works with the DosGetMessage and DosPutMessage OS calls.    */
/* That is why it only works at INIT time. (INIT time is the only time a DD  */
/* can do any architected screen IO anyway).  There also seems to be a bug   */
/* in the message handler when the caller wants to wait for an ENTER, OS/2   */
/* just keeps on going, it doesn't wait.  That is why the DosRead is in      */
/* case 7.                                                                   */
/*                                                                           */
/* The message file name is constructed by make_msg_fname which is called by */
/* INIT.  It uses the DD name and changes the extension to .MSG.             */
/*                                                                           */
/*   Message   Use of parm           Message                                 */
/*     1        Not Used        Loading commercial                           */
/*     2        Not Used        Press Enter to continue                      */
/*                                                                           */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INCL_DOSMISC
#include <os2.h>
#include "genscsi.h"
#include "version.h"
static char near valstr[6];
       char near msg_file[255];


void far make_msg_fname( char *line)
{
char *token;
char *found;

  /* get the DD file name */
  token = strtok(line," ");

  /* Make the message file name from the DD file name */
  strcpy(msg_file,token);             /* Copy the DD fname to msg fname      */
  found = strchr(msg_file,(int)'.');  /* Find the . in the fname             */
  if (found == NULL) {                /* If there is no .                    */
     found = strchr(msg_file,0);      /*    Find the end of the name         */
     found[0] = '.';                  /*    And make it a dot                */
     }
  found[1] = 'M';                     /* Make the chars after the dot MSG    */
  found[2] = 'S';
  found[3] = 'G';
  found[4] = '\0';                    /* and put a terminator                */

}

unsigned far prt_msg(msg)
unsigned msg;
{
   char message[80];
   char num1[10];
   char num2[10];
   char *pointers[2];
   USHORT mlen;
   unsigned i;

   switch (msg) {

   case 1 : /* The loading commercial */

      /* Three lines of intro stuff   */
      for (i = 1; i <= 2; i++) {
         DosGetMessage(NULL, 0, message, 80, i, msg_file, &mlen);
         DosPutMessage(1,mlen,message);
         }

      /* Put the major and minor versions into the array for the  */
      /* message handler to use in the next line                  */
      itoa(VER_MAJ,num1,10);
      pointers[0] = (char *)num1;
      i = 0;
      if (VER_MIN < 10) {
         num2[i++] = '0';
         }
      itoa(VER_MIN,(num2+i),10);
      pointers[1] = (char *)num2;
      DosGetMessage(pointers,2,message,80,3,msg_file,&mlen);
      DosPutMessage(1,mlen,message);

      /* And the end stuff         */
      DosGetMessage(NULL,0,message,80,4,msg_file,&mlen);
      DosPutMessage(1,mlen,message);

      return(0);
      break;

   case 2 : /* Press Enter */
      DosGetMessage(NULL,0,message,80,5,msg_file,&mlen);
      DosPutMessage(1,mlen,message);
      DosRead(1,message,80,&mlen);
      return(0);
      break;

      }

}
