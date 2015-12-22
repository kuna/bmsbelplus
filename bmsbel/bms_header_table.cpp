#include "bmsbel\bms_define.h"
#include "bmsbel\bms_exception.h"

#include "bmsbel\bms_header_table.h"


BmsHeaderTable::BmsHeaderTable( void ) :
table_()
{
}

BmsHeaderTable::~BmsHeaderTable()
{
}

unsigned int
BmsHeaderTable::GetCount( void ) const
{
  return table_.size();
}

bool
BmsHeaderTable::IsExists( const std::string& key ) const
{
  return table_.find( key ) != table_.end();
}


bool
BmsHeaderTable::IsNotExists( const std::string& key ) const
{
  return NOT( this->IsExists( key ) );
}


const std::string&
BmsHeaderTable::operator []( const std::string& key )
{
  if ( NOT( this->IsExists( key ) ) ) {
    throw BmsOutOfRangeAccessException( typeid( *this ) );;
  }
  return table_[key];
}

void
BmsHeaderTable::Set( const std::string& key, const std::string& value )
{
  table_[key] = value;
}

void
BmsHeaderTable::DeleteByKey( const std::string& key )
{
  table_.erase( key );
}

void
BmsHeaderTable::Clear( void )
{
  table_.clear();
}


static void
one_work( std::string& tmp, std::map<std::string, std::string>& table_clone, const char* key )
{
  std::map<std::string, std::string>::iterator it = table_clone.find( key );
  if ( it != table_clone.end() ) {
    tmp.append( "#" );
    tmp.append( it->first );
    tmp.append( " " );
    tmp.append( it->second );
    tmp.append( "\n" );
    table_clone.erase( it );
  }
}

std::string
BmsHeaderTable::ToString( void ) const
{
  std::string tmp;
  std::map<std::string, std::string> table_clone = table_;

  one_work( tmp, table_clone, "TITLE" );
  one_work( tmp, table_clone, "GENRE" );
  one_work( tmp, table_clone, "ARTIST" );

  for ( std::map<std::string, std::string>::iterator it = table_clone.begin(); it != table_clone.end(); ++it ) {
    tmp.append( "#" );
    tmp.append( it->first );
    tmp.append( " " );
    tmp.append( it->second );
    tmp.append( "\n" );
  }
  return tmp;
}


BmsHeaderTable::Iterator
BmsHeaderTable::Begin( void ) const
{
  return table_.begin();
}

BmsHeaderTable::Iterator
BmsHeaderTable::End( void ) const
{
  return table_.end();
}
