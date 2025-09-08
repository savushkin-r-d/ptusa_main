/*** BeginHeader */
#ifndef GLOBTYPE_H
#define GLOBTYPE_H
// ---------------------------------------------------------------------------
//
// Datentypen
// ----------
//
//
// Version v1.0
//
// Autor: Dipl.-Ing. (FH) Holger Busse
//
// (c) 2006 - 2018 KATHREIN Sachsen GmbH
// ---------------------------------------------------------------------------

// $Id: globtype.h,v 1.17 2018/01/30 11:03:13 hbusse Exp $

// ===========================================================================
// Includes

#ifdef __KERNEL__
	#include <linux/types.h>
#else
	#include <stdint.h>
#endif

// ===========================================================================
// allgemeine Datentypen

#define TBool uint8_t
#define TByte uint8_t
#define TsByte int8_t
#define TWord uint16_t
#define TsWord int16_t
#define TDWord uint32_t
#define TsDWord int32_t
#define TLWord uint64_t
#define TsLWord int64_t

// ===========================================================================
// Makros

#ifndef NOBOOL
	#ifndef false
		#define false 0x00
	#endif
	#ifndef true
		#define true 0xFF
	#endif
#endif

// ===========================================================================

#endif

/*** EndHeader */
