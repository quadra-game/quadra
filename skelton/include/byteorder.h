
#include <endian.h>
#include <byteswap.h>

#if (BYTE_ORDER == BIG_ENDIAN)
#define INTELWORD(x) bswap_16(x)
#define INTELDWORD(x) bswap_32(x)
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
