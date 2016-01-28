#include <stdlib.h>

#include <utility>

#include "bmsbel\bms_parser.h"

#include "bmsbel\bms_util.h"
#include "bmsbel\bms_exception.h"
#include <time.h>
#include <stdarg.h>

/*
 * http://hitkey.nekokan.dyndns.info/cmds.htm
 */

namespace BmsParser {
	namespace {
		// simple utils
		const char* Trim(const char* str) {
			while (*str == ' ' || *str == '\t') ++str;
			return str;
		}
		const char* ParseSeparator(const char* str) {
			if (!str) return 0;
			while (*str != ' ' && *str != '\t' && *str != '\0') ++str;
			if (*str == 0) return 0;
			else return str;
		}
		const char* ParseColon(const char* str) {
			while (*str != ':') ++str;
			if (*str == 0) return 0;
			else return ++str;
		}
		bool IsBar(const char* str) {
			return (BmsUtil::IsNotDigit(str[0]) ||
				BmsUtil::IsNotDigit(str[1]) ||
				BmsUtil::IsNotDigit(str[2]));
		}
		bool IsComment(const char* str) {
			return *str != '#';
		}
	}

	Parser::Parser(BmsBms& bms) : Parser(bms, {true, rand()}) {}
	Parser::Parser(BmsBms& bms, ParsingInfo info_) : bms_(bms), info_(info_) { }

	void Parser::WriteLogLine(const char* t, ...) {
		va_list vl;
		va_start(vl, t);
		char buf[1024];
		vsprintf_s(buf, t, vl);
		log_ << buf << "\n";
#ifdef _DEBUG
		printf("%s\n", buf);
#endif
	}

	void Parser::Clear() {
		line_ = 0;
		line_data_.clear();
		log_.clear();

		syntax_tag_.clear();
	}

#ifdef USE_MBCS
	bool Parser::Load(const wchar_t* path) {
		FILE *fp;
		if (_wfopen_s(&fp, path, L"rb") == 0) {
			// read whole file (about limit to 10mb)
			char *buf = new char[1024 * 1024 * 10];
			fread(buf, 1, 1024 * 1024 * 10, fp);
			// parse
			bool r = Parse(buf);
			// cleanup
			delete buf;
			return r;
		}
		else {
			// IO error, but don't make log - log clering is a tidious thing...
			return false;
		}
	}
#endif

	bool Parser::Load(const char* path) {
		FILE *fp;
		if (fopen_s(&fp, path, "rb") == 0) {
			// read whole file (about limit to 10mb)
			char *buf = new char[1024 * 1024 * 10];
			fread(buf, 1, 1024 * 1024 * 10, fp);
			// parse
			bool r = Parse(buf);
			// cleanup
			delete buf;
			return r;
		}
		else {
			// IO error, but don't make log - log clering is a tidious thing...
			return false;
		}
	}

	bool Parser::Parse(const char* text) {
		// detect encoding
		char* encodetext = 0;
		if (!BmsUtil::IsUTF8(text)) {
			encodetext = new char[1024 * 1024 * 10];
			BmsUtil::convert_to_utf8(text, encodetext,
				BmsBelOption::DEFAULT_FALLBACK_ENCODING, 1024 * 1024 * 10);
			text = encodetext;
		}

		// split lines
		for (const char *p = text; p; ) {
			const char* pn = strchr(text, '\n');
			std::string buf;
			if (!pn)
				buf.assign(p);
			else
				buf.assign(p, pn - p);
			line_data_.push_back(buf);
			if (!pn) break;
			p = pn + 1;
		}
		if (encodetext) delete encodetext;

		// parse lines
		return ParseLines();
	}

	bool Parser::ParseLines() {
		// initalize first
		Clear();
		WriteLogLine("Bms parsing started");

		//
		// first parsing
		// - parse syntaxs first, and decide to process
		//
		line_ = 0;
		for (auto it = line_data_.begin(); it != line_data_.end(); ++it) {
			++line_;
			const char *p = Trim(it->c_str());
			if (!*p || IsComment(p)) continue;
			if (!ParseRandomStatement(p + 1))
				return false;
		}

		//
		// if we should parse syntax tree then do it
		// if #IF ~ #ENDIF clause had failed, then panic! stop parsing bms.
		//
		if (syntax_tag_.size() != 0) {
			WriteLogLine("parsing error - statement hadn't finished completely.");
			return false;
		}
		if (info_.make_syntax_tree_)
			ProcessRandomStatement();

		//
		// second parsing
		// - parse metadata / regist / barlengths
		//
		line_ = 0;
		for (auto it = line_data_.begin(); it != line_data_.end(); ++it) {
			++line_;
			const char *p = Trim(it->c_str());
			if (!*p || IsComment(p)) continue;
			ParseHeaderOrBar(p + 1);
		}

		//
		// third parsing
		// - parse objects and add them to buffer
		//
		line_ = 0;
		for (auto it = line_data_.begin(); it != line_data_.end(); ++it) {
			++line_;
			const char *p = Trim(it->c_str());
			if (!*p || IsComment(p)) continue;
			ParseObjectArray(p + 1);
		}

		return true;
	}

	void Parser::ParseComment(const char* str)
	{
		NOT_USE_VAR(str);
	}

	//
	// SyntaxTree
	//

#define KEY(v) (key_ == (v))
	bool Parser::ParseRandomStatement(const char* str) {
		// we should parse a little(like if / endif clause)
		// to know what part is syntax we're going to parse
		key_ = "";

		if (KEY("RANDOM") || KEY("RONDOM") || KEY("SETRANDOM") || KEY("ENDRANDOM")) {
			syntax_line_data_.push_back(str);
		}
		else if (KEY("IF") || KEY("SWITCH") || KEY("SETSWITCH")) {
			syntax_line_data_.push_back(str);
			syntax_tag_.push_back(key_);
		}
		else if (KEY("ENDIF") || KEY("ENDSW")) {
			if (syntax_tag_.size() == 0) {
				WriteLogLine("%d line - end tag without start tag", line_);
				return false;
			}
			syntax_line_data_.push_back(str);
			std::string starttag = syntax_tag_.back();
			syntax_tag_.pop_back();
			// check start ~ end tag matching
			if (KEY("ENDIF") && starttag != "IF" ||
				KEY("ENDSW") && starttag != "SWITCH" ||
				KEY("ENDSW") && starttag != "SETSWITCH") {
				WriteLogLine("%d line - start ~ end tag doesn't match.", line_);
				return false;
			}
		}
		else {
			if (syntax_tag_.size()) {
				syntax_line_data_.push_back(str);
			}
		}
		return true;
	}

#define COND			(condition_[syntax_depth_])
#define CONDMATCHED		(condition_matched_[syntax_depth_])
#define CONDMATCH(x)	(random_value_[syntax_depth_] == (x))
	void Parser::ProcessRandomStatement() {
		// initalize
		srand(info_.seed_);
		int syntax_depth_ = 0;
		std::deque<int>	random_value_;		// currently set random value
		std::deque<int> condition_;			// is currently parsable?
		std::deque<int> condition_matched_;	// is condition matched(parsed) before?
		random_value_.push_back(-1);
		condition_.push_back(0);
		condition_matched_.push_back(0);

		for (auto it = syntax_line_data_.begin(); it != syntax_line_data_.end(); ++it) {
			const char *p = it->c_str();
			const char *sep = ParseSeparator(p);
			if (!sep) continue;
			std::string key_(p, sep - p);
			std::string value_(sep + 1);
			int num_ = atoi(value_.c_str());
			if (key_ == "RANDOM" || key_ == "SETRANDOM" || key_ == "RONDOM" ||
				key_ == "SWITCH" || key_ == "SEtSWITCH") {
				++syntax_depth_;
				random_value_.push_back(-1);
				condition_.push_back(0);
				condition_matched_.push_back(0);
				if (num_) {
					if (key_ == "SETRANDOM" || key_ == "SETSWITCH") 
						random_value_[syntax_depth_] = num_;
					else 
						random_value_[syntax_depth_] = rand() % num_ + 1;
				}
			}
			else if (key_ == "ENDSW" || key_ == "ENDRANDOM") {
				--syntax_depth_;
				random_value_.pop_back();
				condition_.pop_back();
				condition_matched_.pop_back();
			}
			else if (key_ == "IF" || key_ == "ELSEIF") {
				if (key_ == "IF") CONDMATCHED = 0;
				if (!CONDMATCHED && CONDMATCH(num_))
					COND = 1;
				else
					COND = 0;
			}
			else if (key_ == "ENDIF") {
				COND = 0;
			}
			else if (key_ == "CASE") {
				if (!CONDMATCHED && CONDMATCH(num_))
					COND = 1;
				else
					COND = 0;
			}
			else if (key_ == "SKIP") {
				if (!CONDMATCHED)
					COND = 1;
				else
					COND = 0;
			}
			else if (key_ == "DEF") {
				COND = 1;
			}
			else {
				// else are basic commands - add them to line_data_
				if (COND)
					line_data_.push_back(*it);
			}
		}

		// after processing, clear syntax_line_data_
		// since original data was translated.
		syntax_line_data_.clear();
	}

	//
	// HeaderOrBar
	//
	
	void Parser::ParseHeaderOrBar(const char* str)
	{
		if (BmsUtil::IsDigit(*str)) {
			this->ParseMeasure(str);
		}
		else if (BmsUtil::IsAlphabet(*str)) {
			this->ParseHeaderKey(str);
		}
		else {
			WriteLogLine("%d line - header parsing error (%s)", line_, str);
		}
	}

	void Parser::ParseHeaderKey(const char* str)
	{
		key_.clear();
		const char* val = ParseSeparator(str);
		if (val) {
			key_.assign(str, val - str + 1);
			ParseHeaderValue(val + 1);
		}
		else {
			WriteLogLine("%d line - cannot find separator (%s)", line_, str);
		}
	}

	void Parser::ParseHeaderValue(const char* str)
	{
		// regist value or metadata(header) value
		value_.assign(str);
		for (auto it = bms_.GetRegistArraySet().Begin(); it != bms_.GetRegistArraySet().End(); ++it) {
			if (BmsRegistArray::CheckConstruction(it->first, key_)) {
				BmsWord pos(key_.substr(key_.length() - 2));
				std::string array_name = key_.substr(0, key_.length() - 2);
				bms_.GetRegistArraySet()[array_name].Set(pos, value_);
			}
			else {
				if (key_ == "STP") {
					const char* measure_s_ = ParseSeparator(str);
					const char* time_s_ = ParseSeparator(measure_s_);
					if (!measure_s_ || !time_s_) {
						WriteLogLine("%d line - Failed to parse #STP (%s)", line_, str);
						return;
					}
					double  measure_ = atof(measure_s_);
					int time_ = atoi(time_s_);
					bms_.GetSTPManager().Add(measure_, time_);
				}
				else {
					bms_.GetHeaders().Set(key_, value_);
				}
			}
		}
	}

	void Parser::ParseMeasure(const char* str)
	{
		if (!IsBar(str)) return;
		// if channel isn't 02(BAR channel)
		// then ignore
		int channel = atoi(str + 3);
		if (channel != 2) return;
		int bar_ = atoi(std::string(str, 3).c_str());
		const char* value_s_ = ParseColon(str);
		if (!value_s_) {
			WriteLogLine("line %d - cannot parse measure length (%s)", line_, str);
			return;
		}
		double value_ = atof(Trim(++value_s_));
		bms_.GetBarManager().SetRatio(bar_, value_);
	}

	//
	// Objects
	//
	void Parser::ParseObjectArray(const char* str) {
		if (!IsBar(str)) return;
		// if channel is 02 then ignore
		int channel = atoi(str + 3);
		if (channel == 2) return;
		int bar_ = atoi(std::string(str, 3).c_str());
		const char* value_s_ = ParseColon(str);
		if (!value_s_) {
			WriteLogLine("line %d - cannot parse measure length (%s)", line_, str);
			return;
		}
		const char* value_ = Trim(++value_s_);

		// construct values
		// if length is odd, ignore last one
		std::vector<BmsWord> word_array_;
		for (unsigned int i = 0; i < strlen(value_) - 1; i += 2) {
			if (BmsWord::CheckConstruction(str + i)) {
				word_array_.push_back(BmsWord(str + i));
			}
			else {
				WriteLogLine("line %d - wrong note object (%s)", std::string(str + i, 2).c_str());
			}
		}

		// construct channel
		// if BGM channel, then allow multiple channel
		unsigned int channel_dup_count = bms_.GetChannelManager()[channel].GetBufferCount();
		if (channel != 1) channel_dup_count = 0;

		// write down values (write 00 value, too.)
		int bar_measure = bms_.GetBarManager().GetBarNumberByMeasure(bar_);
		for (int i = 0; i < word_array_.size(); i++) {
			int bar_index = bar_measure +
				bms_.GetBarManager()[bar_] * i / word_array_.size();
			bms_.GetChannelManager()[channel][channel_dup_count]
				.Set(bar_index, word_array_[i]);
		}
	}
}

/* 
 * we don't need primes now, sorry!
 */
#if 0
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
#endif