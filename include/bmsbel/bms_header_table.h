#ifndef BMSBEL_HEADER_TABLE_H_
#define BMSBEL_HEADER_TABLE_H_

#include <string>
#include <map>

// -- BmsHeaderTable -----------------------------------------------------
class BmsHeaderTable {
public:
  explicit BmsHeaderTable( void );

  ~BmsHeaderTable();

  unsigned int GetCount( void ) const;

  bool IsExists( const std::string& key ) const;
  bool IsNotExists( const std::string& key ) const;

  const std::string& operator []( const std::string& key );

  void Set( const std::string& key, const std::string& value );
  void DeleteByKey( const std::string& key );

  void Clear( void );

  std::string ToString( void ) const;

  // -- Iterator ---------------------------------------------------------
  typedef std::map<std::string, std::string>::const_iterator Iterator;

  Iterator Begin( void ) const;
  Iterator End( void ) const;

private:
  std::map<std::string, std::string> table_;
};

#endif // BMSBEL_HEADER_TABLE_H_
