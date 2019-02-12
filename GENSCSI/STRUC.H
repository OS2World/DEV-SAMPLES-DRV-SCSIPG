/*****************************************************************************/
/* This file contains all internal structures, unions, and typedefs used by  */
/* the DD.                                                                   */
/*****************************************************************************/
/*****************************************************************************/
/* Definition of a generic 32 bit value.  It can be a far pointer, 2 words   */
/* A segmented address or a 32 bit unsigned integer (probably a phys addr)   */
/*****************************************************************************/
#ifndef NEWSCALARS
   typedef unsigned      word;
   typedef unsigned char byte;
   typedef word          boolean;
   #define NEWSCALARS 1
   #endif

typedef unsigned long physaddr;  /* A physical address is 4 bytes */
typedef void far * farptr;

typedef struct _2words_def {
  unsigned low;
  unsigned high;
  } _2words_type;

typedef struct _segaddr_def {
  unsigned offset;
  unsigned segment;
  } _segadr_type;

typedef union _32bits_def {
   physaddr phys;
   void far * fptr;
   _2words_type _2words;
   _segadr_type _segadr;
   } _32bits;


/*****************************************************************************/
/* Definition of a the various request packets                               */
/*****************************************************************************/
struct reqhdr {
   byte           rh_len;
   byte           rh_unit;
   byte           rh_cmd;
   word           rh_stat;
   byte          *rh_resrvd;
   struct reqhdr *rh_next;
   };

typedef struct reqhdr reqhdr_type;

struct init_hdr_in {
   struct reqhdr  request_hdr;
   byte           data1;
   _32bits        pointer1;
   byte          *pointer2;
   byte           data2;
   };

struct init_hdr_out {
   struct reqhdr  request_hdr;
   byte           data1;
   word           code_end;
   word           data_end;
   byte          *pointer2;
   byte           data2;
   };

struct ioctl_hdr_in {
   struct reqhdr  request_hdr;
   byte           funct_cat;
   byte           funct_cod;
   _32bits        pbuffer;
   _32bits        dbuffer;
   word           file_number;
   };
typedef struct ioctl_hdr_in ioctl_hdr;

struct ioctl_hdr2_in {
   struct reqhdr  request_hdr;
   byte           funct_cat;
   byte           funct_cod;
   _32bits        pbuffer;
   _32bits        dbuffer;
   word           file_number;
   word           pbuf_len;
   word           dbuf_len;
   };
typedef struct ioctl_hdr2_in ioctl_hdr2;

struct close_hdr_def {
   byte           rh_len;
   byte           rh_unit;
   byte           rh_cmd;
   word           rh_stat;
   byte          *rh_resrvd;
   struct reqhdr *rh_next;
   word           file_number;
   };
typedef struct close_hdr_def close_hdr;
typedef struct close_hdr_def open_hdr;

struct rw_hdr_def {
   byte            rh_len;
   byte            rh_unit;
   byte            rh_cmd;
   word            rh_stat;
   byte           *rh_resrvd;
   struct reqhdr  *rh_next;
   byte            media;
   byte           *xfer_address;
   word            count;
   unsigned long   start;
   word            file_number;
   };
typedef struct rw_hdr_def rw_hdr;

/*****************************************************************************/
/*                                                                           */
/* Structures used by dev_helper function interface                          */
/*                                                                           */
/*****************************************************************************/
struct regs {
   unsigned AX;
   unsigned BX;
   unsigned CX;
   unsigned DX;
   unsigned SI;
   unsigned DI;
   unsigned ES;
   unsigned DS;
   unsigned CS;
   unsigned flags;
   char  es_valid;
   char  ds_valid;
   };

struct regs_B {
   unsigned char AL, AH;
   unsigned char BL, BH;
   unsigned char CL, CH;
   unsigned char DL, DH;
   unsigned SI;
   unsigned DI;
   unsigned ES;
   unsigned DS;
   unsigned CS;
   unsigned flags;
   char  es_valid;
   char  ds_valid;
   };

union cpu_regs {
   struct regs W;
   struct regs_B B;
   };

struct regs32 {
   unsigned long EAX;
   unsigned long EBX;
   unsigned long ECX;
   unsigned long EDX;
   unsigned long ESI;
   unsigned long EDI;
   unsigned ES;
   unsigned DS;
   unsigned CS;
   unsigned long eflags;
   char  es_valid;
   char  ds_valid;
   };

struct regs32_B {
   unsigned char AL, AH;
   unsigned EAX_HI;
   unsigned char BL, BH;
   unsigned EBX_HI;
   unsigned char CL, CH;
   unsigned ECX_HI;
   unsigned char DL, DH;
   unsigned EDX_HI;
   unsigned SI;
   unsigned ESI_HI;
   unsigned DI;
   unsigned EDI_HI;
   unsigned ES;
   unsigned DS;
   unsigned CS;
   unsigned flags;
   unsigned eflag_HI;
   char  es_valid;
   char  ds_valid;
   };

struct regs32_W {
   unsigned AX;
   unsigned EAX_HI;
   unsigned BX;
   unsigned EBX_HI;
   unsigned CX;
   unsigned ECX_HI;
   unsigned DX;
   unsigned EDX_HI;
   unsigned SI;
   unsigned ESI_HI;
   unsigned DI;
   unsigned EDI_HI;
   unsigned ES;
   unsigned DS;
   unsigned CS;
   unsigned flags;
   unsigned eflag_HI;
   char  es_valid;
   char  ds_valid;
   };

union cpu_regs32 {
   struct regs32   D;
   struct regs32_W W;
   struct regs32_B B;
   };

/*****************************************************************************/
/*                                                                           */
/* Structure for holding Inter-device driver call info                       */
/*                                                                           */
/*****************************************************************************/
typedef struct idc_def {
   word real_IP;
   word real_CS;
   word real_DS;
   word prot_IP;
   word prot_CS;
   word prot_DS;
   } idc_type;

/*****************************************************************************/
/*                                                                           */
/* Transfer SCB Parm buffer definition                                       */
/*                                                                           */
/*****************************************************************************/
typedef struct xfer_scb_def {
   word    dev_handle;
   word    sense_size;
   _32bits scb_phys;
   _32bits scb_header;
   byte    flags;
   } xfer_scb_type;

/*****************************************************************************/
/*                                                                           */
/* SCB's and headers                                                         */
/*                                                                           */
/*****************************************************************************/
typedef struct scb_def {
   word     command;
   word     enable;
   _32bits  block_number;
   _32bits  system_buf_adr;
   _32bits  system_buf_cnt;
   _32bits  tsb_adr;
   _32bits  chain_adr;
   word     block_count;
   word     block_length;
   } scb_type;

typedef struct long_scb_def {
   word     command;
   word     enable;
   _32bits  reserved;
   _32bits  system_buf_adr;
   _32bits  system_buf_cnt;
   _32bits  tsb_adr;
   _32bits  chain_adr;
   byte     scsi_cmd[12];
   } long_scb_type;

typedef struct scb_hdr_def {
   word      reserved1;
   _32bits   next_scb_hdr;
   word      reserved2;
   word      reserved3;
   _32bits   tsb_adr;
   word      reserved4;
   scb_type  scb;
   } scb_hdr_type;

typedef struct long_scb_hdr_def {
   word           reserved1;
   _32bits        next_scb_hdr;
   word           reserved2;
   word           reserved3;
   _32bits        tsb_adr;
   word           reserved4;
   long_scb_type  scb;
   } long_scb_hdr_type;

typedef struct pagelist_def {
   unsigned long page_phys;
   unsigned long page_size;
   } pagelist_type;
