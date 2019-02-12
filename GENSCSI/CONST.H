/*****************************************************************************/
/* This file contains all internal constants used by the device driver       */
/* Constants exposed outside the DD are in INTERFAC.H                        */
/*****************************************************************************/

/* The largest command OS/2 can issue */
#define MAXCMD 27

/* Boolean values */
#define TRUE              1
#define FALSE             0
#define SUCCESS           0

/* Bit and return code values for the Request packet status field */
#define DONE 0x0100
#define ERROR 0x8000
#define DEV_ERR 0x4000
#define INV_CMD 0x0003
#define WRT_FAULT 0x000A
#define GEN_FAIL 0x000C
#define INIT_FAIL_STATUS 0x8100

/*****************************************************************************/
/*                                                                           */
/* Constants used by devhlp                                                  */
/*                                                                           */
/*****************************************************************************/
#define devhlp_SchedClockAddr           0x0000
#define devhlp_DevDone                  0x0001
#define devhlp_Yield                    0x0002
#define devhlp_TCYield                  0x0003
#define devhlp_Block                    0x0004
#define devhlp_Run                      0x0005
#define devhlp_SemRequest               0x0006
#define devhlp_SemClear                 0x0007
#define devhlp_SemHandle                0x0008
#define devhlp_PushReqPacket            0x0009
#define devhlp_PullReqPacket            0x000a
#define devhlp_PullParticular           0x000b
#define devhlp_SortReqPacket            0x000c
#define devhlp_AllocReqPacket           0x000d
#define devhlp_FreeReqPacket            0x000e
#define devhlp_QueueInit                0x000f
#define devhlp_QueueFlush               0x0010
#define devhlp_QueueWrite               0x0011
#define devhlp_QueueRead                0x0012
#define devhlp_Lock                     0x0013
#define devhlp_Unlock                   0x0014
#define devhlp_PhysToVirt               0x0015
#define devhlp_VirtToPhys               0x0016
#define devhlp_PhysToUVirt              0x0017
#define devhlp_AllocPhys                0x0018
#define devhlp_FreePhys                 0x0019
#define devhlp_SetROMVector             0x001a
#define devhlp_SetIRQ                   0x001b
#define devhlp_UnSetIRQ                 0x001c
#define devhlp_SetTimer                 0x001d
#define devhlp_ResetTimer               0x001e
#define devhlp_MonitorCreate            0x001f
#define devhlp_Register                 0x0020
#define devhlp_DeRegister               0x0021
#define devhlp_MonWrite                 0x0022
#define devhlp_MonFlush                 0x0023
#define devhlp_GetDOSVar                0x0024
#define devhlp_SendEvent                0x0025
#define devhlp_ROMCritSection           0x0026
#define devhlp_VerifyAccess             0x0027
#define devhlp_AttachDD                 0x002a
#define devhlp_Internal                 0x002a
#define devhlp_InternalError            0x002b
#define devhlp_AllocGDTSelector         0x002d
#define devhlp_PhysToGDTSelector        0x002e
#define devhlp_RealToProt               0x002f
#define devhlp_ProtToReal               0x0030
#define devhlp_EOI                      0x0031
#define devhlp_UnPhysToVirt             0x0032
#define devhlp_TickCount                0x0033
#define devhlp_GetLIDEntry              0x0034
#define devhlp_FreeLIDEntry             0x0035
#define devhlp_ABIOSCall                0x0036
#define devhlp_ABIOSCommonEntry         0x0037
#define devhlp_GetDeviceBlock           0x0038
#define devhlp_RegisterStackUseage      0x003A
#define devhlp_VideoPause               0x003C
#define devhlp_DispMsg                  0x003D
#define devhlp_RegisterPDD              0x0050
#define devhlp_RegisterBeep             0x0051
#define devhlp_Beep                     0x0052
#define devhlp_FreeGDTSelector          0x0053
#define devhlp_PhysToGDTSel             0x0054
#define devhlp_VMLock                   0x0055
#define devhlp_VMUnLock                 0x0056
#define devhlp_VMAlloc                  0x0057
#define devhlp_VMFree                   0x0058
#define devhlp_VMProcessToGlobal        0x0059
#define devhlp_VMGlobalToProcess        0x005A
#define devhlp_VirtToLin                0x005B
#define devhlp_LinToGDTSelector         0x005C
#define devhlp_GetDescInfo              0x005D
#define devhlp_LinToPageList            0x005E
#define devhlp_PageListToLin            0x005F
#define devhlp_PageListToGDT            0x0060
#define devhlp_RegisterTmrDD            0x0061
#define devhlp_AllocateCtxHook          0x0063
#define devhlp_FreeCtxHook              0x0064
#define devhlp_ArmCtxHook               0x0065
#define devhlp_VMSetMem                 0x0066
#define devhlp_OpenEventSem             0x0067
#define devhlp_CloseEventSem            0x0068
#define devhlp_PostEventSem             0x0069
#define devhlp_ResetEventSem            0x006A
#define devhlp_DynamicAPI               0x006c

