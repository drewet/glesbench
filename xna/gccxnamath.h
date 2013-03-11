#ifndef __GCCXNAMATH_H__
#define __GCCXNAMATH_H__

#include <stdint.h>

// WinDef.h

typedef char CHAR;
typedef unsigned char BYTE;
typedef int16_t SHORT;
typedef uint16_t USHORT;
typedef int32_t INT;
typedef uint32_t UINT;
typedef int64_t INT64;
typedef uint64_t UINT64;
typedef float FLOAT;
typedef int BOOL;

#define TRUE 1
#define FALSE 0

#define __forceinline inline
#define CONST const
#define VOID void

#define _X86_
#define _XM_NO_INTRINSICS_

// SAL dummy defs

#define _In_
#define _In_z_
#define _In_count_c_(n)
#define _In_bytecount_x_(n)

#define _Out_
#define _Out_cap_c_(n)
#define _Out_bytecap_x_(n)

#include <stdio.h>

inline
void
__debugbreak()
{
    // TODO
}

inline
void
OutputDebugStringA(
    const CHAR *pMsg)
{
    puts(pMsg);
}

#include "xnamath.h"

#endif // __GCCXNAMATH_H__
