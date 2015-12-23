#include "bmsbel\bms_define.h"
#include "bmsbel\bms_exception.h"

#include "bmsbel\bms_header_table.h"


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
BmsHeaderTable::IsExists(const std::wstring& key) const
{
	return table_.find(key) != table_.end();
}


bool
BmsHeaderTable::IsNotExists(const std::wstring& key) const
{
	return NOT(this->IsExists(key));
}


const std::wstring&
BmsHeaderTable::operator [](const std::wstring& key)
{
	if (NOT(this->IsExists(key))) {
		throw BmsOutOfRangeAccessException(typeid(*this));;
	}
	return table_[key];
}

void
BmsHeaderTable::Set(const std::wstring& key, const std::wstring& value)
{
	table_[key] = value;
}

void
BmsHeaderTable::DeleteByKey(const std::wstring& key)
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
static void introduceHeaderIter(std::wstring& out, std::map<std::wstring, std::wstring>::iterator it) {
	out.append(L"#");
	out.append(it->first);
	out.append(L" ");
	out.append(it->second);
	out.append(L"\n");
}

static void
one_work(std::wstring& tmp, std::map<std::wstring, std::wstring>& table_clone, const wchar_t* key)
{
	std::map<std::wstring, std::wstring>::iterator it = table_clone.find(key);
	if (it != table_clone.end()) {
		introduceHeaderIter(tmp, it);
		table_clone.erase(it);
	}
}

std::wstring
BmsHeaderTable::ToString(void) const
{
	std::wstring tmp;
	std::map<std::wstring, std::wstring> table_clone = table_;

	one_work(tmp, table_clone, L"TITLE");
	one_work(tmp, table_clone, L"GENRE");
	one_work(tmp, table_clone, L"ARTIST");

	for (std::map<std::wstring, std::wstring>::iterator it = table_clone.begin(); it != table_clone.end(); ++it) {
		introduceHeaderIter(tmp, it);
	}
	return tmp;
}


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
