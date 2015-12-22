#include <time.h>

#include "tt_path.h"
#include "tt_temporaly_array.h"

#include "bmx2wav_template_path_converter.h"

using namespace Bmx2Wav;

namespace {
  void remove_can_not_use_char_as_file_path_from( std::string& str ) {
    std::string::iterator it = str.begin();
    if ( NOT( TtPath::IsRelative( str ) ) ) {
      ++it;
      ++it;
    }
    while ( it != str.end() ) {
      switch ( *it ) {
      case '\/':
      case '\:':
      case '\*':
      case '\?':
      case '\"':
      case '\<':
      case '\>':
      case '\|':
        it = str.erase( it );
        continue;
      }
      ++it;
    }
  }
}


TemplatePathConverter::TemplatePathConverter( const std::string& input_filename,
                                              const std::string& output_filename,
                                              BmsBms& bms ) :
input_filename_( input_filename ),
output_filename_( output_filename ),
bms_( bms )
{
}

void
TemplatePathConverter::SetRemoveCanNotUseCharAsFilePath( bool flag )
{
  remove_can_not_use_char_as_file_path_ = flag;
}

bool
TemplatePathConverter::GetRemoveCanNotUseCharAsFilePath( void ) const
{
  return remove_can_not_use_char_as_file_path_;
}


std::string
TemplatePathConverter::Convert( const std::string& source )
{
  const char* p = source.c_str();
  std::string result;
  while ( *p != '\0' ) {
    switch ( *p ) {
    case '%': {
      ++p;
      switch ( *p ) {
      default:
        result.append( 1, '%' );
        result.append( 1, *p );
        ++p;
        break;

      case '\0':
        result.append( 1, '%' );
        continue;

      case '%':
        result.append( 1, '%' );
        ++p;
        break;

        // output_filename_
      case 'O':
        result.append( output_filename_ );
        ++p;
        break;

      case 'P':
        result.append( TtPath::EditPath( output_filename_, false, false, true, false ) );
        ++p;
        break;

      case 'Q':
        result.append( TtPath::DirName( output_filename_ ) );
        ++p;
        break;

      case 'R':
        result.append( TtPath::BaseName( TtPath::DirName( output_filename_ ) ) );
        ++p;
        break;

        // input_filename_
      case 'I':
        result.append( input_filename_ );
        ++p;
        break;

      case 'J':
        result.append( TtPath::EditPath( input_filename_, false, false, true, false ) );
        ++p;
        break;

      case 'K':
        result.append( TtPath::DirName( input_filename_ ) );
        ++p;
        break;

      case 'L':
        result.append( TtPath::BaseName( TtPath::DirName( input_filename_ ) ) );
        ++p;
        break;

      case '#':
        ++p;
        this->ConvertBmsHeader( result, p );
        break;

      case '=':
        ++p;
        this->ConvertStringFormatTime( result, p );
        break;
      }
    }
      break;

    default:
      result.append( 1, *p );
      ++p;
      break;
    }
  }
  return result;
}

void
TemplatePathConverter::ConvertBmsHeader( std::string& result, const char*& p )
{
  std::string header;
  while ( *p != '#' ) {
    if ( *p == '\0' ) {
      result.append( "%#" );
      result.append( header );
      return;
    }
    header.append( 1, *p );
    ++p;
  }
  ++p;
  if ( bms_.GetHeaders().IsExists( header ) ) {
    std::string tmp( bms_.GetHeaders()[header] );
    if ( remove_can_not_use_char_as_file_path_ ) {
      remove_can_not_use_char_as_file_path_from( tmp );
    }
    result.append( tmp );
  }
}


void
TemplatePathConverter::ConvertStringFormatTime( std::string& result, const char*& p )
{
  std::string format;
  while ( *p != '=' ) {
    if ( *p == '\0' ) {
      result.append( "%=" );
      result.append( format );
      return;
    }
    format.append( 1, *p );
    ++p;
  }
  ++p;
  TtTemporalyString buf( format.size() + 1024 );
  time_t t = time( NULL );
  struct tm *tmp_tm = localtime( &t );
  strftime( buf.GetPtr(), buf.GetSize(), format.c_str(), tmp_tm );
  std::string tmp_str( buf.GetPtr() );
  if ( remove_can_not_use_char_as_file_path_ ) {
    remove_can_not_use_char_as_file_path_from( tmp_str );
  }
  result.append( tmp_str );
}
