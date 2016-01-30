#ifndef BMSBEL_DEFINE_H_
#define BMSBEL_DEFINE_H_

// must define it to use lib file
#define USING_STATIC_LIBICONV	1

#include <limits>

#ifndef NOT_USE_VAR
#define NOT_USE_VAR( var ) ((void)var)
#endif

#ifndef NOT
#define NOT( v ) (!(v))
#endif

#ifdef _WIN32
#define USE_MBCS
#endif

namespace BmsConst {
	// Now this is the default bar division count.
	// COMMENT: INT_MAX = 2147483647, RES_MAX should be under 2147483.
	//          Primes should be consisted of 2 and 5.
	//          (NOTE: 15360 is perfect for L999999999^999999999)
	//          Bar may can change it's resolution in some very hard case, so be careful - it's not fixed value.
	const int BAR_DEFAULT_RESOLUTION = 10240;

	const int HEX36_MIN_VALUE = 0;
	const int HEX36_MAX_VALUE = 35;
	const int HEX36_MAX_COUNT = 36;

	const int WORD_MIN_VALUE = 0;
	const int WORD_MAX_VALUE = 1295;
	const int WORD_MAX_COUNT = 1296;

	const int BAR_MAX_VALUE = 999;
	const int BAR_MAX_COUNT = 1000;

	const int BMS_MAX_KEY = 18;

	const int BMS_MAX_LINE_BUFFER = 10240;
	const double BMS_BASIC_BPM = 120.0;
}

namespace BmsBelOption {
	// detailed option during parsing
	const int CONVERT_ATTEMPT_LINES = 10;
	const char DEFAULT_FALLBACK_ENCODING[] = "Shift_JIS";
	const char DEFAULT_UNICODE_ENCODING[] = "UTF-8";
}

#ifdef _DEBUG
#  include <stdio.h>
#  define PCD()            printf( "%s(%d) gone.\n", __FILE__, __LINE__ );
#  define PCA( type, obj ) printf( "%s(%d) : %s = " type "\n", __FILE__, __LINE__, #obj, obj )
#  define PCC( c )         PCA( "%c", c )
#  define PCI( i )         PCA( "%d", i )
#  define PCP( p )         PCA( "%p", p )
#  define PCS( s )         PCA( "%s", s )
#  define PCF( f )         PCA( "%f", f )
#endif

// ITERATOR
/** @brief iterate buffer until target bar */
#define FOR_BUFFER_AVAILABLE(buffer, bar)\
	for (BmsBuffer::Iterator iter = buffer.Current(); \
		iter != buffer.End() && iter->first <= bar; ++iter)
/** @brief iterate buffer until target bar, updating original buffer */
#define FOR_BUFFER_AVAILABLE_UPDATE(buffer, bar)\
	for (; buffer.Current() != buffer.End() && buffer.Current()->first <= bar; buffer.Next())

//
// you can use BMSBEL BAR as LARGEINT(__int64)
// - that may drop performance in game
// - this option may gain more precision.
// use this option at your own risk.
//

#define BMSBEL_BAR_LARGEINT_

#ifdef BMSBEL_BAR_LARGEINT_
typedef __int64			barindex;
#else
typedef int				barindex;
#endif

typedef unsigned int	measureindex;

#endif // BMSBEL_DEFINE_H_
