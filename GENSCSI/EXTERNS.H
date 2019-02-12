/*****************************************************************************/
/* This file contains function prototypes for all functions in the DD        */
/*****************************************************************************/

/* Debugger breakpoint function */
extern void far breakpoint(void);

/* OS/2 Request packet command processors */
extern word far init_mod( struct init_hdr_in *);
extern int near strategy_c(struct reqhdr *);
extern int far  open(reqhdr_type *);
extern int far  close(reqhdr_type *);

/* Print a message on the display (INIT time only) */
extern unsigned far prt_msg(unsigned);
extern void far make_msg_fname( char *);

/* Linear addressing functions */
extern _32bits far virt_to_lin(_32bits);
extern void    far lin_to_pages(_32bits, _32bits, _32bits);


/* Create an LDT descriptor for a physical address                        */
/* Returns 0:0 if unsuccessful                                            */
extern _32bits far phys_to_ldt(_32bits,               /* Physical address */
                         unsigned);                   /* Size of block    */


/* Create an LDT descriptor for a phys addr (uses far call to devhlp)     */
/* Returns 0:0 if unsuccessful                                            */
extern _32bits far phys_to_ldt1(_32bits,              /* Physical address */
                         unsigned);                   /* Size of block    */


/* Get the physical address of a segment                                  */
/* Returns 0 if unsuccessful                                              */
extern _32bits far get_phys_addr(_32bits );           /*  Virtual address */

/* Get the physical address of a segment    (uses far call to devhlp)     */
/* Returns 0 if unsuccessful                                              */
extern _32bits far get_phys_addr1(_32bits );          /*  Virtual address */


/* Verify application access to a segment                                 */
/* Returns SUCCESS if OK                                                  */
/* Note that this function will cause the application to be stopped with  */
/* a TRAP 000D popup if the verivy fails.                                 */
extern word far verify_acc(_32bits ,             /* Virtual Address       */
                           word,                /* Size                   */
                           word);               /* Access type 0=RD,1=R/W */

extern _32bits far phys_to_gdt( _32bits, word, word);

/* Free an LDT created by PHYS_TO_LDT or PHYS_TO_LDT1                     */
extern int far free_virt(unsigned);                  /* Selector of block */


/* Allocate GDT slots for use by this DD.  Only valid at INIT time        */
extern word far get_gdt_slots(word,           /* number of slots          */
                              _32bits);       /* Array to store selectors */


/* Call the DevHlp function.                                               */
/* dev_help1 uses a far return                                             */
extern unsigned far dev_help ( union cpu_regs *, /* Input register values  */
                           union cpu_regs *);    /* Output register values */
extern unsigned far dev_help32( union cpu_regs32 *, /* Input reg values    */
                           union cpu_regs32 *);     /* Output reg values   */

extern unsigned far dev_help1(union cpu_regs *, /* Input register values  */
                          union cpu_regs *);    /* Output register values */

/* Signal an internal error */
extern void far internal_error(word);           /* Message number         */

/* Request Packet processing */
extern _32bits far alloc_req(void);
extern void far free_req(_32bits);
extern void far dev_done(reqhdr_type *,         /* Request Packet         */
                         word);                 /* Status To give         */

/* Lock/UnLock functions */
extern word far unlock(unsigned long);
extern unsigned long far lock(word);

/* SCSI DD interface functions */
extern word far scsi_init(void);
extern void far open_init(void);
extern word far call_scsi(reqhdr_type *);
extern void far call_idc(void far *,            /* IDC entry point        */
                         void far *,            /* Request packet pointer */
                         word);                 /* Target DD's DS         */

extern word delete_devh(word , word);
extern word set_new_devh(word , word);
extern word get_avail_devh(word);
extern word free_dhand(word);
