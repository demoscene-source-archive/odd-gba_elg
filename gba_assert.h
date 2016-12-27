#ifndef _gba_assert_h_
#define _gba_assert_h_

#include <mappy.h>
#include <stdlib.h>

#ifdef assert
#undef assert
#endif

#ifdef NDEBUG
#define assert(p)  	((void)0)
#else
#define assert(e) if (!(e)) { \
	dprintf("assert (file: %s, line: %i): %s\n", __FILE__, __LINE__, #e); \
	exit(1); \
}
#endif

//---------------------------------------------------------------------------------
#endif // _gba_assert_h_
//---------------------------------------------------------------------------------
