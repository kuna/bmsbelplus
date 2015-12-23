#ifndef BMSBEL_REGIST_ARRAY_H_
#define BMSBEL_REGIST_ARRAY_H_

#include <string>
#include <map>
#include <utility>

#include "bms_define.h"
#include "bms_word.h"

class BmsRegistArraySet;

// -- BmsRegistArray -----------------------------------------------------
class BmsRegistArray {
  friend class BmsRegistArraySet;

public:
  // str = channel(BmsWord)
  // `#---@@ value` is converted into:  [--- => name] [@@ => 00-ZZ] = [value]
  static bool CheckConstruction( const std::wstring& name, const std::wstring& str );

private:
  explicit BmsRegistArray( const std::wstring& name );

public:
  ~BmsRegistArray();

  const std::wstring& GetName( void ) const;

  unsigned int GetExistCount( void ) const;

  bool IsExists( const BmsWord &pos ) const;
  bool IsNotExists( const BmsWord &pos ) const;

  // 参照のみ
  const std::wstring& At( const BmsWord &pos ) const;
  const std::wstring& operator []( const BmsWord &pos ) const;

  void Set( const BmsWord &pos, const std::wstring& value );

  void DeleteAt( BmsWord pos );

  void Clear( void );

  std::wstring ToString( void ) const;

private:
  const std::wstring             name_;
  std::pair<bool, std::wstring>* array_;
};

// -- BmsRegistArraySet --------------------------------------------------
class BmsRegistArraySet {
public:
  explicit BmsRegistArraySet( void );
  ~BmsRegistArraySet();

  BmsRegistArray& MakeNewArray( const std::wstring& key );
  void DeleteArray( const std::wstring& key );

  bool Exists( const std::wstring& key ) const;
  BmsRegistArray& operator []( const std::wstring& key );
  const BmsRegistArray& operator []( const std::wstring& key ) const;

  void Clear( void );

  typedef std::map<std::wstring, BmsRegistArray*>::iterator Iterator;
  typedef std::map<std::wstring, BmsRegistArray*>::const_iterator ConstIterator;
  Iterator Begin();
  Iterator End();
  ConstIterator Begin() const;
  ConstIterator End() const;

private:
  std::map<std::wstring, BmsRegistArray*> table_;
};

#endif // BMSBEL_REGIST_ARRAY_H_
