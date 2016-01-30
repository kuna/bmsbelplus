#ifndef BMSBEL_PARSER_H_
#define BMSBEL_PARSER_H_

#include <vector>
#include <queue>
#include <sstream>

#include "bms_define.h"

/*
 * isn't it too much complex?
 */
class BmsBms;

namespace BmsParser {
	// -- ParsingInfo ------------------------------------------------------
	// more detail options for parsing
	struct ParsingInfo {
		/** @brief decide to parse RANDOM clause or not. */
		bool			make_syntax_tree_;
		/** @brief seed value when generate RANDOM clause */
		unsigned int	seed_;
	};

	// -- Parser -----------------------------------------------------------
	// only for Bms parser, currently.
	// doesn't parses comment.
	class Parser {
	public:
		Parser(BmsBms &bms);
		Parser(BmsBms &bms, ParsingInfo _info);

#ifdef USE_MBCS
		bool Load(const wchar_t* path);
#endif
		bool Load(const char* path);
		bool Parse(const char* text);

		std::string GetLog();
		// automatically called after parsing
		void Clear();

	private:
		// start of the parsing
		bool ParseLines();
		// comment parsing (this function isn't actually called)
		void ParseComment(const char* str);
		// random statement parsing
		bool IsRandomStatement(const char* str);
		void ProcessRandomStatement();
		// measure / header / regist parsing
		void ParseMeasure(const char* str);
		void ParseHeaderOrBar(const char* str);
		void ParseHeaderKey(const char* str);
		void ParseHeaderValue(const char* str);
		// object parsing
		void ParseObjectArray(const char* str);

		void WriteLogLine(const char* t, ...);

	private:
		//
		// basic parsing attribute / parsed result
		//
		ParsingInfo					info_;
		BmsBms&						bms_;
		std::vector<std::string>	syntax_line_data_;
	private:
		//
		// should be cleared after parsing
		//
		int							line_;
		std::vector<std::string>	line_data_;
		std::string					key_, value_;
		std::ostringstream			log_;
		unsigned int				bgm_dup_count[BmsConst::BAR_MAX_COUNT];
		//
		// check random statement
		//
		std::deque<std::string>		syntax_tag_;
		//
		// panic?
		//
		bool						panic_;
	};
}

#endif // BMSBEL_PARSER_H_
