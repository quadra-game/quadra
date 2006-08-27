#include "config.h"

#ifdef UGS_LINUX
#ifdef HAVE_MACHINE_ENDIAN_H
#include <machine/endian.h>
#else
#include <endian.h>
#endif
#ifdef HAVE_MACHINE_BYTE_ORDER_H
#include <machine/byte_order.h>
#else
#include <byteswap.h>
#endif
#elif defined(UGS_DIRECTX)
#define LITTLE_ENDIAN 4321
#define BIG_ENDIAN 1234
#define BYTE_ORDER LITTLE_ENDIAN
#else
#error Platform undefined
#endif

#if (BYTE_ORDER == BIG_ENDIAN)
#ifdef HAVE_MACHINE_BYTE_ORDER_H
#define INTELWORD(x) NXSwapShort(x)
#define INTELDWORD(x) NXSwapLong(x)
#else
#define INTELWORD(x) bswap_16(x)
#define INTELDWORD(x) bswap_32(x)
#endif
#elif (BYTE_ORDER == LITTLE_ENDIAN)
#define INTELWORD(x) (x)
#define INTELDWORD(x) (x)
#else
#error define byteswapping code for your architecture
#endif

#if (BYTE_ORDER == BIG_ENDIAN)

#define UNALIGNEDDWORD(x) (int)( \
   (((char *)(&(x)))[0] << 24) | \
   (((char *)(&(x)))[1] << 16) | \
   (((char *)(&(x)))[2] << 8) | \
   (((char *)(&(x)))[3] << 0) )

#define UNALIGNEDWORD(x) (int)( \
   (((char *)(&(x)))[0] << 8) | \
   (((char *)(&(x)))[1] << 0) )

#elif (BYTE_ORDER == LITTLE_ENDIAN) 
/* assume intel -- we can read/write bytes anywhere */
#define UNALIGNEDDWORD(x) (x)
#define UNALIGNEDWORD(x)  (x)
#else
#error defined unaligned int access for your platform
#endif
