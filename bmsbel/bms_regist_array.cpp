#include "bmsbel/bms_regist_array.h"
#include "bmsbel/bms_define.h"
#include "bmsbel/bms_util.h"
#include "bmsbel/bms_exception.h"

bool
BmsRegistArray::CheckConstruction(const std::string& name, const std::string& str)
{
	if (str.length() != name.length() + 2) {
		return false;
	}
	std::string tmp = str.substr(0, name.length());
	BmsUtil::StringToUpper(tmp);
	if (tmp.compare(name) != 0) {
		return false;
	}
	if (NOT(BmsWord::CheckConstruction(str.c_str() + name.length()))) {
		return false;
	}

	switch (str.c_str()[name.length() + 2]) {
	case ' ':
	case '\t':
	case '\0':
		return true;

	default:
		return false;
	}
}



BmsRegistArray::BmsRegistArray(const std::string& name) :
name_(name),
array_(new std::pair<bool, std::string>[BmsConst::WORD_MAX_COUNT])
{
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		array_[i].first = false;
	}
}


BmsRegistArray::~BmsRegistArray()
{
	delete[] array_;
}


const std::string&
BmsRegistArray::GetName(void) const
{
	return name_;
}

unsigned int
BmsRegistArray::GetExistCount(void) const
{
	unsigned int count = 0;
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		if (array_[i].first) {
			++count;
		}
	}
	return count;
}



bool
BmsRegistArray::IsExists(const BmsWord &pos) const
{
	return array_[pos.ToInteger()].first;
}

bool
BmsRegistArray::IsNotExists(const BmsWord &pos) const
{
	return NOT(this->IsExists(pos));
}


#ifdef USE_MBCS
std::wstring
BmsRegistArray::At_w(const BmsWord &pos) const
{
	if (this->IsNotExists(pos)) {
		throw BmsOutOfRangeAccessException(typeid(*this));
	}
	wchar_t *_w = new wchar_t[10240];
	BmsUtil::utf8_to_wchar(array_[pos.ToInteger()].second.c_str(), _w, 10240);
	std::wstring w = _w;
	delete _w;
	return w;
}
#endif

const std::string&
BmsRegistArray::At(const BmsWord &pos) const
{
	if (this->IsNotExists(pos)) {
		throw BmsOutOfRangeAccessException(typeid(*this));
	}
	return array_[pos.ToInteger()].second;
}

const std::string&
BmsRegistArray::operator [](const BmsWord &pos) const
{
	return this->At(pos);
}


void
BmsRegistArray::Set(const BmsWord &pos, const std::string& value)
{
	pos.CheckValid();
	array_[pos.ToInteger()].first = true;
	array_[pos.ToInteger()].second = value;
}


void
BmsRegistArray::DeleteAt(BmsWord pos)
{
	pos.CheckValid();
	array_[pos.ToInteger()].first = false;
	array_[pos.ToInteger()].second = "";
}


void
BmsRegistArray::Clear(void)
{
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		array_[i].first = false;
		array_[i].second = "";
	}
}

std::string
BmsRegistArray::ToString(void) const
{
	std::string tmp;
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		if (array_[i].first) {
			tmp.append("#");
			tmp.append(name_);
			tmp.append(BmsWord(i).ToString());
			tmp.append(" ");
			tmp.append(array_[i].second);
			tmp.append("\n");
		}
	}
	return tmp;
}

#ifdef USE_MBCS
std::wstring
BmsRegistArray::ToWString(void) const
{
	wchar_t wname_[1024];
	BmsUtil::utf8_to_wchar(name_.c_str(), wname_, 1024);
	std::wstring tmp;
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		if (array_[i].first) {
			tmp.append(L"#");
			tmp.append(wname_);
			tmp.append(BmsWord(i).ToWString());
			tmp.append(L" ");
			tmp.append(At_w(i));
			tmp.append(L"\n");
		}
	}
	return tmp;
}
#endif


// -- BmsRegistArraySet --------------------------------------------------
BmsRegistArraySet::BmsRegistArraySet(void) :
table_()
{
}

BmsRegistArraySet::~BmsRegistArraySet()
{
	for (std::map<std::string, BmsRegistArray*>::iterator it = table_.begin(); it != table_.end(); ++it) {
		delete it->second;
	}
}

BmsRegistArray&
BmsRegistArraySet::MakeNewArray(const std::string& key)
{
	if (this->Exists(key)) {
		return *table_[key];
	}
	BmsRegistArray* tmp = new BmsRegistArray(key);
	table_[key] = tmp;
	return *tmp;
}

void
BmsRegistArraySet::DeleteArray(const std::string& key)
{
	if (this->Exists(key)) {
		delete table_[key];
		table_.erase(key);
	}
}


bool
BmsRegistArraySet::Exists(const std::string& key) const
{
	return table_.find(key) != table_.end();
}

BmsRegistArray&
BmsRegistArraySet::operator [](const std::string& key)
{
	if (NOT(this->Exists(key))) {
		throw BmsOutOfRangeAccessException(typeid(*this));;
	}
	return *table_[key];
}

const BmsRegistArray&
BmsRegistArraySet::operator [](const std::string& key) const
{
	if (NOT(this->Exists(key))) {
		throw BmsOutOfRangeAccessException(typeid(*this));;
	}
	return *table_.find(key)->second;
}

void
BmsRegistArraySet::Clear(void)
{
	for (std::map<std::string, BmsRegistArray*>::iterator it = table_.begin(); it != table_.end(); ++it) {
		it->second->Clear();
	}
}


BmsRegistArraySet::Iterator
BmsRegistArraySet::Begin()
{
	return table_.begin();
}

BmsRegistArraySet::Iterator
BmsRegistArraySet::End()
{
	return table_.end();
}

BmsRegistArraySet::ConstIterator
BmsRegistArraySet::Begin() const
{
	return table_.begin();
}

BmsRegistArraySet::ConstIterator
BmsRegistArraySet::End() const
{
	return table_.end();
}
