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
	static bool CheckConstruction(const std::string& name, const std::string& str);

private:
	explicit BmsRegistArray(const std::string& name);

public:
	~BmsRegistArray();

	const std::string& GetName(void) const;

	unsigned int GetExistCount(void) const;

	bool IsExists(const BmsWord &pos) const;
	bool IsNotExists(const BmsWord &pos) const;

	// const
	const std::string& At(const BmsWord &pos) const;
#ifdef USE_MBCS
	std::wstring At_w(const BmsWord &pos) const;
#endif
	const std::string& operator [](const BmsWord &pos) const;

	void Set(const BmsWord &pos, const std::string& value);

	void DeleteAt(BmsWord pos);

	void Clear(void);

	std::string ToString() const;
#ifdef USE_MBCS
	std::wstring ToWString() const;
#endif

private:
	const std::string             name_;
	std::pair<bool, std::string>* array_;
};

// -- BmsRegistArraySet --------------------------------------------------
class BmsRegistArraySet {
public:
	explicit BmsRegistArraySet(void);
	~BmsRegistArraySet();

	BmsRegistArray& MakeNewArray(const std::string& key);
	void DeleteArray(const std::string& key);

	bool Exists(const std::string& key) const;
	BmsRegistArray& operator [](const std::string& key);
	const BmsRegistArray& operator [](const std::string& key) const;

	void Clear(void);

	typedef std::map<std::string, BmsRegistArray*>::iterator Iterator;
	typedef std::map<std::string, BmsRegistArray*>::const_iterator ConstIterator;
	Iterator Begin();
	Iterator End();
	ConstIterator Begin() const;
	ConstIterator End() const;

private:
	std::map<std::string, BmsRegistArray*> table_;
};

#endif // BMSBEL_REGIST_ARRAY_H_
