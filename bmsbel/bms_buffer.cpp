#include "bmsbel/bms_buffer.h"

#include "bmsbel/bms_define.h"
#include "bmsbel/bms_util.h"
#include "bmsbel/bms_exception.h"
#include "bmsbel/bms_word.h"


BmsBuffer::BmsBuffer()
{
}

BmsBuffer::BmsBuffer(int length, const std::string& input)
{
	if (input.size() % 2 == 1) {
		throw BmsInvalidStringConvertedAsBufferException(input);
	}

	// COMMENT
	// previous method was very bad in precision.
	const char* p = input.c_str();
	for (unsigned int i = 0; i < input.size() / 2; i++) {
		int ai = length * i / (input.size() / 2);
		array_[ai] = BmsWord(p);
		p += 2;
	}
}


BmsBuffer::~BmsBuffer()
{
}

unsigned int
BmsBuffer::GetObjectCount() const
{
	return array_.size();
}

unsigned int
BmsBuffer::GetObjectCount(barindex start, barindex length) const
{
	unsigned int r = 0;
	for (auto it = Begin(start); it != End(); ++it) {
		if (length >= 0 && it->first > start + length) break;
		r++;
	}
	return r;
}

void
BmsBuffer::GetObjectExistBar(std::set<barindex> &barmap) const
{
	for (auto it = Begin(); it != End(); ++it) {
		barmap.insert(it->first);
	}
}

BmsWord
BmsBuffer::Get(barindex pos) const
{
	if (array_.find(pos) == array_.end()) return 0;
	else return array_.at(pos);
}

/*
 * getting with operator can cause creation of unnecessary element.
 * that'll make program much slower...
 *
const BmsWord
BmsBuffer::operator [](unsigned int pos)
{
	return array_[pos];
}*/

void
BmsBuffer::Set(barindex pos, BmsWord v) {
	array_[pos] = v;
}

void
BmsBuffer::DeleteAt(barindex pos)
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
BmsBuffer::Merge(barindex start, const BmsBuffer& buffer)
{
	// merge buffer from start position
	for (auto it = buffer.Begin(); it != buffer.End(); ++it) {
		array_[start + it->first] = it->second;
	}
}

BmsBuffer
BmsBuffer::SubBuffer(barindex from, barindex length) const
{
	BmsBuffer buf;
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
BmsBuffer::MagnifyBy(double multiplier)
{
	if (GetObjectCount() == 0) return;
	for (auto it = --End();; --it) {
		array_[it->first * multiplier] = array_[it->first];
		DeleteAt(it->first);
		if (it == Begin()) break;
	}
}


std::string
BmsBuffer::ToString(void) const
{
	if (this->GetObjectCount() == 0) {
		return "";
	}

	// calculate GCD (minimize output string)
	unsigned int step = Begin()->first;
	for (auto it = ++Begin(); it != End(); ++it) {
		step = BmsUtil::GCD(step, it->first);
	}

	std::string tmp;
	tmp.reserve(this->GetObjectCount() / step * 2);
	for (unsigned int i = 0; i < this->GetObjectCount(); i += step) {
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

BmsBuffer::Iterator
BmsBuffer::Begin(barindex startbar) {
	auto iters = array_.equal_range(startbar);
	return iters.first;
}

BmsBuffer::ConstIterator
BmsBuffer::Begin(barindex startbar) const {
	auto iters = array_.equal_range(startbar);
	return iters.first;
}
