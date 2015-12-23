#ifndef BMSBEL_PARSER_H_
#define BMSBEL_PARSER_H_

#include <vector>
#include <stack>
#include <queue>
#include <set>

#include "bms_define.h"
#include "bms_bms.h"
#include "bms_random.h"
#include "bms_exception.h"
#include "bms_parser_frame.h"
#include "bms_text_file_reader.h"

namespace BmsParser {
	class Parser;
	class Reactor;

	// -- StartInfo --------------------------------------------------------
	// contains data for basic frame
	struct StartInfo {
	public:
		StartInfo(BmsBms&                   bms,
			BmsRandom::RootStatement& root_statement,
			Reactor&                  reactor);
	public:
		BmsBms&                   bms_;
		BmsRandom::RootStatement& root_statement_;
		Reactor&                  reactor_;
		bool                      make_syntax_tree_;
		std::set<BmsWord>         ignore_channel_set_;
	};

	// -- ParsingInfo ------------------------------------------------------
	// contains data for each frame (if~ clause syntax)
	struct ParsingInfo {
	public:
		ParsingInfo(StartInfo&  start_info,
			FrameStack& frame_stack);

	public:
		BmsBms&                   bms_;
		BmsRandom::RootStatement& root_statement_;
		Reactor&                  reactor_;
		bool                      make_syntax_tree_;
		std::set<BmsWord>         ignore_channel_set_;
		std::vector<std::wstring> line_data_;
		FrameStack&               frame_stack_;
		std::queue<unsigned int>  random_value_queue_;
		bool                      is_first_parse_;
		unsigned int              line_number_;
		unsigned int              bar_;
		BmsWord                   channel_number_;
		std::wstring              key_;
		std::wstring              value_;
		double                    bar_change_ratio_;
		std::vector<BmsWord>      word_array_;
	};

	// -- Reactor ----------------------------------------------------------
	// kind of handler
	class Reactor {
	public:
		static Reactor DefaultObject;

	public:
		explicit Reactor(void);
		virtual ~Reactor();

		virtual std::wstring AtDuplicateHeader(Parser& parser,
			const std::wstring header,
			const std::wstring before,
			const std::wstring present);
		virtual double AtDuplicateBarChange(Parser& parser,
			unsigned int bar,
			double       before,
			double       present);
		virtual bool AtParseError(Parser& parser, BmsParseException& e, const std::wstring& line);
		virtual unsigned int AtGenerateRandom(Parser& parser, unsigned int max);

		virtual void AtLineDataRead(Parser& parser);
		virtual void AtParseStart(Parser& parser);
		virtual void AtLineEnd(Parser& parser);
		virtual void AtParseEnd(Parser& parser);

	private:
		unsigned int random_value_;
	};

	// -- Parser -----------------------------------------------------------
	void Parse(const std::wstring& filename, BmsBms& bms);
	void Parse(const std::wstring& filename, BmsBms& bms, const char *encoding);
	void Parse(const std::wstring& filename, const char *encoding, StartInfo& start_info);
	void Parse(BmsTextFileReader& reader, StartInfo& start_info);

	class Parser {
		friend void Parse(BmsTextFileReader& reader, StartInfo& start_info);

	public:
		ParsingInfo& GetInfo(void);

	private:
		explicit Parser(StartInfo& start_info, FrameStack& frame_stack);
		void Start(void);

		void ParseDefault(const wchar_t* str);
		void ParseComment(const wchar_t* str);
		void ParseHeaderOrBar(const wchar_t* str);
		void ParseHeaderKey(const wchar_t* str);
		void ParseHeaderSeparator(const wchar_t* str);
		void ParseHeaderValue(const wchar_t* str);
		void WriteDownHeader(const FrameData& data);
		void WriteDownRegistArray(const FrameData& data);
		void ParseBar(const wchar_t* str);
		void ParseChannel(const wchar_t* str);
		void ParseColon(const wchar_t* str);
		void ParseBarChangeValue(const wchar_t* str);
		void WriteDownBarChangeValue(const FrameData& data);
		void ParseObjectArray(const wchar_t* str);
		void WriteDownObjectArray(const FrameData& data);
		void ParseRandomStatement(void);

		unsigned int DecideBarDivisionCountFromBarChange(unsigned int current, double bar_change_ratio);
		unsigned int DecideBarDivisionCountFromObjectArray(unsigned int current, double bar_change_ratio);

	private:
		ParsingInfo info_;
	};
}

#endif // BMSBEL_PARSER_H_
