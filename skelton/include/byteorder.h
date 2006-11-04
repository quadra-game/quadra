#include "SDL.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

#define UNALIGNEDDWORD(x) (int)( \
   (((char *)(&(x)))[0] << 24) | \
   (((char *)(&(x)))[1] << 16) | \
   (((char *)(&(x)))[2] << 8) | \
   (((char *)(&(x)))[3] << 0) )

#define UNALIGNEDWORD(x) (int)( \
   (((char *)(&(x)))[0] << 8) | \
   (((char *)(&(x)))[1] << 0) )

#elif SDL_BYTEORDER == SDL_LIL_ENDIAN

/* assume intel -- we can read/write bytes anywhere */
#define UNALIGNEDDWORD(x) (x)
#define UNALIGNEDWORD(x)  (x)

#else

#error defined unaligned int access for your platform

#endif
