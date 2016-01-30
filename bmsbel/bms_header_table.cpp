#include "bmsbel/bms_define.h"
#include "bmsbel/bms_exception.h"

#include "bmsbel/bms_header_table.h"
#include "bmsbel/bms_util.h"

#include <string.h>

BmsHeaderTable::BmsHeaderTable(void) :
table_()
{
}

BmsHeaderTable::~BmsHeaderTable()
{
}

unsigned int
BmsHeaderTable::GetCount(void) const
{
	return table_.size();
}

bool
BmsHeaderTable::IsExists(const std::string& key) const
{
	return table_.find(key) != table_.end();
}


bool
BmsHeaderTable::IsNotExists(const std::string& key) const
{
	return NOT(this->IsExists(key));
}


const BmsHeader&
BmsHeaderTable::operator [](const std::string& key)
{
	if (NOT(this->IsExists(key))) {
		throw BmsOutOfRangeAccessException(typeid(*this));;
	}
	return table_[key];
}

void
BmsHeaderTable::Set(const std::string& key, const std::string& value)
{
	table_[key] = value;
}

void
BmsHeaderTable::DeleteByKey(const std::string& key)
{
	table_.erase(key);
}

void
BmsHeaderTable::Clear(void)
{
	table_.clear();
}

//
// this function make string
// from table if header[`key`] exists, and removed header[`key`].
// So, `key` values will be introduced first.
// used for BmsHeaderTable::ToString()
//
static void introduceHeaderIter(std::string& out, std::map<std::string, BmsHeader>::iterator it) {
	out.append("#");
	out.append(it->first);
	out.append(" ");
	out.append(it->second.ToString());
	out.append("\n");
}

static void
one_work(std::string& tmp, std::map<std::string, BmsHeader>& table_clone, const char* key)
{
	std::map<std::string, BmsHeader>::iterator it = table_clone.find(key);
	if (it != table_clone.end()) {
		introduceHeaderIter(tmp, it);
		table_clone.erase(it);
	}
}

std::string
BmsHeaderTable::ToString(void) const
{
	std::string tmp;
	std::map<std::string, BmsHeader> table_clone = table_;

	one_work(tmp, table_clone, "TITLE");
	one_work(tmp, table_clone, "GENRE");
	one_work(tmp, table_clone, "ARTIST");

	for (std::map<std::string, BmsHeader>::iterator it = table_clone.begin(); it != table_clone.end(); ++it) {
		introduceHeaderIter(tmp, it);
	}
	return tmp;
}

#ifdef USE_MBCS
std::wstring
BmsHeaderTable::ToWString(void) const
{
	std::string s = ToString();
	wchar_t *w = new wchar_t[10240];
	BmsUtil::utf8_to_wchar(s.c_str(), w, 10240);
	std::wstring _w = w;
	delete w;
	return _w;
}
#endif

BmsHeaderTable::Iterator
BmsHeaderTable::Begin(void) const
{
	return table_.begin();
}

BmsHeaderTable::Iterator
BmsHeaderTable::End(void) const
{
	return table_.end();
}

bool
BmsHeaderTable::Query(const std::string& key, int* v)
{
	if (IsExists(key)) {
		*v = this->operator[](key).ToInteger();
		return true;
	}
	return false;
}

bool
BmsHeaderTable::Query(const std::string& key, char* v)
{
	if (IsExists(key)) {
		strcpy(v, this->operator[](key).c_str());
		return true;
	}
	return false;
}

bool
BmsHeaderTable::Query(const std::string& key, std::string& v)
{
	if (IsExists(key)) {
		v = this->operator[](key).ToString();
		return true;
	}
	return false;
}

bool
BmsHeaderTable::Query(const std::string& key, double* v)
{
	if (IsExists(key)) {
		*v = this->operator[](key).ToFloat();
		return true;
	}
	return false;
}

#ifdef USE_MBCS
bool
BmsHeaderTable::Query(const std::string& key, std::wstring& v)
{
	if (IsExists(key)) {
		v = this->operator[](key).ToWString();
		return true;
	}
	return false;
}
#endif

// BmsHeader ---------------------------

BmsHeader::BmsHeader() {}

BmsHeader::BmsHeader(const std::string& val) : value(val) {}

void BmsHeader::Set(const std::string& val) { value = val; }
std::string BmsHeader::ToString() const { return value; }
const char *BmsHeader::c_str() const { return value.c_str(); }
#ifdef USE_MBCS
std::wstring BmsHeader::ToWString() const {
	wchar_t *_w = new wchar_t[10240];
	BmsUtil::utf8_to_wchar(c_str(), _w, 10240);
	std::wstring w = _w;
	delete _w;
	return w;
}
#endif
int BmsHeader::ToInteger() const {
	return atoi(value.c_str());
}
double BmsHeader::ToFloat() const {
	return atof(value.c_str());
}
bool BmsHeader::IsInteger() const {
	char *t[20];
	errno = 0;
	strtol(value.c_str(), t, 10);
	return errno == 0;
}
bool BmsHeader::IsFloat() const {
	char *t[20];
	errno = 0;
	strtod(value.c_str(), t);
	return errno == 0;
}
