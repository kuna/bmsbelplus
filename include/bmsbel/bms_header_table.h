#ifndef BMSBEL_HEADER_TABLE_H_
#define BMSBEL_HEADER_TABLE_H_

#include <string>
#include <map>

// -- BmsHeaderTable -----------------------------------------------------
class BmsHeader;
class BmsHeaderTable {
public:
	explicit BmsHeaderTable(void);

	~BmsHeaderTable();

	unsigned int GetCount(void) const;

	bool IsExists(const std::string& key) const;
	bool IsNotExists(const std::string& key) const;

	const BmsHeader& operator [](const std::string& key);

	void Set(const std::string& key, const std::string& value);
	void DeleteByKey(const std::string& key);

	void Clear(void);

	std::string ToString(void) const;
#ifdef USE_MBCS
	std::wstring ToWString(void) const;
#endif

	bool Query(const std::string& key, int* v);
	bool Query(const std::string& key, double* v);
	bool Query(const std::string& key, std::string& v);
	bool Query(const std::string& key, char* v);
#ifdef USE_MBCS
	bool Query(const std::string& key, std::wstring& v);
#endif

	// -- Iterator ---------------------------------------------------------
	typedef std::map<std::string, BmsHeader>::const_iterator Iterator;

	Iterator Begin(void) const;
	Iterator End(void) const;

private:
	std::map<std::string, BmsHeader> table_;
};

class BmsHeader {
public:
	BmsHeader();
	BmsHeader(const std::string& val);
	void Set(const std::string& val);
	std::string ToString() const;
	const char *c_str() const;
#ifdef USE_MBCS
	std::wstring ToWString() const;
#endif
	int ToInteger() const;
	double ToFloat() const;
	bool IsInteger() const;
	bool IsFloat() const;
private:
	std::string value;
};

#endif // BMSBEL_HEADER_TABLE_H_
