#include <stdlib.h>

#include <utility>

#include "bmsbel\bms_parser.h"

#include "bmsbel\bms_util.h"
#include "bmsbel\bms_exception.h"
#include <time.h>


// -- BmsParser::StartInfo -----------------------------------------------
BmsParser::StartInfo::StartInfo(BmsBms&                   bms,
	BmsRandom::RootStatement& root_statement,
	Reactor&                  reactor) :
	bms_(bms),
	root_statement_(root_statement),
	reactor_(reactor),
	make_syntax_tree_(true),
	ignore_channel_set_()
{
}

// -- BmsParser::ParsingInfo ---------------------------------------------
BmsParser::ParsingInfo::ParsingInfo(StartInfo&  start_info,
	FrameStack& frame_stack) :
	bms_(start_info.bms_),
	root_statement_(start_info.root_statement_),
	reactor_(start_info.reactor_),
	make_syntax_tree_(start_info.make_syntax_tree_),
	ignore_channel_set_(start_info.ignore_channel_set_),
	line_data_(),
	frame_stack_(frame_stack),
	random_value_queue_(),
	is_first_parse_(true),
	line_number_(0),
	bar_(0),
	channel_number_(BmsWord::MIN),
	key_(),
	value_(),
	bar_change_ratio_(1.0),
	word_array_()
{
}

// -- BmsParser::Reactor -------------------------------------------------
BmsParser::Reactor
BmsParser::Reactor::DefaultObject;

BmsParser::Reactor::Reactor(void) :
random_value_(static_cast<unsigned int>(time(NULL)))
{
}

BmsParser::Reactor::~Reactor()
{
}

std::string
BmsParser::Reactor::AtDuplicateHeader(Parser& parser,
const std::string header,
const std::string before,
const std::string present)
{
	NOT_USE_VAR(before);
	NOT_USE_VAR(present);
	if (BmsBelOption::ERROR_ON_DUPLICATED_HEADER)
		throw BmsParseDuplicateHeaderException(parser.GetInfo().line_number_, header);
	else
		// cover with new value
		return present;
}

double
BmsParser::Reactor::AtDuplicateBarChange(Parser& parser,
unsigned int bar,
double       before,
double       present)
{
	NOT_USE_VAR(before);
	NOT_USE_VAR(present);
	throw BmsParseDuplicateBarChangeException(parser.GetInfo().line_number_, bar);
}

bool
BmsParser::Reactor::AtParseError(Parser& parser, BmsParseException& e, const std::string& line)
{
	NOT_USE_VAR(parser);
	NOT_USE_VAR(e);
	NOT_USE_VAR(line);
	return false;
}

unsigned int
BmsParser::Reactor::AtGenerateRandom(Parser& parser, unsigned int max)
{
	NOT_USE_VAR(parser);
	random_value_ = random_value_ * 22695477 + 1;
	return (((random_value_ >> 16) & 32767) % max) + 1;
}

void
BmsParser::Reactor::AtLineDataRead(Parser& parser)
{
	NOT_USE_VAR(parser);
}

void
BmsParser::Reactor::AtParseStart(Parser& parser)
{
	NOT_USE_VAR(parser);
}

void
BmsParser::Reactor::AtLineEnd(Parser& parser)
{
	NOT_USE_VAR(parser);
}

void
BmsParser::Reactor::AtParseEnd(Parser& parser)
{
	NOT_USE_VAR(parser);
}

// -- BmsParser::Parser -----------------------------------------------------------
#ifdef USE_MBCS
void
BmsParser::Parse(const std::wstring& filename_w, BmsBms& bms, const char *encoding)
{
	char filename[1024];
	BmsUtil::wchar_to_utf8(filename_w.c_str(), filename, 1024);
	Parse(filename, bms, encoding);
}

void
BmsParser::Parse(const std::wstring& filename_w, BmsBms& bms)
{
	char filename[1024];
	BmsUtil::wchar_to_utf8(filename_w.c_str(), filename, 1024);
	Parse(filename, bms);
}
#endif

void
BmsParser::Parse(const std::string& filename, BmsBms& bms)
{
	if (BmsUtil::IsFileUTF8(filename))
		BmsParser::Parse(filename, bms, "UTF-8");
	else
		BmsParser::Parse(filename, bms, BmsBelOption::DEFAULT_FALLBACK_ENCODING);
}

void
BmsParser::Parse(const std::string& filename, BmsBms &bms, const char* encoding)
{
	// create default Reactor & StartInfo
	BmsRandom::RootStatement root_statement;					// for #RANDOM or some etc. commands
	BmsParser::Reactor reactor;									// handler - but don't do anything this time
	BmsParser::StartInfo info(bms, root_statement, reactor);	// contains information about parsing BMS
	BmsParser::Parse(filename, encoding, info);
}

void
BmsParser::Parse(const std::string& filename, const char* encoding, StartInfo& start_info)
{
	BmsTextFileReader reader(filename, encoding);
	BmsParser::Parse(reader, start_info);
}

void
BmsParser::Parse(BmsTextFileReader& reader, StartInfo& start_info)
{
	std::auto_ptr<FrameStack> frame_stack_auto_ptr;
	if (start_info.make_syntax_tree_) {
		frame_stack_auto_ptr.reset(new FrameStackMakeSyntaxTree(start_info.bms_, start_info.root_statement_));
	}
	else {
		frame_stack_auto_ptr.reset(new FrameStackMasterOnly(start_info.bms_));
	}

	BmsParser::Parser parser(start_info, *frame_stack_auto_ptr);
	{
		std::string tmp;
		while (reader.ReadLine(tmp, true)) {
			for (int i = tmp.length() - 1; i >= 0; --i) {
				if (tmp[i] == L' ' || tmp[i] == L'\t') {
					tmp.erase(i);
				}
				else {
					break;
				}
			}
			parser.info_.line_data_.push_back(tmp);
			parser.info_.reactor_.AtLineDataRead(parser);
		}
	}
	parser.Start();
}

BmsParser::ParsingInfo&
BmsParser::Parser::GetInfo(void)
{
	return info_;
}

BmsParser::Parser::Parser(StartInfo& start_info, FrameStack& frame_stack) :
info_(start_info, frame_stack)
{
}

void
BmsParser::Parser::Start(void)
{
	info_.reactor_.AtParseStart(*this);

	std::set<unsigned int> ignore_line_set;

	info_.is_first_parse_ = true;
	info_.line_number_ = 0;
	info_.frame_stack_.SetRandomValue(0);

	//
	// first parsing
	// - parse metadata / if clause / bar(measure) length first
	//
	for (std::vector<std::string>::const_iterator it = info_.line_data_.begin(); it != info_.line_data_.end(); ++it) {
		info_.line_number_ += 1;
		try {
			this->ParseDefault(it->c_str());
		}
		catch (BmsParseException& e) {
			if (NOT(info_.reactor_.AtParseError(*this, e, *it))) {
				throw;
			}
			ignore_line_set.insert(info_.line_number_);
		}
		info_.reactor_.AtLineEnd(*this);
	}
	if (info_.frame_stack_.GetDepth() > 1) {
		throw BmsParseNotFoundEndIfException(info_.line_number_);
	}

	info_.is_first_parse_ = false;
	info_.line_number_ = 0;
	info_.frame_stack_.SetRandomValue(0);

	//
	// second parsing
	// - parse bar data
	//
	for (std::vector<std::string>::const_iterator it = info_.line_data_.begin(); it != info_.line_data_.end(); ++it) {
		info_.line_number_ += 1;
		if (ignore_line_set.find(info_.line_number_) != ignore_line_set.end()) {
			continue;
		}
		try {
			this->ParseDefault(it->c_str());
		}
		catch (BmsParseException& e) {
			if (NOT(info_.reactor_.AtParseError(*this, e, *it))) {
				throw;
			}
			ignore_line_set.insert(info_.line_number_);
		}
		info_.reactor_.AtLineEnd(*this);
	}
	if (info_.frame_stack_.GetDepth() > 1) {
		throw BmsParseNotFoundEndIfException(info_.line_number_);
	}

	info_.reactor_.AtParseEnd(*this);
}

void
BmsParser::Parser::ParseDefault(const char* str)
{
	while (*str == L' ' || *str == L'\t') {
		++str;
	}
	switch (*str) {
	case '#':
		this->ParseHeaderOrBar(str + 1);
		break;

	case '\0':
		break;

	default:
		this->ParseComment(str);
		break;
	}
}

void
BmsParser::Parser::ParseComment(const char* str)
{
	// 今は特に何もしない
	NOT_USE_VAR(str);
}

void
BmsParser::Parser::ParseHeaderOrBar(const char* str)
{
	if (BmsUtil::IsDigit(*str)) {
		this->ParseBar(str);
	}
	else if (BmsUtil::IsAlphabet(*str)) {
		this->ParseHeaderKey(str);
	}
	else {
		throw BmsParseInvalidCharAsHeaderOrBarException(info_.line_number_, std::string(str, 1));
	}
}

void
BmsParser::Parser::ParseHeaderKey(const char* str)
{
	info_.key_.clear();
	for (unsigned int i = 0;; ++i) {
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\0') {
			info_.key_.assign(str, i);
			BmsUtil::StringToUpper(info_.key_);
			this->ParseHeaderSeparator(str + i);
			break;
		}
		if (BmsUtil::IsNotHex36(str[i])) {
			throw BmsParseInvalidCharAsHeaderKeyException(info_.line_number_, std::string(str + i, 1));
		}
	}
}

void
BmsParser::Parser::ParseHeaderSeparator(const char* str)
{
	while (*str == ' ' || *str == '\t') {
		++str;
	}
	this->ParseHeaderValue(str);
}

void
BmsParser::Parser::ParseHeaderValue(const char* str)
{
	info_.value_.assign(str);

	if (info_.key_ == "RANDOM" || info_.key_ == "IF" || info_.key_ == "ENDIF") {
		this->ParseRandomStatement();
		return;
	}

	if (info_.is_first_parse_) {
		return;
	}

	WriteDownFunction function = &Parser::WriteDownHeader;
	for (BmsRegistArraySet::Iterator it = info_.bms_.GetRegistArraySet().Begin(); it != info_.bms_.GetRegistArraySet().End(); ++it) {
		if (BmsRegistArray::CheckConstruction(it->first, info_.key_)) {
			function = &Parser::WriteDownRegistArray;
			//this->WriteDownRegistArray(master_frame_data_);
		}
	}
	info_.frame_stack_.CallWriteDownFunction(*this, function);
}

void
BmsParser::Parser::WriteDownHeader(const FrameData& data)
{
	std::string header_value = info_.value_;
	if (data.bms_.GetHeaders().IsExists(info_.key_)) {
		header_value = info_.reactor_.AtDuplicateHeader(*this, info_.key_, data.bms_.GetHeaders()[info_.key_].ToString(), info_.value_);
	}
	data.bms_.GetHeaders().Set(info_.key_, header_value);
}

void
BmsParser::Parser::WriteDownRegistArray(const FrameData& data)
{
	std::string header_value = info_.value_;
	BmsWord pos(info_.key_.substr(info_.key_.length() - 2));
	std::string array_name = info_.key_.substr(0, info_.key_.length() - 2);
	if (NOT(data.bms_.GetRegistArraySet().Exists(array_name))) {
		throw BMS_INTERNAL_EXCEPTION;
	}
	BmsRegistArray& regist_array = data.bms_.GetRegistArraySet()[array_name];

	if (regist_array.IsExists(pos)) {
		header_value = info_.reactor_.AtDuplicateHeader(*this, info_.key_, regist_array[pos], info_.value_);
	}
	regist_array.Set(pos, header_value);
}

void
BmsParser::Parser::ParseBar(const char* str)
{
	if (BmsUtil::IsNotDigit(str[0]) ||
		BmsUtil::IsNotDigit(str[1]) ||
		BmsUtil::IsNotDigit(str[2])) {
		throw BmsParseInvalidCharAsBarException(info_.line_number_, std::string(str, 3));
	}
	info_.bar_ = atoi(std::string(str, 3).c_str());
	this->ParseChannel(str + 3);
}

void
BmsParser::Parser::ParseChannel(const char* str)
{
	if (NOT(BmsWord::CheckConstruction(str))) {
		throw BmsParseInvalidCharAsChannelException(info_.line_number_, std::string(str, 2));
	}
	info_.channel_number_ = BmsWord(str);
	if (info_.ignore_channel_set_.find(info_.channel_number_) != info_.ignore_channel_set_.end()) {
		return;
	}
	if ((info_.channel_number_ == BmsWord("02") && info_.is_first_parse_) ||
		(info_.channel_number_ != BmsWord("02") && NOT(info_.is_first_parse_))) {
		this->ParseColon(str + 2);
	}
}

void
BmsParser::Parser::ParseColon(const char* str)
{
	if (*str != ':') {
		throw BmsParseInvalidCharAsColonException(info_.line_number_, std::string(str, 1));
	}
	if (info_.is_first_parse_) {
		this->ParseBarChangeValue(str + 1);
	}
	else {
		this->ParseObjectArray(str + 1);
	}
}

void
BmsParser::Parser::ParseBarChangeValue(const char* str)
{
	if (NOT(BmsUtil::StringToFloat(str, &info_.bar_change_ratio_))) {
		throw BmsParseInvalidBarChangeValueException(info_.line_number_, std::string(str));
	}
	if (info_.bar_change_ratio_ <= 0.0) {
		throw BmsParseInvalidBarChangeValueException(info_.line_number_, std::string(str));
	}
	info_.frame_stack_.CallWriteDownFunction(*this, &Parser::WriteDownBarChangeValue);
}

void
BmsParser::Parser::WriteDownBarChangeValue(const FrameData& data)
{
	double tmp = info_.bar_change_ratio_;

	if (data.bms_.GetBarManager()[info_.bar_].GetRatio() != 1.0) {
		tmp = info_.reactor_.AtDuplicateBarChange(
			*this, info_.bar_, data.bms_.GetBarManager()[info_.bar_].GetRatio(), info_.bar_change_ratio_);
	}

	unsigned int new_bar_division_count =
		this->DecideBarDivisionCountFromBarChange(data.bms_.GetBarManager().GetBarDivisionCount(), tmp);
	if (new_bar_division_count > data.bms_.GetBarManager().GetBarDivisionCount()) {
		data.bms_.MultiplyBarDivisionCount(new_bar_division_count / data.bms_.GetBarManager().GetBarDivisionCount());
	}
	data.bms_.GetBarManager()[info_.bar_].SetRatio(tmp);
}

void
BmsParser::Parser::ParseObjectArray(const char* str)
{
	info_.word_array_.clear();

	if (*str == '\0' || *str == ' ' || *str == '\t') {
		// 先頭で終了はオブジェが一つも無い
		throw BmsParseNoObjectArrayException(info_.line_number_);
	}

	for (unsigned int i = 0; str[i] != '\0'; i += 2) {
		if (BmsWord::CheckConstruction(str + i)) {
			info_.word_array_.push_back(BmsWord(str + i));
		}
		else if (str[i] == '\0' || str[i] == ' ' || str[i] == '\t') {
			// 末尾の空白無視
			while (str[i] != '\0') {
				if (str[i] != ' ' && str[i] != '\t') {
					throw BmsParseInvalidEndAsObjectArrayException(info_.line_number_);
				}
				++i;
			}
			break;
		}
		else {
			throw BmsParseInvalidCharAsObjectArrayException(
				info_.line_number_, std::string(str + i, str[i + 1] != L'\0' ? 2 : 1));
		}
	}

	info_.frame_stack_.CallWriteDownFunction(*this, &Parser::WriteDownObjectArray);
}

void
BmsParser::Parser::WriteDownObjectArray(const FrameData& data)
{
	unsigned int to = data.bms_.GetBarManager().GetChannelPositionByBarNumber(info_.bar_);
	unsigned int length = data.bms_.GetBarManager()[info_.bar_].GetLength();

	if (length % info_.word_array_.size() != 0) {
		unsigned int new_bar_division_count = this->DecideBarDivisionCountFromObjectArray(
			data.bms_.GetBarManager().GetBarDivisionCount(), data.bms_.GetBarManager()[info_.bar_].GetRatio());

		if (new_bar_division_count > data.bms_.GetBarManager().GetBarDivisionCount()) {
			data.bms_.MultiplyBarDivisionCount(new_bar_division_count / data.bms_.GetBarManager().GetBarDivisionCount());
			to = data.bms_.GetBarManager().GetChannelPositionByBarNumber(info_.bar_);
			length = data.bms_.GetBarManager()[info_.bar_].GetLength();
		}
	}

	unsigned int channel_dup_count;
	if (info_.channel_number_ == BmsWord("01")) {
		channel_dup_count = data.bgm_channel_counter_[info_.bar_];
		data.bgm_channel_counter_[info_.bar_] += 1;
	}
	else {
		channel_dup_count = 0;
	}

	int k = 0;
	for (double i = static_cast<double>(to);
		static_cast<unsigned int>(i) < to + length;
		i += static_cast<double>(length) / static_cast<double>(info_.word_array_.size())) {
		if (info_.word_array_[k] != BmsWord::MIN) {
			data.bms_.GetChannelManager()[info_.channel_number_][channel_dup_count][static_cast<unsigned int>(i)]
				= info_.word_array_[k];
		}
		//     BmsChannelManager& mg = bms.GetChannelManager();
		//     BmsChannel& ch = mg[channel_];
		//     BmsChannelBuffer& chbf = ch[channel_dup_count];
		//     BmsWord wd = word_array[static_cast<unsigned int>( i )];
		//     chbf[static_cast<unsigned int>( i )] = wd;
		++k;
	}
}

void
BmsParser::Parser::ParseRandomStatement(void)
{
	if (info_.is_first_parse_) {
		if (info_.key_ == "RANDOM") {
			int tmp;
			if (NOT(BmsUtil::StringToInteger(info_.value_, &tmp, 10))) {
				throw BmsParseInvalidRandomValueException(info_.line_number_, info_.key_, info_.value_);
			}
			if (tmp <= 0) {
				throw BmsParseInvalidRandomValueException(info_.line_number_, info_.key_, info_.value_);
			}
			unsigned int random_max = static_cast<unsigned int>(tmp);
			info_.frame_stack_.PushGenerateRandom(random_max);
			info_.frame_stack_.SetRandomValue(info_.reactor_.AtGenerateRandom(*this, random_max));
			info_.random_value_queue_.push(info_.frame_stack_.GetRandomValue());
		}
		else if (info_.key_ == "IF") {
			int prerequisite;
			if (NOT(BmsUtil::StringToInteger(info_.value_, &prerequisite, 10)) ||
				prerequisite <= 0) {
				throw BmsParseInvalidRandomValueException(info_.line_number_, info_.key_, info_.value_);
			}
			info_.frame_stack_.PushIfStatement(static_cast<unsigned int>(prerequisite));
		}
		else if (info_.key_ == "ENDIF") {
			if (info_.frame_stack_.GetDepth() <= 1) {
				throw BmsParseUnexceptedEndIfException(info_.line_number_);
			}
			info_.frame_stack_.PopIfStatement();
		}
	}
	else {
		if (info_.key_ == "RANDOM") {
			info_.frame_stack_.SetRandomValue(info_.random_value_queue_.front());
			info_.random_value_queue_.pop();
		}
		else if (info_.key_ == "IF") {
			int prerequisite;
			if (NOT(BmsUtil::StringToInteger(info_.value_, &prerequisite, 10)) ||
				prerequisite <= 0) {
				throw BmsParseInvalidRandomValueException(info_.line_number_, info_.key_, info_.value_);
			}
			info_.frame_stack_.MoveChildIfStatement(static_cast<unsigned int>(prerequisite));
		}
		else if (info_.key_ == "ENDIF") {
			if (info_.frame_stack_.GetDepth() <= 1) {
				throw BmsParseUnexceptedEndIfException(info_.line_number_);
			}
			info_.frame_stack_.MoveParentIfStatement();
		}
	}
}

namespace {
	std::pair<unsigned int, unsigned int> factorization(unsigned int x) {
		if (x == 0 || x == 1) {
			return std::make_pair(1, 1);
		}

		unsigned int quotient = 1;
		unsigned int prime_factors[] = { 2, 3, 5, 7 };
		for (unsigned int i = 0; i < sizeof(prime_factors) / sizeof(unsigned int); ++i) {
			while (x % prime_factors[i] == 0) {
				x /= prime_factors[i];
				quotient *= prime_factors[i];
			}
		}
		return std::make_pair(quotient, x);
	}
}

unsigned int
BmsParser::Parser::DecideBarDivisionCountFromBarChange(unsigned int current, double bar_change_ratio)
{
	unsigned int request = BmsUtil::GetDenominator(bar_change_ratio, BmsConst::BAR_DIVISION_COUNT_MAX);

	if (current == 0 || request == 0) {
		throw BMS_INTERNAL_EXCEPTION;
	}

	std::pair<unsigned int, unsigned int> factors = factorization(BmsUtil::LCM(current, request));
	while (factors.first < factors.second * 4) {
		factors.first *= 2;
	}
	if (factors.first > BmsConst::BAR_DIVISION_COUNT_MAX) {
		unsigned int prev = 1;
		for (unsigned int i = 1;; ++i) {
			if (factorization(i).second != 1) {
				continue;
			}
			if (current * i > BmsConst::BAR_DIVISION_COUNT_MAX) {
				return current * prev;
			}
			prev = i;
		}
	}
	return factors.first;
}

unsigned int
BmsParser::Parser::DecideBarDivisionCountFromObjectArray(unsigned int current, double bar_change_ratio)
{
	unsigned int request = info_.word_array_.size() * BmsUtil::GetDenominator(bar_change_ratio, BmsConst::BAR_DIVISION_COUNT_MAX);
	if (current == 0 || request == 0) {
		throw BMS_INTERNAL_EXCEPTION;
	}

	std::pair<unsigned int, unsigned int> factors = factorization(BmsUtil::LCM(current, request));
	while (factors.first < factors.second * 4) {
		factors.first *= 2;
	}
	if (factors.first > BmsConst::BAR_DIVISION_COUNT_MAX) {
		for (unsigned int i = 1;; ++i) {
			if (factorization(i).second != 1) {
				continue;
			}
			if (current * i > request * 3) {
				return current * i;
			}
			if (current * i > BmsConst::BAR_DIVISION_COUNT_MAX) {
				break;
			}
		}
		throw BmsParseTooManyObjectException(info_.line_number_);
	}
	return factors.first;
}
