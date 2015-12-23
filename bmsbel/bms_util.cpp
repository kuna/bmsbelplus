#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>

#include <algorithm>

#include "bmsbel\bms_util.h"

#include "bmsbel\bms_define.h"

namespace {
  const char HEX36_TABLE[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
}

char
BmsUtil::CharacterToNumber( char c )
{
  switch ( c ) {
  case '0':           return 0;
  case '1':           return 1;
  case '2':           return 2;
  case '3':           return 3;
  case '4':           return 4;
  case '5':           return 5;
  case '6':           return 6;
  case '7':           return 7;
  case '8':           return 8;
  case '9':           return 9;
  case 'a': case 'A': return 10;
  case 'b': case 'B': return 11;
  case 'c': case 'C': return 12;
  case 'd': case 'D': return 13;
  case 'e': case 'E': return 14;
  case 'f': case 'F': return 15;
  case 'g': case 'G': return 16;
  case 'h': case 'H': return 17;
  case 'i': case 'I': return 18;
  case 'j': case 'J': return 19;
  case 'k': case 'K': return 20;
  case 'l': case 'L': return 21;
  case 'm': case 'M': return 22;
  case 'n': case 'N': return 23;
  case 'o': case 'O': return 24;
  case 'p': case 'P': return 25;
  case 'q': case 'Q': return 26;
  case 'r': case 'R': return 27;
  case 's': case 'S': return 28;
  case 't': case 'T': return 29;
  case 'u': case 'U': return 30;
  case 'v': case 'V': return 31;
  case 'w': case 'W': return 32;
  case 'x': case 'X': return 33;
  case 'y': case 'Y': return 34;
  case 'z': case 'Z': return 35;
  default:            return -1;
  }
}


bool
BmsUtil::IsDigit( char c )
{
  char tmp = BmsUtil::CharacterToNumber( c );
  return tmp >= 0 && tmp <= 9;
}

bool
BmsUtil::IsNotDigit( char c )
{
  return NOT( BmsUtil::IsDigit( c ) );
}

bool
BmsUtil::IsAlphabet( char c )
{
  char tmp = BmsUtil::CharacterToNumber( c );
  return tmp >= 10 && tmp <= 35;
}

bool
BmsUtil::IsNotAlphabet( char c )
{
  return NOT( BmsUtil::IsAlphabet( c ) );
}

bool
BmsUtil::IsHex36( char c )
{
  return BmsUtil::CharacterToNumber( c ) >= 0;
}

bool
BmsUtil::IsNotHex36( char c )
{
  return NOT( BmsUtil::IsHex36( c ) );
}



char
BmsUtil::IntegerToHex36( int x )
{
  return HEX36_TABLE[x % BmsConst::HEX36_MAX_COUNT];
}


int
BmsUtil::GCD( int x, int y )
{
  if ( x == 0 || y == 0 ) { return 0; }
  x = x > 0 ? x : -x;
  y = y > 0 ? y : -y;
  while ( x != y ) {
    if ( x > y ) { x -= y; }
    else         { y -= x; }
  }
  return x;
}

int
BmsUtil::LCM( int x, int y )
{
  if ( x == 0 || y == 0 ) { return 0; }
  return x * y / BmsUtil::GCD( x, y );
}


unsigned int
BmsUtil::GetDenominator( double x, unsigned int max )
{
  const double epsilon = 0.0000000001;
  if ( x < 0.0 ) {
    x = fabs( x );
  }
  if ( x > 1.0 ) {
    x -= floor( x );
  }
  if ( x < epsilon ) {
    return 1;
  }

  for ( unsigned int i = 1; i < max; ++i ) {
    if ( fabs( x * i - floor( x * i + 0.5 ) ) < epsilon ) {
      return i;
    }
  }
  return max;
}


bool
BmsUtil::StringToInteger( const std::string& str, int* ret, unsigned int base )
{
  char* endptr;
  errno = 0;
  *ret = strtol( str.c_str(), &endptr, base );
  return *endptr == '\0' && errno == 0;
}

bool
BmsUtil::StringToFloat( const std::string& str, double* ret )
{
  char* endptr;
  errno = 0;
  *ret = strtod( str.c_str(), &endptr );
  return *endptr == '\0' && errno == 0;
}

void
BmsUtil::StringToUpper( std::string& str )
{
  std::transform( str.begin(), str.end(), str.begin(), toupper ); 
}

bool
BmsUtil::StringToInteger(const std::wstring& str, int* ret, unsigned int base)
{
	wchar_t* endptr;
	errno = 0;
	*ret = wcstol(str.c_str(), &endptr, base);
	return *endptr == L'\0' && errno == 0;
}

bool
BmsUtil::StringToFloat(const std::wstring& str, double* ret)
{
	wchar_t* endptr;
	errno = 0;
	*ret = wcstod(str.c_str(), &endptr);
	return *endptr == L'\0' && errno == 0;
}

void
BmsUtil::StringToUpper(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), toupper);
}
