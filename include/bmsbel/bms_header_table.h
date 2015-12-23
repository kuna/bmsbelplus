#ifndef BMSBEL_HEADER_TABLE_H_
#define BMSBEL_HEADER_TABLE_H_

#include <string>
#include <map>

// -- BmsHeaderTable -----------------------------------------------------
class BmsHeaderTable {
public:
	explicit BmsHeaderTable(void);

	~BmsHeaderTable();

	unsigned int GetCount(void) const;

	bool IsExists(const std::wstring& key) const;
	bool IsNotExists(const std::wstring& key) const;

	const std::wstring& operator [](const std::wstring& key);

	void Set(const std::wstring& key, const std::wstring& value);
	void DeleteByKey(const std::wstring& key);

	void Clear(void);

	std::wstring ToString(void) const;

	// -- Iterator ---------------------------------------------------------
	typedef std::map<std::wstring, std::wstring>::const_iterator Iterator;

	Iterator Begin(void) const;
	Iterator End(void) const;

private:
	std::map<std::wstring, std::wstring> table_;
};

#endif // BMSBEL_HEADER_TABLE_H_
