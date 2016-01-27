#include "bmsbel\bms_buffer.h"

#include "bmsbel\bms_define.h"
#include "bmsbel\bms_util.h"
#include "bmsbel\bms_exception.h"
#include "bmsbel\bms_word.h"


BmsBuffer::BmsBuffer(int length)
{
	length_ = length;
}

BmsBuffer::BmsBuffer(int length, const std::string& input)
{
	length_ = length;
	if (input.size() % 2 == 1) {
		throw BmsInvalidStringConvertedAsBufferException(input);
	}

	// COMMENT
	// previous method was very bad in precision.
	const char* p = input.c_str();
	for (unsigned int i = 0; i < input.size() / 2; i++) {
		int ai = length_ * i / (input.size() / 2);
		array_[ai] = BmsWord(p);
		p += 2;
	}
}


BmsBuffer::~BmsBuffer()
{
}


unsigned int
BmsBuffer::GetLength(void) const
{
	return array_.size();
}

unsigned int
BmsBuffer::GetCount(void) const
{
	return array_.size();
}


BmsWord
BmsBuffer::Get(unsigned int pos) const
{
	if (array_.find(pos) == array_.end()) return 0;
	else return array_.at(pos);
}
/*
const BmsWord
BmsBuffer::operator [](unsigned int pos)
{
	return array_[pos];
}*/

void
BmsBuffer::Set(unsigned int pos, BmsWord v) {
	array_[pos] = v;
}

void
BmsBuffer::DeleteAt(unsigned int pos)
{
	array_.erase(pos);
}

void
BmsBuffer::Clear(void)
{
	array_.clear();
}


void
BmsBuffer::Merge(const BmsBuffer& buffer)
{
	Merge(0, buffer);
}

void
BmsBuffer::Merge(unsigned int start, const BmsBuffer& buffer)
{
	// merge buffer from start position
	for (auto it = buffer.Begin(); it != buffer.End(); ++it) {
		array_[start + it->first] = it->second;
	}
}

void BmsBuffer::Append(const BmsBuffer& buffer) {
	length_ += buffer.GetLength();
	Merge(GetLength(), buffer);
}

BmsBuffer
BmsBuffer::SubBuffer(unsigned int from, unsigned int length) const
{
	BmsBuffer buf(length);
	for (unsigned int i = 0; i < length; ++i) {
		BmsWord w = Get(from + i);
		if (w != BmsWord::MIN)
			buf.Set(i, w);
	}
	return buf;
}

bool
BmsBuffer::Contains(const BmsWord &word) const
{
	for (auto it = this->Begin(); it != this->End(); ++it) {
		if (it->second == word) {
			return true;
		}
	}
	return false;
}

void
BmsBuffer::MagnifyBy(unsigned int multiplier)
{
	// COMMENT: we may can optimize this routine,
	// like only changing key number.
	int old_length = static_cast<int>(this->GetLength());
	length_ *= multiplier;
	for (int i = old_length - 1; i > 0; --i) {
		array_[i * multiplier] = array_[i];
		DeleteAt(i);
	}
}


std::string
BmsBuffer::ToString(void) const
{
	if (this->GetLength() == 0) {
		return "";
	}

	// calculate GCD (minimize output string)
	unsigned int step = this->GetLength();
	for (auto it = Begin(); it != End(); ++it) {
		step = BmsUtil::GCD(step, it->first);
	}

	std::string tmp;
	tmp.reserve(this->GetLength() / step * 2);
	for (unsigned int i = 0; i < this->GetLength(); i += step) {
		tmp.append(Get(i).ToCharPtr());
	}
	return tmp;
}


BmsBuffer::Iterator
BmsBuffer::Begin(void)
{
	return array_.begin();
}

BmsBuffer::Iterator
BmsBuffer::End(void)
{
	return array_.end();
}

BmsBuffer::ConstIterator
BmsBuffer::Begin(void) const
{
	return array_.begin();
}

BmsBuffer::ConstIterator
BmsBuffer::End(void) const
{
	return array_.end();
}

BmsBuffer::Iterator BmsBuffer::Reset(int startbar) {
	for (iter_ = Begin(); iter_ != End(); ++iter_) {
		if (iter_->first >= startbar) break;
	}
	return iter_;
}

BmsBuffer::Iterator BmsBuffer::Next() {
	// COMMENT: should we check _iter == End() ?
	return ++iter_;
}

BmsBuffer::Iterator BmsBuffer::Current() {
	return iter_;
}