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

namespace BmsConst {
  // 分解能
  const int DEFAULT_BAR_DIVISION_COUNT = 1;
  const int BAR_DIVISION_COUNT_MAX = 13440;

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
}

namespace BmsBelOption {
	// detailed option during parsing
	const int ERROR_ON_DUPLICATED_HEADER	= 0;
	const int ERROR_ON_UNKNOWN_CHANNEL		= 0;
	const int CONVERT_ATTEMPT_LINES			= 10;
	const char DEFAULT_FALLBACK_ENCODING[]	= "Shift_JIS";
	const char DEFAULT_UNICODE_ENCODING[]	= "UTF-16LE";	// WCHAR_T may be correct in other OS.
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

#endif // BMSBEL_DEFINE_H_
