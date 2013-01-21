/// @file kbusapi.h
/// @brief Setting the configuration of the Fieldbus-Controller in the Flash.
/// The Flash is reading from the Bootloader if it start the Linux-Kernel.
///
/// @version    0.02
/// @date       18-MAY-2006
/// @author     Florian Reckmann
///
/// @par History:
/// 01.12.2004  Init(LF)
///

#ifndef KBUS_API_H
#define KBUS_API_H

#include <asm/types.h>

#define IOCTL_KBUSUPDATE           1
#define IOCTL_GETBININPUTOFFSET    2
#define IOCTL_GETBINOUTPUTOFFSET   3
#define IOCTL_CHANGE_UPDATE_SPEED  4

typedef enum { FALSE, TRUE } BOOL;

#define PAB_SIZE 1020
typedef union
    {
    struct   /* Zugriff auf PAB als u16 */
        {
        __u16  Pab [PAB_SIZE];       /* Prozess-Abbild */

#if PFC
        __u16  Var [PFC_VAR_SIZE];  /* Variablen-Bereich, direkt hinter PAB */
#endif

        } us;

    struct   /* Zugriff auf PAB als u8 */
        {
        __u8 Pab [sizeof (__u16)*PAB_SIZE];     /* Prozess-Abbild */

#if PFC
        __u8 Var [sizeof (__u16)*PFC_VAR_SIZE]; /* Variablen-Bereich, direkt hinter PAB */
#endif

        } uc;

    }T_PabVarUnion;

#define PABIN (volatile T_PabVarUnion *) 0xFFE060;
#define PABOUT (volatile T_PabVarUnion *) (0xFFE060+sizeof(T_PabVarUnion));
static volatile T_PabVarUnion * const pstPabOUT = PABOUT
static volatile T_PabVarUnion * const pstPabIN = PABIN

extern int KbusOpen(void);
extern int KbusUpdate(void);
extern int KbusClose(void);
extern int KbusGetBinaryInputOffset(void);
extern int KbusGetBinaryOutputOffset(void);


#endif // KBUS_API_H
