/*********************************
**  Include files               **
*********************************/
#pragma pack(1)         /* Pack the structures */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES
#include <OS2.h>

/*****************************************************************************/
/*                                                                           */
/* TSB                                                                       */
/*                                                                           */
/* This is the Termination Status Block - it tells how an SCB completed      */
/* and what went wrong when it dodn't.  I've not had a lot of luck           */
/* interpreting these things.  They are documented in the SCSI adapter       */
/* tech reference.                                                           */
/*                                                                           */
/*****************************************************************************/
typedef struct tsb_def {
   USHORT  SCB_status;
   USHORT  retry_counts;
   ULONG   residual_buffer_byte_count;
   ULONG   residual_buffer_address;
   USHORT  additional_status_length;
   unsigned int   bit0                    : 1;
   unsigned int   SCSI_status_code        : 4;
   unsigned int   bit5                    : 1;
   unsigned int   bit6                    : 1;
   unsigned int   bit7                    : 1;
   unsigned int   command_status          : 8;
   unsigned int   device_error_code       : 8;
   unsigned int   command_error_code      : 8;
   USHORT  error_modifier;
   USHORT  cache_info;
   ULONG   last_SCB_address_processed;
   } tsb_type;

/*****************************************************************************/
/*                                                                           */
/* Inquiry block                                                             */
/*                                                                           */
/* This is the structure returned by a device in response to an Inquiry      */
/* SCSI command.  It gives some interesting information like device type     */
/* (cdrom, tape, etc), is the media removeable, what standards the device    */
/* conforms to and generally, the device's manufacturer and model numbers.   */
/*                                                                           */
/*****************************************************************************/
typedef struct inq_def {
   UCHAR   device_type;
   UCHAR   removable;
   UCHAR   standards;
   UCHAR   reserved;
   UCHAR   extra_length;
   UCHAR   res[3];
   CHAR    manufacturer[2];
   } inq_type;


/*****************************************************************************/
/*                                                                           */
/* SCBs                                                                      */
/*                                                                           */
/* These are the definitions of the SCSI Command Block used by the IBM SCSI  */
/* adapters.  There are 2 blocks defined - the standard SCB and the 'long'   */
/* SCB that has a SCSI command imbedded in it.  These are documented in the  */
/* IBM SCSI adapter technical references.                                    */
/*                                                                           */
/*****************************************************************************/
typedef struct scb_def {
   USHORT   command;                /* Command to the SCSI card              */
   USHORT   enable;                 /* Control bits to modify the command    */
   ULONG    block_number;           /* Starting block in SCSI device         */
   UCHAR * _Seg16 system_buf_adr;   /* Buffer address to transfer to/from    */
   ULONG    system_buf_cnt;         /* How big is the system buffer in bytes */
   tsb_type * _Seg16 tsb_adr;          /* Where to store the TSB                */
   UCHAR * _Seg16 chain_adr;        /* Where the next SCB is                 */
   USHORT   block_count;            /* How many blocks to transfer           */
   USHORT   block_length;           /* How big is a block in bytes           */
   } scb_type;

typedef struct long_scb_def {
   USHORT   command;                /* Command to the SCSI card              */
   USHORT   enable;                 /* Control bits to modify the command    */
   UCHAR    cmd_len;                /* How long is the SCSI command in bytes */
   UCHAR    reserved1;              /* Reserved space                        */
   USHORT   reserved2;              /* Reserved space                        */
   UCHAR * _Seg16 system_buf_adr;   /* Buffer address to transfer to/from    */
   ULONG    system_buf_cnt;         /* How big is the system buffer in bytes */
   tsb_type * _Seg16 tsb_adr;          /* Where to store the TSB                */
   UCHAR * _Seg16 chain_adr;        /* Pointer to next SCB                   */
   UCHAR    scsi_cmd[12];           /* The imbedded SCSI command (6-12 bytes)*/
   } long_scb_type;


/*****************************************************************************/
/*                                                                           */
/* SCB Headers                                                               */
/*                                                                           */
/* These are the definitions of the SCB Headers as described in the OEMBASE  */
/* document.  There are 2 blocks defined - one for the standard SCB and one  */
/* for the 'long' scb.                                                       */
/*                                                                           */
/*****************************************************************************/
typedef struct scb_hdr_def {
   USHORT    reserved1;                 /* Reserved space                    */
   char     * _Seg16 next_scb_hdr;      /* Ptr to next SCB hdr - never used  */
   USHORT    reserved2;                 /* Reserved space                    */
   USHORT    reserved3;                 /* Reserved space                    */
   tsb_type * _Seg16 tsb_adr;           /* Pointer to TSB                    */
   USHORT    reserved4;                 /* Reserved space                    */
   scb_type  scb;                       /* The 'short' SCB itself            */
   } scb_hdr_type;

typedef struct long_scb_hdr_def {
   USHORT         reserved1;            /* Reserved space                    */
   char          * _Seg16 next_scb_hdr; /* Ptr to next SCB hdr - never used  */
   USHORT         reserved2;            /* Reserved space                    */
   USHORT         reserved3;            /* Reserved space                    */
   tsb_type      * _Seg16  tsb_adr;     /* Pointer to the TSB                */
   USHORT         reserved4;            /* Reserved space                    */
   long_scb_type  scb;                  /* The 'long' SCB                    */
   } long_scb_hdr_type;


/*****************************************************************************/
/*                                                                           */
/* Transfer SCB Parm buffer definition                                       */
/*                                                                           */
/* This block is passed to GENSCSI and then on to OS2SCSI via the Transfer   */
/* SCB command.  It is documented in the OEMBASE document                    */
/*                                                                           */
/*****************************************************************************/
typedef struct xfer_scb_def {
   USHORT  dev_handle;          /* The device handle returned from Allocate  */
   USHORT  sense_size;          /* How big is the sense buffer in bytes      */
   ULONG   scb_phys;            /* Physical addr of SCB, GENSCSI will calc   */
   scb_hdr_type * _Seg16 scb_header;   /* Virtual pointer to the SCB header         */
   UCHAR   flags;               /* Is this a 'long' SCB or a 'short' SCB     */
   } xfer_scb_type;

typedef struct xfer_lscb_def {
   USHORT  dev_handle;          /* The device handle returned from Allocate  */
   USHORT  sense_size;          /* How big is the sense buffer in bytes      */
   ULONG   scb_phys;            /* Physical addr of SCB, GENSCSI will calc   */
   long_scb_hdr_type * _Seg16 scb_header;   /* Virtual pointer to the SCB header         */
   UCHAR   flags;               /* Is this a 'long' SCB or a 'short' SCB     */
   } xfer_lscb_type;

/*****************************************************************************/
/*                                                                           */
/* Timeout block                                                             */
/*                                                                           */
/* This is the block sent and returned with the Set/Read device timeout      */
/* commands.  I haven't found these commmands to be very reliable.           */
/* but then again, I didn't check it out very well either.  There may be a   */
/* bug here.                                                                 */
/*                                                                           */
/*****************************************************************************/
typedef struct timeout_def {
   USHORT    handle;
   ULONG     timeout;
   } timeout_type;

/*****************************************************************************/
/*                                                                           */
/* Device parms record                                                       */
/*                                                                           */
/* This is the block used in the Read Devcie Parms command.  As with the     */
/* Timeout commands, I haven't been able to make these return useful info,   */
/* but then again, I didn't check it out very well either.  There may be a   */
/* bug here.                                                                 */
/*                                                                           */
/*****************************************************************************/
typedef struct devparm_def {
   USHORT    dev_key_index;
   UCHAR     scb_arch_lvl;
   UCHAR     adapter_index;
   USHORT    device_flags;
   UCHAR     lun;
   UCHAR     pun;
   } devparm_type;

int     reset_dev(USHORT handle);
int     free_dev(USHORT handle);
int     read_dev(USHORT handle);
int     write_dev(USHORT handle);
int     dev_capacity(USHORT handle);
int     read_sense(USHORT handle);
int     timeout_dev(USHORT handle);
int     read_timeout_dev(USHORT handle);
int     allocate_dev(USHORT *handle);
int     read_dev_parms(USHORT handle);
int     gen_cmd(USHORT handle);
int     inquiry(USHORT handle);
void    syntax(void);
void    show_sense_x(void);
void    show_sense(void);
void    show_data(void);
void    save_datab(void);
void    save_data(void);
void    show_tsb(void);
void    show_tsb_x(void);
void    count_dev(void);
void    hexdisp(UCHAR *, int);
void    clear_sense(void);
ULONG   swap32(ULONG);
USHORT  swap16(USHORT);

/*********************************
**  Global variables            **
*********************************/
#define   SENSE_SIZE    32    /* How much sense data do we deal with */
UCHAR     sense[SENSE_SIZE];  /* This is the sense buffer            */
tsb_type  tsb;                /* The Termination Status Block        */
UCHAR    *buffer;             /* The data buffer                     */
int       bufsize      = 0;   /* How big is the buffer - start empty */
HFILE     dh;                 /* The File Handle from DosOpen        */
int       retval;             /* A generic Return Code variable      */
int       alloced = 0;        /* Have we allocated a device flag     */

/***************************************************************************/
/*                                                                         */
/* The main subroutine.  We get a handle from the DD and if it worked,     */
/* show all the commands.  Then we go into a forever loop that waits for   */
/* a command from stdin and executes it, and goes back to waiting.  The    */
/* way out of this loop is the 'quit' command which exits after freeing    */
/* the allocated device.                                                   */
/*                                                                         */
/***************************************************************************/
main( int argc, char *argv[], char *envp[])
{

   ULONG          acted;             /* action taken on OPEN               */
   USHORT         dhandle;           /* Device handle from Allocate        */
   int            rc;                /* Return code from an operation      */
   HFILE          keyboard;          /* File handle for Standard Input     */
   UCHAR          input_string[100]; /* Buffer for command string          */
   ULONG          Dos_rc;            /* Return code from DosRead           */
   ULONG          bytes_read;        /* Number of bytes read from stdin    */
   int            command;           /* Command number                     */

   /* Open the DD */
   retval = DosOpen (
      (char *)"$GENSCSI",               /* File path name */
      &dh,                              /* New file's handle */
      &acted,                           /* Action taken - 1=file existed, */
                                        /* 2=file was created */
      (ULONG)0,                         /* File primary allocation */
      (USHORT)0x0000,                   /* File attributes */
      (USHORT)0x0001,                   /* Open function type */
      (USHORT)0x60C2,                   /* Open mode of the file */
      (ULONG)0                          /* Reserved (must be zero) */
      );

   /* If it went OK, show the commands */
   if (retval == 0) { syntax(); }
   else {
      printf("Device open failed, retval=%d, action taken=%x\n",(UCHAR)retval,acted);
      return (-1);
      }

   /* Do forever */
   keyboard = 0;
   for (;;) {

      /* Get a command from the standard input */
      Dos_rc = DosRead(keyboard, (void *)input_string, 100, &bytes_read);

      /* Figure out what the command was */
      command = -1;
      if (0 == strnicmp(input_string,"alloc",    5))    command = 0;
      if (0 == strnicmp(input_string,"parms",    5))    command = 1;
      if (0 == strnicmp(input_string,"timeout",  7))    command = 2;
      if (0 == strnicmp(input_string,"read",     4))    command = 3;
      if (0 == strnicmp(input_string,"reset",    5))    command = 4;
      if (0 == strnicmp(input_string,"free",     4))    command = 5;
      if (0 == strnicmp(input_string,"quit",     4))    command = 6;
      if (0 == strnicmp(input_string,"exit",     4))    command = 6;
      if (0 == strnicmp(input_string,"p_sns",    5))    command = 7;
      if (0 == strnicmp(input_string,"p_tsb",    5))    command = 8;
      if (0 == strnicmp(input_string,"p_data",   6))    command = 9;
      if (0 == strnicmp(input_string,"count",    5))    command = 10;
      if (0 == strnicmp(input_string,"rtime",    5))    command = 11;
      if (0 == strnicmp(input_string,"sense",    5))    command = 12;
      if (0 == strnicmp(input_string,"general",  7))    command = 13;
      if (0 == strnicmp(input_string,"xp_tsb",   6))    command = 14;
      if (0 == strnicmp(input_string,"inq",      3))    command = 15;
      if (0 == strnicmp(input_string,"other",    5))    command = 16;
      if (0 == strnicmp(input_string,"xp_sns",   6))    command = 17;
      if (0 == strnicmp(input_string,"write",    5))    command = 18;
      if (0 == strnicmp(input_string,"capac",    5))    command = 19;
      if (0 == strnicmp(input_string,"w_dat",    5))    command = 20;
      if (0 == strnicmp(input_string,"xw_dat",   6))    command = 21;

      /* Do the command */
      switch(command) {

        /* alloc = allocate the device */
        case 0:
           retval = allocate_dev(&dhandle);
           break;

        /* parms = read device parms */
        case 1:
           clear_sense();
           retval = read_dev_parms(dhandle);
           break;

        /* timeout = set timeout */
        case 2:
           clear_sense();
           retval = timeout_dev(dhandle);
           break;

        /* read = read from device */
        case 3:
           clear_sense();
           rc = read_dev(dhandle);
           break;

        /* reset = initialize the device */
        case 4:
           clear_sense();
           reset_dev(dhandle);
           break;

        /* free = free the device */
        case 5:
           free_dev(dhandle);
           break;

        /* quit = quit */
        case 6:
           if (alloced) free_dev(dhandle);
           exit(0);
           break;

        /* print sense */
        case 7:
           show_sense();
           break;

        /* print TSB */
        case 8:
           show_tsb();
           break;

        /* print data */
        case 9:
           show_data();
           break;

        /* count = count devices */
        case 10:
           clear_sense();
           count_dev();
           break;

        /* rtime = set timeout */
        case 11:
           clear_sense();
           retval = read_timeout_dev(dhandle);
           break;

        /* sense = read sense */
        case 12:
           clear_sense();
           retval = read_sense(dhandle);
           break;

        /* general = general SCSI command*/
        case 13:
           clear_sense();
           retval = gen_cmd(dhandle);
           break;

        /* print TSB hex */
        case 14:
           show_tsb_x();
           break;

        /* inquiry = device inquiry*/
        case 15:
           clear_sense();
           retval = inquiry(dhandle);
           break;

        /* other = Send other SCSI command */
        case 16:
           clear_sense();
           retval = other(dhandle);
           break;

        /* print sense hex */
        case 17:
           show_sense_x();
           break;

        /* write = write to device */
        case 18:
           clear_sense();
           rc = write_dev(dhandle);
           break;

        /* capac = Device capacity */
        case 19:
           clear_sense();
           rc = dev_capacity(dhandle);
           break;

        /* w_dat = Write data to file */
        case 20:
           save_data();
           break;

        /* xw_dat = Write data to file - binary */
        case 21:
           save_datab();
           break;

        /* If here, the command was invalid - display the help */
        default:
           syntax();
           break;

        }
     }

}

/*******************************************************/
/* This commands free's the currently allocated device */
/* It issues a Deallocate Device IOCtl to GENSCSI      */
/*******************************************************/
int free_dev(USHORT handle)
{
   ULONG  psize;
   ULONG  dsize;
   USHORT pbuf;
   USHORT dbuf;

   /* Release the device */
   pbuf = handle;
   psize = 2;
   dsize = 2;
   dbuf = 0;
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x54,                      /* Device-specific function code */
      (PVOID *)&pbuf,                   /* Command-specific argument list */
      2,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&dbuf,                   /* Data area */
      2,                                /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) { printf("Dealloc device OK\n"); }
   else {
      printf("Dealloc device failed, retval=%d\n",(UCHAR)retval);
      dbuf = 0;
      }

   alloced = 0;
   return(retval);


}

/**************************************************************************/
/* This function does the Device Inquiry cmd.  It uses the Device Inquiry */
/* SCB instead of issuing the Inquiry CDB to the device.  If the command  */
/* worked, it displays the interpreted data.                              */
/**************************************************************************/
int    inquiry(USHORT handle)
{

   int            s_size;     /* How big a buffer to use */
   inq_type      *inq;        /* Pointer to the INQUIRY data for display */
   int            i;          /* Just a counter */
   xfer_scb_type  xfer;       /* Data buffer for Transfer SCB cmd    */
   scb_hdr_type   hdr;        /* An SCB/header pair                  */
   ULONG          psize;      /* Parm buffer size variable for IOCTL */
   ULONG          dsize;      /* Data buffer size variable for IOCTL */

   /* Set the device data block size */
   s_size = 255;

   /* allocate the buffer (if needed) and fill it with zeros */
   if (bufsize < s_size) {
      if (bufsize) { free(buffer); }
      buffer = malloc(s_size);
      bufsize = s_size;
      }
   for (i=0; i<bufsize; i++) { *(buffer+i) = '\0'; }

   /* Put something in psize and dsize */
   psize = 4;
   dsize = 2;

   /* Fill in the transfer SCB control block */
   xfer.dev_handle = handle;
   xfer.sense_size = SENSE_SIZE;
   xfer.scb_header = &hdr;
   xfer.flags      = 0;
   xfer.scb_phys   = 0;

   /* Fill in the SCB header */
   hdr.next_scb_hdr = NULL;
   hdr.tsb_adr = &tsb;
   hdr.reserved1          = 0;
   hdr.reserved2          = 0;
   hdr.reserved3          = 0;
   hdr.reserved4          = 0;

   /* Fill in the SCB */
   hdr.scb.tsb_adr        = &tsb;
   hdr.scb.block_count    = 0;
   hdr.scb.block_length   = 0;
   hdr.scb.chain_adr      = NULL;
   hdr.scb.block_number   = 0;
   hdr.scb.system_buf_cnt = s_size;
   hdr.scb.system_buf_adr = buffer;
   hdr.scb.command        = 0x1C4B;    /* This is the Inquiry command   */
   hdr.scb.enable         = 0xE600;    /* Along with these enable bits  */

   /* Do the command */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x52,                      /* Device-specific function code */
      (PVOID *)&xfer,                   /* Command-specific argument list */
      13,                               /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&sense,                  /* Data area */
      SENSE_SIZE,                       /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Inquiry went OK\n");
      inq = (inq_type *)buffer;
      printf("Device type  = %02X\n",inq->device_type);
      printf("Removable    = %02X\n",inq->removable);
      printf("Standards    = %02X\n",inq->standards);
      printf("Extra data   = %02X\n",inq->extra_length);
      printf("Mfg info     = %s\n",  inq->manufacturer);
      }
   else {
      printf("Inquiry failed, retval=%d\n",(UCHAR)retval);
      }
   return(retval);

}

/**************************************************************************/
/* This function reads up to 64K bytes from the device.  It asks the user */
/* for the starting sector (block), how many sectors (blocks) to read and */
/* how big each block is.  It then builds the READ SCB and sends it to    */
/* GENSCSI who will pass it on to OS2SCSI.  If it was successful, the     */
/* data is pointed to by the global variable 'buffer'.                    */
/*                                                                        */
/* Note that it doesn't check for the 64K limit imposed by GENSCSI.       */
/**************************************************************************/
int    read_dev(USHORT handle)
{
   char           sector_char[10];
   int            sector;
   char           count_char[10];
   int            count;
   char           size_char[10];
   int            s_size;
   FILE          *ofile;
   ULONG          psize;
   ULONG          dsize;
   xfer_scb_type  xfer;
   scb_hdr_type   hdr;

   /* Get the start, number, and size of sectors */
   printf("Enter sector to read:\n");
   gets(sector_char);
   sector = atoi(sector_char);

   printf("Enter number of sectors to read:\n");
   gets(count_char);
   count = atoi(count_char);

   printf("Enter number of bytes per sector:\n");
   gets(size_char);
   s_size = atoi(size_char);
   printf("Reading %d sectors\n",count);

   /* allocate the buffer (if needed) */
   if (bufsize < (count * s_size)) {
      if (bufsize) { free(buffer); }
      buffer = malloc(s_size * count);
      bufsize = count * s_size;
      }

   /* Set some dummy values in psize and dsize */
   psize = 4;
   dsize = 2;

   /* Fill in the Transfer SCB record */
   xfer.dev_handle = handle;
   xfer.sense_size = SENSE_SIZE;
   xfer.scb_header = &hdr;
   xfer.flags      = 0;
   xfer.scb_phys   = 0;

   /* Fill in the SCB Header */
   hdr.next_scb_hdr = NULL;
   hdr.tsb_adr = &tsb;
   hdr.reserved1          = 0;
   hdr.reserved2          = 0;
   hdr.reserved3          = 0;
   hdr.reserved4          = 0;

   /* Fill in the SCB */
   hdr.scb.tsb_adr        = &tsb;
   hdr.scb.block_count    = count;
   hdr.scb.block_length   = s_size;
   hdr.scb.chain_adr      = NULL;
   hdr.scb.block_number   = sector;
   hdr.scb.system_buf_cnt = s_size * count;
   hdr.scb.system_buf_adr = buffer;
   hdr.scb.command        = 0x1C41;  /* This is the READ command      */
   hdr.scb.enable         = 0xE000;  /* Along with these enable bits  */

   /* Do the command  */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x52,                      /* Device-specific function code */
      (PVOID *)&xfer,                   /* Command-specific argument list */
      13,                               /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&sense,                  /* Data area */
      SENSE_SIZE,                       /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) { printf("READ went OK\n"); }
   else { printf("READ failed, retval=%d\n",(UCHAR)retval); }
   return(retval);

}

/**************************************************************************/
/* This function writes up to 64K bytes to the device from a file.  It    */
/* asks the same questions as read_dev, but also asks for the name of the */
/* file containing the data.  Then, it opens the file and reads as much   */
/* as needed into the buffer.  The results of the read are ignored (hey,  */
/* this is a test and demo program, whadayawant!?).  The write SCB is     */
/* then built and sent to OS2SCSI and the results displayed.              */
/*                                                                        */
/* As in read_dev, the 64K limit is not checked.                          */
/**************************************************************************/
int    write_dev(USHORT handle)
{
   char           sector_char[10];
   int            sector;
   char           count_char[10];
   int            count;
   char           size_char[10];
   int            s_size;
   char           fname[100];
   FILE          *ifile;
   int            i;
   ULONG          psize;
   ULONG          dsize;
   xfer_scb_type  xfer;
   scb_hdr_type   hdr;

   /* Get the start, number, and size of sectors */
   printf("Enter starting sector to write:\n");
   gets(sector_char);
   sector = atoi(sector_char);

   printf("Enter number of sectors to write:\n");
   gets(count_char);
   count = atoi(count_char);

   printf("Enter number of bytes per sector:\n");
   gets(size_char);
   s_size = atoi(size_char);
   printf("Reading %d sectors\n",count);

   /* Get the name of the file holding the data */
   printf("Enter name of file to get data from:\n");
   gets(fname);

   /* allocate the buffer (if needed) */
   if (bufsize < (count * s_size)) {
      if (bufsize) { free(buffer); }
      buffer = malloc(s_size * count);
      bufsize = count * s_size;
      }
   for (i=0; i<bufsize; i++) { *(buffer+i) = '\0'; }

   /* Load the file into the buffer */
   ifile = fopen(fname,"rb");
   fread(buffer,1,bufsize,ifile);
   fclose(ifile);

   /* The usual variable settings ... dummy sizes */
   psize = 4;
   dsize = 2;

   /* Fill the transfer SCB */
   xfer.dev_handle = handle;
   xfer.sense_size = SENSE_SIZE;
   xfer.scb_header = &hdr;
   xfer.flags      = 0;
   xfer.scb_phys   = 0;

   /* Fill the SCB Header */
   hdr.next_scb_hdr = NULL;
   hdr.tsb_adr = &tsb;
   hdr.reserved1          = 0;
   hdr.reserved2          = 0;
   hdr.reserved3          = 0;
   hdr.reserved4          = 0;

   /* Fill the SCB */
   hdr.scb.tsb_adr        = &tsb;
   hdr.scb.block_count    = count;
   hdr.scb.block_length   = s_size;
   hdr.scb.chain_adr      = NULL;
   hdr.scb.block_number   = sector;
   hdr.scb.system_buf_cnt = s_size * count;
   hdr.scb.system_buf_adr = buffer;
   hdr.scb.command        = 0x1C42;      /* This is the write command     */
   hdr.scb.enable         = 0x6000;      /* Along with these enable bits  */

   /* Do it */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x52,                      /* Device-specific function code */
      (PVOID *)&xfer,                   /* Command-specific argument list */
      13,                               /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&sense,                  /* Data area */
      SENSE_SIZE,                       /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("WRITE went OK\n");
      }
   else {
      printf("WRITE failed, retval=%d\n",(UCHAR)retval);
      }
   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function reads the current sense data from the device into the      */
/*  global sense buffer.  It uses the SENSE SCB instead of the 0x03 command. */
/*                                                                           */
/*****************************************************************************/
int    read_sense(USHORT handle)
{

   char          tempsense[100]; /* This is the sense buffer for this cmd    */
   int           i;              /* Just a counter                           */
   ULONG         psize;          /* The usual 4 variables : dummy sizes      */
   ULONG         dsize;          /*                                          */
   xfer_scb_type xfer;           /* The transfer SCB and the SCB header      */
   scb_hdr_type  hdr;            /*    (which holds the SCB, as well)        */

   /* Empty the temporary sense buffer */
   for (i=0; i<100; i++) {tempsense[i] = '\0'; }

   /* The usual filling of variables   */
   psize = 4;
   dsize = 2;

   /* Fill the transfer SCB   */
   xfer.dev_handle = handle;
   xfer.sense_size = 0;
   xfer.scb_header = &hdr;
   xfer.flags      = 0;
   xfer.scb_phys   = 0;

   /* Fill the SCB Header */
   hdr.next_scb_hdr = NULL;
   hdr.tsb_adr = &tsb;
   hdr.reserved1          = 0;
   hdr.reserved2          = 0;
   hdr.reserved3          = 0;
   hdr.reserved4          = 0;

   /* Fill the SCB itself */
   hdr.scb.tsb_adr        = &tsb;
   hdr.scb.block_count    = 0;
   hdr.scb.block_length   = 0;
   hdr.scb.chain_adr      = NULL;
   hdr.scb.block_number   = 0;
   hdr.scb.system_buf_cnt = 255;
   hdr.scb.system_buf_adr = sense;
   hdr.scb.command        = 0x1C48;   /* This is the sense command     */
   hdr.scb.enable         = 0xE600;   /* Along with these enable bits  */

   /* Do it */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x52,                      /* Device-specific function code */
      (PVOID *)&xfer,                   /* Command-specific argument list */
      13,                               /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&tempsense,              /* Data area */
      100,                              /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) { printf("Sense went OK\n"); }
   else {
      printf("Sense failed, retval=%d\n",(UCHAR)retval);
      printf("Sense data = ");
      hexdisp(tempsense, 16);
      }
   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function returns the number of blocks and the size of each block    */
/*  for the media currently in the device.  It uses the Device Capacity SCB. */
/*                                                                           */
/*****************************************************************************/
int    dev_capacity(USHORT handle)
{

   ULONG         cap[2];        /* A block to hold the returned data         */
   int           i;             /* A counter                                 */
   ULONG         last_block;    /* The last block number                     */
   ULONG         block_len;     /* The size of each block                    */
   ULONG         psize;         /* A holder for the parm buffer size         */
   ULONG         dsize;         /* A holder for the data buffer size         */
   xfer_scb_type xfer;          /* The Transfer SCB                          */
   scb_hdr_type  hdr;           /* The SCB                                   */

   /* Zero the target block */
   for (i=0; i<2; i++) {cap[i] = 0; }

   /* Set some variables to dummy values */
   psize = 4;
   dsize = 2;

   /* Fill in the Transfer SCB */
   xfer.dev_handle = handle;
   xfer.sense_size = 0;
   xfer.scb_header = &hdr;
   xfer.flags      = 0;
   xfer.scb_phys   = 0;

   /* Fill in the SCB Header */
   hdr.next_scb_hdr = NULL;
   hdr.tsb_adr = &tsb;
   hdr.reserved1          = 0;
   hdr.reserved2          = 0;
   hdr.reserved3          = 0;
   hdr.reserved4          = 0;

   /* Fill in the SCB itself */
   hdr.scb.tsb_adr        = &tsb;
   hdr.scb.block_count    = 0;
   hdr.scb.block_length   = 0;
   hdr.scb.chain_adr      = NULL;
   hdr.scb.block_number   = 0;
   hdr.scb.system_buf_cnt = 8;
   hdr.scb.system_buf_adr = (UCHAR *)cap;
   hdr.scb.command        = 0x1C49;     /* This is the Capacity command  */
   hdr.scb.enable         = 0xE400;     /* Along with these enable bits  */

   /* Do the command (transfer SCB) */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x52,                      /* Device-specific function code */
      (PVOID *)&xfer,                   /* Command-specific argument list */
      13,                               /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&sense,                  /* Data area */
      100,                              /* length of data area  */
      &dsize                            /* Data area */
      );

   /* If it worked, make the values little-endian and display them */
   if (retval == 0) {
      printf("Device Capacity went OK\n");
      last_block = swap32(cap[0]);
      block_len = swap32(cap[1]);
      printf("Last block = %d, Block length = %d bytes\n",last_block, block_len);
      }
   else {
      printf("Device capacity failed, retval=%d\n",(UCHAR)retval);
      }
   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function sets the timeout value for a particular device.  It issues */
/*  the Set Timeout command to OS2SCSI.                                      */
/*                                                                           */
/*****************************************************************************/
int  timeout_dev(USHORT handle)
{

   int           timeval;     /* The timeout (in millisec)                  */
   char          entry[10];   /* A place to hold the input string           */
   ULONG         psize;       /* The size of the parm buffer                */
   ULONG         dsize;       /* The size of the data buffer                */
   timeout_type  tmt;         /* The record sent to OS2SCSI                 */

   /* Get the timeout value */
   printf("Enter timeout value in milliseconds:\n");
   gets(entry);
   timeval = atoi(entry);

   /* Store it and the device handle in the buffer to be sent */
   tmt.handle = handle;
   tmt.timeout = timeval;

   /* Set the parm and data buffer sizes */
   psize = 6;
   dsize = 0;

   /* Do the request */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x50,                      /* Device-specific function code */
      (PVOID *)&tmt,                    /* Command-specific argument list */
      6,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)NULL,                    /* Data area */
      0,                                /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Set Timeout (0x%08X milliseconds) went OK\n",tmt.timeout);
      }
   else {
      printf("Set timeout failed, retval=%d\n",(UCHAR)retval);
      }

   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function reads the timeout value for a particular device.  It does  */
/*  the Get Timeout command to OS2SCSI.                                      */
/*                                                                           */
/*****************************************************************************/
int  read_timeout_dev(USHORT handle)
{

   int          timeval;  /* Where the answer is going to go                 */
   ULONG        psize;    /* The size of the parm buffer                     */
   ULONG        dsize;    /* The size of the data buffer                     */
   timeout_type tmt;      /* The structure we are going to send to OS2SCSI   */

   /* Clear the target variable */
   timeval = 0;

   /* Set the device handle into the structure to be sent */
   tmt.handle = handle;

   /* Set the parm and data buffer sizes */
   psize = 2;
   dsize = 4;

   /* Do the command */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x51,                      /* Device-specific function code */
      (PVOID *)&tmt,                    /* Command-specific argument list */
      4,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&timeval,                /* Data area */
      4,                                /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Device timeout = 0x%08X milliseconds\n",timeval);
      }
   else {
      printf("Read timeout failed, retval=%d\n",(UCHAR)retval);
      }

   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function requests that a device of a given type be allocated to     */
/*  this application.  It asks the user for the peripheral type (cdrom,      */
/*  tape, etc) and whether the media is removable.  It then builds the       */
/*  request and sends it to OS2SCSI.  If the results are successful, it      */
/*  returns the handle.                                                      */
/*                                                                           */
/*****************************************************************************/
int  allocate_dev(USHORT *handle)
{

   int    dev_type, periph_type;
   char   entry[10];
   ULONG  psize;
   ULONG  dsize;
   USHORT dbuf;
   USHORT alloc_pbuf[2];

   /* Get the device type and peripheral type values */
   printf("Enter device type:\n");
   gets(entry);
   periph_type = atoi(entry);
   printf("Removable (y/n)?\n");
   gets(entry);
   dev_type = 0;
   if (121 == (UCHAR)entry[0]) { dev_type  = 0x8000; }
   if ( 89 == (UCHAR)entry[0]) { dev_type  = 0x8000; }

   /* Fill the parm buffer with the requested device info */
   alloc_pbuf[0] = (USHORT)(dev_type + periph_type);
   alloc_pbuf[1] = 0x0000;

   /* Set the parm and data buffer sizes */
   psize = 4;
   dsize = 2;

   /* Zero the target buffer (where the device handle will go */
   dbuf = 0;

   /* Do the command */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x55,                      /* Device-specific function code */
      (PVOID *)&alloc_pbuf,             /* Command-specific argument list */
      4,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&dbuf,                   /* Data area */
      2,                                /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Allocate device went OK, handle = %d\n",dbuf);
      }
   else {
      printf("Allocate device failed, retval=%d\n",(UCHAR)retval);
      dbuf = 0;
      return;
      }

   /* If it worked, note that we have a device allocated and return the */
   /* handle to the caller                                              */
   alloced = 1;
   *handle = dbuf;
   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function asks how many devices of a certain type are known or       */
/*  defined to OS2SCSI.  It does this by issuing the Device Count command    */
/*  for both removable and non-removable media after asking the user for the */
/*  device type.                                                             */
/*                                                                           */
/*  Note that some systems will return the number of devices defined to      */
/*  BIOS, not how many are really in the system.                             */
/*                                                                           */
/*****************************************************************************/
void count_dev(void)
{

   int    dev_type;
   char   entry[10];
   ULONG  psize;
   ULONG  dsize;
   USHORT pbuf;
   USHORT dbuf;

   /* Get the device type and peripheral type values */
   printf("Enter device type:\n");
   gets(entry);
   dev_type = atoi(entry);

   /* Ask for non-removable media count */
   pbuf = (USHORT)(dev_type);
   psize = 2;
   dsize = 2;
   dbuf = 0;
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x56,                      /* Device-specific function code */
      (PVOID *)&pbuf,                   /* Command-specific argument list */
      2,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&dbuf,                   /* Data area */
      2,                                /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Device type count OK, count = %d non-removable\n",dbuf);
      }
   else {
      printf("Device type count failed, retval=%d\n",(UCHAR)retval);
      }

   /* Now do removable media count */
   pbuf = (USHORT)(0x8000+dev_type);
   psize = 2;
   dsize = 2;
   dbuf = 0;
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x56,                      /* Device-specific function code */
      (PVOID *)&pbuf,                   /* Command-specific argument list */
      2,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&dbuf,                   /* Data area */
      2,                                /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Device type count OK, count = %d removable\n",dbuf);
      }
   else {
      printf("Device type count failed, retval=%d\n",(UCHAR)retval);
      }

   return;

}

/*****************************************************************************/
/*                                                                           */
/*  This function issues the Reset command to OS2SCSI for a given device     */
/*  handle.                                                                  */
/*                                                                           */
/*  Note that I've seen this command hang the system, for some unknown       */
/*  reason.  It seems to make the call to OS2SCSI, but it never returns.     */
/*  Other times, the command works just fine.                                */
/*                                                                           */
/*****************************************************************************/
int  reset_dev(USHORT handle)
{
   ULONG   psize;
   ULONG   dsize;
   USHORT  dbuf;
   USHORT  alloc_pbuf[2];

   alloc_pbuf[0] = handle;
   alloc_pbuf[1] = SENSE_SIZE;
   psize = 4;
   dsize = SENSE_SIZE;
   dbuf = 0;
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x45,                      /* Device-specific function code */
      (PVOID *)&alloc_pbuf,             /* Command-specific argument list */
      4,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&sense,                  /* Data area */
      SENSE_SIZE,                       /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Reset device went OK\n");
      }
   else {
      printf("Reset device failed, retval=%d\n",(UCHAR)retval);
      }
   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function issues the Return Device Parms command for a given device  */
/*  handle.                                                                  */
/*                                                                           */
/*  Note that I've not seen this command return useful (or even accurate)    */
/*  information.                                                             */
/*                                                                           */
/*****************************************************************************/
int  read_dev_parms(USHORT handle)
{
   devparm_type      devparms;
   ULONG             psize;
   ULONG             dsize;
   USHORT            pbuf;

   /* Fill the buffer with junk to prove that something happened */
   devparms.dev_key_index   = 0xFFFF;
   devparms.scb_arch_lvl    = 0xFF;
   devparms.adapter_index   = 0xFF;
   devparms.device_flags    = 0xFFFF;
   devparms.lun             = 0xFF;
   devparms.pun             = 0xFF;

   /* Build the command */
   pbuf = handle;
   psize = 2;
   dsize = 8;

   /* Do it */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x43,                      /* Device-specific function code */
      (PVOID *)&pbuf,                   /* Command-specific argument list */
      2,                                /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&devparms,               /* Data area */
      8,                                /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("Read device parms went OK\n");
      printf("  Device key index = %04X\n",devparms.dev_key_index);
      printf("  SCB Arch level   = %02X\n",devparms.scb_arch_lvl);
      printf("  Adapter index    = %02X\n",devparms.adapter_index);
      printf("  Device flags     = %04X\n",devparms.device_flags);
      printf("  Logical Unit #   = %02X\n",devparms.lun);
      printf("  Physical Unit #  = %02X\n",devparms.pun);
      }
   else {
      printf("Read device parms failed, retval=%d\n",(UCHAR)retval);
      }
   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function lets the user build any 'short' SCB and send it to OS2SCSI */
/*  for execution.  It's basic operation is to ask the user for each element */
/*  of the SCB and to put that value into the control block.  When it is     */
/*  done building it, it displays the SCB and then sends it to OS2SCSI.      */
/*                                                                           */
/*****************************************************************************/
int    gen_cmd(USHORT handle)
{
   char           entry[10];
   char          *stopstr;
   ULONG          ent_val;
   ULONG          psize;
   ULONG          dsize;
   xfer_scb_type  xfer;
   scb_hdr_type   hdr;

   /* SCB Command word */
   printf("Command Word:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   hdr.scb.command        = ent_val;

   /* SCB Enable word */
   printf("Enable Word:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   hdr.scb.enable         = ent_val;

   /* The Logical Address (what bock to read/write/seek to,modify,whatever) */
   printf("Logical Address:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   hdr.scb.block_number  = ent_val;

   /* How big is the buffer */
   printf("Buffer size:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   hdr.scb.system_buf_cnt = ent_val;

   /* How big is a block */
   printf("Block size:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   hdr.scb.block_length = ent_val;

   /* How many blocks are involved */
   printf("Block count:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   hdr.scb.block_count = ent_val;

   /* allocate the buffer (if needed) */
   if (bufsize < ent_val) {
      if (bufsize) { free(buffer); }
      buffer = malloc(ent_val);
      bufsize = ent_val;
      }

   /* Here is our normal Transfer SCB operation */
   psize = 4;
   dsize = 2;
   xfer.dev_handle = handle;
   xfer.sense_size = SENSE_SIZE;
   xfer.scb_header = &hdr;
   xfer.flags      = 0;
   xfer.scb_phys   = 0;

   hdr.next_scb_hdr = NULL;
   hdr.tsb_adr = &tsb;

   hdr.reserved1          = 0;
   hdr.reserved2          = 0;
   hdr.reserved3          = 0;
   hdr.reserved4          = 0;
   hdr.scb.tsb_adr        = &tsb;
   hdr.scb.chain_adr      = NULL;
   hdr.scb.system_buf_adr = buffer;

   /* Display the SCB prior to execution */
   printf("Attempting GENERAL SCSI command :\n");
   printf("Command word = %04X\n",hdr.scb.command);
   printf("Enable word  = %04X\n",hdr.scb.enable);
   printf("Buffer size  = %08X\n",hdr.scb.system_buf_cnt);
   printf("Block number = %08X\n",hdr.scb.block_number);
   printf("Block length = %04X\n",hdr.scb.block_length);
   printf("Block count  = %04X\n",hdr.scb.block_count);

   /* Do it */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x52,                      /* Device-specific function code */
      (PVOID *)&xfer,                   /* Command-specific argument list */
      13,                               /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&sense,                  /* Data area */
      SENSE_SIZE,                       /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("GEN_CMD went OK\n");
      }
   else {
      printf("GEN_CMD failed, retval=%d\n",(UCHAR)retval);
      }
   return(retval);

}

/*****************************************************************************/
/*                                                                           */
/*  This function lets the user build a SCSI command and send it to the      */
/*  device via the Send Other SCB subcommand of the Transfer SCB command.    */
/*  It asks the user for the size of the SCSI command (in bytes) and then    */
/*  for each byte of the command,  It then allocates a buffer, builds the    */
/*  Send Other SCB and sends it.                                             */
/*                                                                           */
/*****************************************************************************/
int    other(USHORT handle)
{
   char              entry[10];
   char             *stopstr;
   ULONG             ent_val;
   ULONG             cmd_len;
   int               i;
   ULONG             psize;
   ULONG             dsize;
   xfer_lscb_type    xfer;
   long_scb_hdr_type lhdr;

   /* Fill in the SCB */
   lhdr.scb.command        = 0x245F;   /* This is the Send Other SCB    */
   lhdr.scb.enable         = 0x6600;   /* Along with these enable bits  */

   /* Ask for the number of bytes */
   printf("Number of bytes in command:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   lhdr.scb.cmd_len  = (UCHAR)ent_val;
   cmd_len = ent_val;

   /* Get the SCSI command */
   for (i=0; i<cmd_len; i++) {
       printf("Command byte %1d:\n",i);
       gets(entry);
       ent_val = strtoul(entry, &stopstr, 0);
       lhdr.scb.scsi_cmd[i]  = (UCHAR)ent_val;
       }

   /* Get the buffer size and allocate it if needed */
   printf("Buffer size:\n");
   gets(entry);
   ent_val = strtoul(entry, &stopstr, 0);
   lhdr.scb.system_buf_cnt = ent_val;
   if (bufsize < ent_val) {
      if (bufsize) { free(buffer); }
      buffer = malloc(ent_val);
      bufsize = ent_val;
      }

   /* Figyre out how to set the rd/wr bit in the SCB enable word */
   printf("Read/Write (w/r)?\n");
   gets(entry);
   if (114 == (UCHAR)entry[0]) { lhdr.scb.enable |= 0x8000; }
   if ( 82 == (UCHAR)entry[0]) { lhdr.scb.enable |= 0x8000; }

   /* Our normal transfer SCB processing */
   psize = 4;
   dsize = 2;
   xfer.dev_handle = handle;
   xfer.sense_size = SENSE_SIZE;
   xfer.scb_header = &lhdr;
   xfer.flags      = 1;
   xfer.scb_phys   = 0;

   lhdr.next_scb_hdr = NULL;
   lhdr.tsb_adr = &tsb;

   lhdr.reserved1          = 0;
   lhdr.reserved2          = 0;
   lhdr.reserved3          = 0;
   lhdr.reserved4          = 0;
   lhdr.scb.tsb_adr        = &tsb;
   lhdr.scb.chain_adr      = NULL;
   lhdr.scb.system_buf_adr = buffer;

   /* Display the command before processing */
   printf("Attempting OTHER SCSI command :\n");
   for (i=0; i<cmd_len; i++) {
      printf("Command byte %02X = %02X\n",i,lhdr.scb.scsi_cmd[i]);
      }

   /* Do it */
   retval = DosDevIOCtl (
      dh,                               /* Device handle returned by Open */
      (ULONG)0x80,                      /* Device category */
      (ULONG)0x52,                      /* Device-specific function code */
      (PVOID *)&xfer,                   /* Command-specific argument list */
      13,                               /* length of parm list  */
      &psize,                           /* length of parm list  */
      (PVOID *)&sense,                  /* Data area */
      SENSE_SIZE,                       /* length of data area  */
      &dsize                            /* Data area */
      );

   /* Tell how it went */
   if (retval == 0) {
      printf("OTHER went OK\n");
      }
   else {
      printf("OTHER failed, retval=%d\n",(UCHAR)retval);
      }
   return(retval);

}

/* This function displays the valid commands */
void syntax(void)
{
   printf("Commands:\n");
   printf("quit    = quit                     ?       = help\n");
   printf("alloc   = Allocate a device        free    = free allocated device\n");
   printf("gen     = General SCB              other   = Send Other SCB\n");
   printf("capac   = Show device capacity     inq     = Device inquiry\n");
   printf("count   = Get device count         parms   = Get device parms\n");
   printf("timeout = Set device timeout       rtime   = Read device timeout\n");
   printf("reset   = Reset device\n");
   printf("\n");
   printf("read    = Read data from device (up to 64K bytes)\n");
   printf("p_data  = Display data buffer (up to 64 bytes)\n");
   printf("w_dat   = Write data to disk - printable form\n");
   printf("xw_dat  = Write data to disk - binary\n");
   printf("write   = Write data from file to device\n");
   printf("\n");
   printf("sense   = Get device sense data\n");
   printf("p_sns   = Display sense (interpreted)\n");
   printf("xp_sns  = Display Sense (hex)\n");
   printf("p_tsb   = Display tsb (interpreted)\n");
   printf("xp_tsb  = Display tsb (hex)\n");
}

/* This function displays a buffer in printable format */
void hexdisp(UCHAR *data, int count)
{
   char buffer[80];
   int  i;
   int  maxdisp;
   int  remaining;
   int  offset;

   remaining = count;
   offset = 0;
   while (remaining) {
      maxdisp = 16;
      if (16 > remaining) maxdisp = remaining;
      for (i=0; i<maxdisp; i++) { sprintf((buffer+(i*3)),"%02X ",*(data+offset+i)); }
      printf("%s\n",buffer);
      remaining -= maxdisp;
      offset += 16;
      }

}

/* This function writes a buffer to the output file in printable format */
void hexsave(UCHAR *data, int count)
{
   char buffer[80];
   int  i;
   int  maxdisp;
   int  remaining;
   int  offset;
   FILE *ofile;

   ofile = fopen("scsitest.dat","w");
   remaining = count;
   offset = 0;
   while (remaining) {
      maxdisp = 16;
      if (16 > remaining) maxdisp = remaining;
      for (i=0; i<maxdisp; i++) { sprintf((buffer+(i*3)),"%02X ",*(data+offset+i)); }
      fprintf(ofile,"%s\n",buffer);
      remaining -= maxdisp;
      offset += 16;
      }
   fclose(ofile);

}

/* This function writes a buffer to the output file in binary format */
void binsave(UCHAR *data, int count)
{
   FILE *ofile;

   ofile = fopen("scsitest.dat","wb");
   fwrite(data,1, count, ofile);
   fclose(ofile);

}

/* This function displays the Sense buffer in hex format */
void show_sense_x(void)
{
   hexdisp(sense, SENSE_SIZE);
}

/* This function displays the TSB buffer in hex format */
void show_tsb_x(void)
{
   hexdisp((UCHAR *)&tsb, sizeof(tsb_type));
}

/* This function displays the TSB buffer in interpreted format */
void show_tsb(void)
{
   printf("SCB_status            = %04X\n",tsb.SCB_status);
   printf("Retries               = %04X\n",tsb.retry_counts);
   printf("Residual Count        = %08X\n",tsb.residual_buffer_byte_count);
   printf("Residual Address      = %08X\n",tsb.residual_buffer_address);
   printf("Additional status len = %04X\n",tsb.additional_status_length);
   printf("SCSI Status code      = %01X\n",tsb.SCSI_status_code);
   printf("Command Status        = %02X\n",tsb.command_status);
   printf("Device error code     = %02X\n",tsb.device_error_code);
   printf("Command error code    = %02X\n",tsb.command_error_code);
   printf("Error modifier        = %04X\n",tsb.error_modifier);
   printf("Cache info            = %04X\n",tsb.cache_info);
   printf("Last SCB processed    = %04X\n",tsb.last_SCB_address_processed);
}

/* This function displays the sense buffer in interpreted format */
void show_sense(void)
{
   printf("Info Valid       = %01X\n",(sense[0] >> 7));
   printf("Error Class      = %01X\n",7 & (sense[0] >> 4));
   printf("Error Code       = %01X\n",(sense[0] & 0x0F));
   printf("Segment Number   = %02X\n",sense[1]);
   printf("Filemark seen    = %01X\n",(sense[2] >> 7));
   printf("End of Medium    = %01X\n",((0x40 & sense[2]) >> 6));
   printf("Invalid Length   = %01X\n",((0x20 & sense[2]) >> 5));
   printf("Sense Key        = %01X\n",(sense[2] & 0x0F));
   printf("Additional Data  = %02X\n",sense[12]);
   printf("Additional Data1 = %02X\n",sense[13]);
}

/* This function displays 64 bytes of a buffer */
void show_data(void)
{
   hexdisp(buffer, 64);
}

/* This function writes a buffer to the output file in printable form */
void save_data(void)
{
   hexsave(buffer, bufsize);
}

/* This function writes a buffer to the output file in binary */
void save_datab(void)
{
   binsave(buffer, bufsize);
}

/* This function fulls the sense buffer with 0's */
void clear_sense(void)
{
   int i;
   for (i=0; i<SENSE_SIZE; i++) { sense[i] = '\0'; }
}

/* This function reverses the byte order in a 16-bit value */
USHORT swap16(USHORT inval)
{
   USHORT temp;

   temp = 256*(inval  & 0xFFFF);
   temp += (inval >> 8);
   return(temp);

}

/* This function reverses the byte order in a 32-bit value */
ULONG swap32(ULONG inval)
{
   ULONG temp;

   temp = 0;
   temp += (inval >> 24) &  0x000000FF;
   temp += (inval >>  8) &  0x0000FF00;
   temp += (inval <<  8) &  0x00FF0000;
   temp += (inval << 24) &  0xFF000000;
   return(temp);

}

