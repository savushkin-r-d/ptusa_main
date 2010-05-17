#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <asm/types.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include "kbusapi.h"

int iFD;

//static void modifyPABforEffect( void )
//{
//    pstPabOUT->us.Pab[0] = pstPabOUT->us.Pab[0] << 1;
//    if( pstPabOUT->us.Pab[0] > 0xFFF ) pstPabOUT->us.Pab[0] = 1;
//    return;
//}

// -------------------------------------------------------------------------------
/// Oeffnet einen Kanal zum Kbus-Treiber und initialisert die Bibliothek.
/// \retval 0 Kbus-Schnittstelle bereit
/// \retval EACCES Kein Zugriff auf /dev/kbus moeglich
/// \retval ENODEV /dev/kbus existiert nicht auf dem Dateisystem
/// \retval ENOMEM nicht genug Speicher verfuegbar
/// \retval EMFILE zuviele Dateien auf dem System bereits geoeffnet
/// \retval EIO low-level IO Fehler
// -------------------------------------------------------------------------------

int KbusOpen( )
    {
    iFD = open( "/dev/kbus", O_WRONLY );

    if( iFD < 1 )
        {
        printf( "KBUSAPI: Failed opening fifo for writing: %s", strerror( errno ) );
        return errno;
        }

    return 0;
    }

// -------------------------------------------------------------------------------
/// Aktualisiert Prozessdaten und Prozessabbild. Kann nur nach KbusOpen()
/// aufgerufen werden.
/// \retval 0 Aktualisierungsnachricht erfolgreich gesendet
/// \retval EINVAL Kanal zum Kbus wurde noch nicht oder fehlerhaft geoeffnet
// -------------------------------------------------------------------------------

int KbusUpdate( )
    {
    int iBytes = 0;
    int iTmp;

    if( iFD < 1 ) return -EINVAL;

    iBytes = ioctl( iFD, IOCTL_KBUSUPDATE, &iTmp );
    if( 0 >= iBytes )
        return -EINVAL;
    return 0;
    }

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

int KbusGetBinaryInputOffset( )
    {
    int iBytes = 0;
    int iInputOffset = 0;

    if( iFD < 1 ) return -EINVAL;

    iBytes = ioctl( iFD, IOCTL_GETBININPUTOFFSET, &iInputOffset );
    if( 0 >= iBytes )
        return -EINVAL;
    return iInputOffset;
    }

// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------

int KbusGetBinaryOutputOffset( )
    {
    int iBytes = 0;
    int iOutputOffset = 0;

    if( iFD < 1 ) return -EINVAL;

    iBytes = ioctl( iFD, IOCTL_GETBINOUTPUTOFFSET, &iOutputOffset );
    if( 0 >= iBytes )
        return -EINVAL;
    return iOutputOffset;
    }

// -------------------------------------------------------------------------------
/// Schliesst den Kanal zum Kbus und gibt allozierte Resourcen wieder frei.
/// \retval 0 Kanal geschlossen
/// \retval EINVAL Kbus-Kanal war nicht (erfolgreich) geoeffnet
// -------------------------------------------------------------------------------

int KbusClose( )
    {
    /* Close /dev/kbus */
    close( iFD );
    return 0;
    }
