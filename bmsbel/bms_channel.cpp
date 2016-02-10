#include "bmsbel/bms_channel.h"

#include "bmsbel/bms_exception.h"

// -- BmsChannel ---------------------------------------------------------
BmsChannel::BmsChannel(const BmsWord& channel_number) :
channel_number_(channel_number),
buffers_(0)
{
}

BmsChannel::~BmsChannel()
{
	for (unsigned int i = 0; i < buffers_.size(); ++i) {
		delete buffers_[i];
	}
}


void
BmsChannel::SetChannelNumber(const BmsWord& channel_number)
{
	channel_number_ = channel_number;
}

BmsWord&
BmsChannel::GetChannelNumber()
{
	return channel_number_;
}


unsigned int
BmsChannel::GetBufferCount(void) const
{
	return buffers_.size();
}


BmsBuffer&
BmsChannel::MakeNewBuffer(void)
{
	buffers_.push_back(new BmsBuffer());
	return *buffers_.back();
}


BmsBuffer&
BmsChannel::GetBuffer(unsigned int pos)
{
	while (pos >= buffers_.size()) {
		this->MakeNewBuffer();
	}
	return *buffers_[pos];
}

BmsBuffer&
BmsChannel::operator [](unsigned int pos)
{
	return this->GetBuffer(pos);
}

BmsChannel::Iterator
BmsChannel::Begin(void)
{
	return buffers_.begin();
}

BmsChannel::Iterator
BmsChannel::End(void)
{
	return buffers_.end();
}

BmsChannel::ConstIterator
BmsChannel::Begin(void) const
{
	return buffers_.begin();
}

BmsChannel::ConstIterator
BmsChannel::End(void) const
{
	return buffers_.end();
}


bool
BmsChannel::Contains(const BmsWord &word) const
{
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		if ((*it)->Contains(word)) {
			return true;
		}
	}
	return false;
}



barindex
BmsChannel::GetObjectExistsFirstPosition() const
{
	bool flag = false;
	barindex pos = 0;
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		if (!(*it)->GetObjectCount()) continue;
		barindex tmp = (*it)->Begin()->first;
		if (tmp < pos || !flag) {
			pos = tmp;
			flag = true;
		}
	}
	return pos;
}



barindex
BmsChannel::GetObjectExistsMaxPosition() const
{
	barindex pos = 0;
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		if (!(*it)->GetObjectCount()) continue;
		barindex tmp = (--(*it)->End())->first;
		if (tmp > pos) {
			pos = tmp;
		}
	}
	return pos;
}


void
BmsChannel::MultiplyBarDivisionCount(double multiplier)
{
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		(*it)->MagnifyBy(multiplier);
	}
}

unsigned int 
BmsChannel::GetObjectCount(barindex start, barindex length) const {
	unsigned int r = 0;
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		r += (*it)->GetObjectCount(start, length);
	}
	return r;
}

void 
BmsChannel::GetObjectExistBar(std::set<barindex> &barmap) const
{
	for (auto it = Begin(); it != End(); ++it) {
		(**it).GetObjectExistBar(barmap);
	}
}

bool
BmsChannel::IsChannel(void) const
{
	return true;
}

bool
BmsChannel::IsWavChannel(void) const
{
	// 01, 11 -- 1Z, 21 -- 2Z, 31 -- 3Z, 41 -- 4Z, 51 -- 5Z, 61 -- 6Z
	switch (GetChannelType()) {
	case BmsChannelType::BGM:
	case BmsChannelType::FIRSTPLAYER:
	case BmsChannelType::FIRSTPLAYERHIDDEN:
	case BmsChannelType::FIRSTPLAYERLN:
	case BmsChannelType::SECONDPLAYER:
	case BmsChannelType::SECONDPLAYERHIDDEN:
	case BmsChannelType::SECONDPLAYERLN:
		return true;
	default:
		return false;
	}
}

bool
BmsChannel::IsShouldPlayWavChannel(void) const
{
	// 01, 11 -- 1Z, 21 -- 2Z, 51 -- 5Z, 61 -- 6Z

	switch (GetChannelType()) {
	case BmsChannelType::BGM:
	case BmsChannelType::FIRSTPLAYER:
	case BmsChannelType::FIRSTPLAYERLN:
	case BmsChannelType::SECONDPLAYER:
	case BmsChannelType::SECONDPLAYERLN:
		return true;
	default:
		return false;
	}
}

bool
BmsChannel::IsFirstPlayerNoteChannel(void) const
{
	switch (GetChannelType()) {
	case BmsChannelType::FIRSTPLAYER:
	case BmsChannelType::FIRSTPLAYERLN:
		return true;
	default:
		return false;
	}
}

bool
BmsChannel::IsSecondPlayerNoteChannel(void) const
{
	switch (GetChannelType()) {
	case BmsChannelType::SECONDPLAYER:
	case BmsChannelType::SECONDPLAYERLN:
		return true;
	default:
		return false;
	}
}

bool
BmsChannel::IsFirstPlayerChannel(void) const
{
	switch (GetChannelType()) {
	case BmsChannelType::FIRSTPLAYER:
	case BmsChannelType::FIRSTPLAYERHIDDEN:
	case BmsChannelType::FIRSTPLAYERLN:
	case BmsChannelType::FIRSTPLAYERMINE:
		return true;
	default:
		return false;
	}
}

bool
BmsChannel::IsSecondPlayerChannel(void) const
{
	switch (GetChannelType()) {
	case BmsChannelType::SECONDPLAYER:
	case BmsChannelType::SECONDPLAYERHIDDEN:
	case BmsChannelType::SECONDPLAYERLN:
	case BmsChannelType::SECONDPLAYERMINE:
		return true;
	default:
		return false;
	}
}

bool
BmsChannel::IsBmpChannel(void) const
{
	switch (channel_number_.ToInteger()) {
		// 04 06 07 0A A5
	case 4:
	case 6:
	case 7:
	case 10:
	case 365:
		return true;

	default:
		return false;
	}
}

bool
BmsChannel::IsLongNoteChannel(void) const
{
	switch (GetChannelType()) {
	case BmsChannelType::FIRSTPLAYERLN:
	case BmsChannelType::SECONDPLAYERLN:
		return true;

	default:
		return false;
	}
}

bool
BmsChannel::IsMineNoteChannel(void) const
{
	switch (GetChannelType()) {
	case BmsChannelType::FIRSTPLAYERMINE:
	case BmsChannelType::SECONDPLAYERMINE:
		return true;

	default:
		return false;
	}
}

bool BmsChannel::Is5KeyChannel(void) const {
	int c = channel_number_.ToInteger();
	int c_key = c % 36;	// base36
	return (IsFirstPlayerChannel() || IsSecondPlayerChannel())
		&& (c_key == 1 || c_key == 2 || c_key == 3 || c_key == 4 || c_key == 5
		|| c_key == 6);
}

bool BmsChannel::Is7KeyChannel(void) const {
	int c = channel_number_.ToInteger();
	int c_key = c % 36;	// base36
	return (IsFirstPlayerChannel() || IsSecondPlayerChannel())
		&& (c_key == 1 || c_key == 2 || c_key == 3 || c_key == 4 || c_key == 5
		|| c_key == 8 || c_key == 9 || c_key == 6);
}

bool BmsChannel::Is9KeyChannel(void) const {
	int c = channel_number_.ToInteger();
	int c_key = c % 36;	// base36
	return (IsFirstPlayerChannel() || IsSecondPlayerChannel())
		&& (c_key == 1 || c_key == 2 || c_key == 3 || c_key == 4 || c_key == 5
		|| c_key == 8 || c_key == 9 || c_key == 6 || c_key == 7);
}

bool BmsChannel::IsVisibleChannel() const {
	return IsFirstPlayerNoteChannel() || IsSecondPlayerNoteChannel();
}

int BmsChannel::GetChannelType() const {
	int c = channel_number_.ToInteger();
	if (c == 1)
		return BmsChannelType::BGM;
	else if (c == 2)
		return BmsChannelType::MEASURELENGTH;
	else if (c == 3)
		return BmsChannelType::BPM;
	else if (c == 4)
		return BmsChannelType::BGA;
	else if (c == 5)
		return BmsChannelType::EXTOBJ;
	else if (c == 6)
		return BmsChannelType::BGAPOOR;
	else if (c == 7)
		return BmsChannelType::BGALAYER;
	else if (c == 8)
		return BmsChannelType::EXTBPM;
	else if (c == 9)
		return BmsChannelType::STOP;
	else if (c == 10)
		return BmsChannelType::BGALAYER2;
	else if (c == 11)
		return BmsChannelType::BGAOPAC;
	else if (c == 12)
		return BmsChannelType::BGALAYEROPAC;
	else if (c == 13)
		return BmsChannelType::BGALAYER2OPAC;
	else if (c == 14)
		return BmsChannelType::BGAPOOROPAC;
	else if (c == 365)
		return BmsChannelType::BGAKEYBOUND;
	else if (c == 361)
		return BmsChannelType::BGAARGB;
	else if (c == 362)
		return BmsChannelType::BGALAYERARGB;
	else if (c == 363)
		return BmsChannelType::BGALAYER2ARGB;
	else if (c == 364)
		return BmsChannelType::BGAPOORARGB;
	else if (37 <= c && c < 72)
		return BmsChannelType::FIRSTPLAYER;
	else if (73 <= c && c < 108)
		return BmsChannelType::SECONDPLAYER;
	else if (109 <= c && c < 144)
		return BmsChannelType::FIRSTPLAYERHIDDEN;
	else if (145 <= c && c < 180)
		return BmsChannelType::SECONDPLAYERHIDDEN;
	else if (181 <= c && c < 216)
		return BmsChannelType::FIRSTPLAYERLN;
	else if (217 <= c && c < 252)
		return BmsChannelType::SECONDPLAYERLN;
	else if (469 <= c && c < 504)
		return BmsChannelType::FIRSTPLAYERMINE;
	else if (505 <= c && c < 540)
		return BmsChannelType::SECONDPLAYERMINE;
	return 0;	// unknown
}



// -- BmsChannelManager --------------------------------------------------
BmsChannelManager::BmsChannelManager(void) :
channels_()
{
}

BmsChannelManager::~BmsChannelManager()
{
	for (std::map<BmsWord, BmsChannel*>::iterator it = channels_.begin(); it != channels_.end(); ++it) {
		delete it->second;
	}
}

unsigned int
BmsChannelManager::GetCount(void) const
{
	return channels_.size();
}

BmsChannel&
BmsChannelManager::GetChannel(const BmsWord& channel_number)
{
	if (channels_.find(channel_number) == channels_.end()) {
		channels_[channel_number] = new BmsChannel(channel_number);
	}
	return *channels_[channel_number];
}

BmsChannel&
BmsChannelManager::operator [](const BmsWord &channel_number)
{
	return this->GetChannel(channel_number);
}


void
BmsChannelManager::DeleteChannel(const BmsWord &channel_number)
{
	std::map<BmsWord, BmsChannel*>::iterator it = channels_.find(channel_number);
	if (it != channels_.end()) {
		delete it->second;
		channels_.erase(it);
	}
}

void
BmsChannelManager::Clear(void)
{
	for (Iterator it = this->Begin(); it != this->End(); ++it) {
		delete it->second;
	}
	channels_.clear();
}

bool
BmsChannelManager::IsExists(const BmsWord channel) const
{
	return channels_.find(channel) != channels_.end();
}

bool
BmsChannelManager::Contains(const BmsWord& word) const
{
	return this->Contains(word, &BmsChannel::IsChannel);
}


bool
BmsChannelManager::Contains(const BmsWord& word, BmsChannelConditionJudgeFunction func) const
{
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		if ((it->second->*func)() && it->second->Contains(word)) {
			return true;
		}
	}
	return false;
}


barindex
BmsChannelManager::GetObjectExistsMaxPosition(void) const
{
	return this->GetObjectExistsMaxPosition(&BmsChannel::IsChannel);
}

barindex
BmsChannelManager::GetObjectExistsMaxPosition(BmsChannelConditionJudgeFunction func) const
{
	barindex pos = 0;
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		if ((it->second->*func)()) {
			barindex tmp = it->second->GetObjectExistsMaxPosition();
			if (tmp > pos) {
				pos = tmp;
			}
		}
	}
	return pos;
}

barindex
BmsChannelManager::GetObjectExistsFirstPosition(BmsChannelConditionJudgeFunction func) const
{
	bool flag = false;
	barindex pos = 0;
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		if ((it->second->*func)()) {
			barindex tmp = it->second->GetObjectExistsFirstPosition();
			if (tmp < pos || !flag) {
				pos = tmp;
				flag = true;
			}
		}
	}
	return pos;
}

void BmsChannelManager::GetObjectExistBar(std::set<barindex> &barmap) const {
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		it->second->GetObjectExistBar(barmap);
	}
}

void
BmsChannelManager::MultiplyBarDivisionCount(double multiplier)
{
	for (ConstIterator it = this->Begin(); it != this->End(); ++it) {
		it->second->MultiplyBarDivisionCount(multiplier);
	}
}



BmsChannelManager::Iterator
BmsChannelManager::Begin(void)
{
	return channels_.begin();
}

BmsChannelManager::Iterator
BmsChannelManager::End(void)
{
	return channels_.end();
}

BmsChannelManager::ConstIterator
BmsChannelManager::Begin(void) const
{
	return channels_.begin();
}

BmsChannelManager::ConstIterator
BmsChannelManager::End(void) const
{
	return channels_.end();
}
