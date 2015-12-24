#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\bmsbel.lib")
#else
#pragma comment(lib, "..\\Release\\bmsbel.lib")
#endif

// https://msdn.microsoft.com/en-us/library/hh598953.aspx
namespace bmstest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(BMS_Unicode_Test) {
			Assert::AreEqual(true, (BmsUtil::IsFileUTF8(L"..\\test\\bms\\BBKKBKK(easy)_n_Unicode.bms") == 0));
			Assert::AreEqual(false, (BmsUtil::IsFileUTF8(L"..\\test\\bms\\47_LNM(TEN).bml") == 0));
		}

		TEST_METHOD(BMS_TAG_Length_Test)
		{
			/* Do you want to ignore BGA channel?
			BmsRandom::RootStatement root_statement;					// for #RANDOM or some etc. commands
			BmsParser::Reactor reactor;									// handler - but don't do anything this time
			BmsParser::StartInfo info(bms, root_statement, reactor);	// contains information about parsing BMS
			info.make_syntax_tree_ = false;
			info.ignore_channel_set_.insert(BmsWord(4));
			info.ignore_channel_set_.insert(BmsWord(6));
			info.ignore_channel_set_.insert(BmsWord(7));
			info.ignore_channel_set_.insert(BmsWord(10));
			info.ignore_channel_set_.insert(BmsWord(365));
			*/

			// BMS read
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\47_LNM(TEN).bml";
			BmsParser::Parse(filename, bms, "Shift_JIS");
			bms.CalculateTimeTable();

			/*
			// wanna to ignore channel set?
			bms.GetChannelManager().DeleteChannel(BmsWord(4));
			bms.GetChannelManager().DeleteChannel(BmsWord(6));
			bms.GetChannelManager().DeleteChannel(BmsWord(7));
			bms.GetChannelManager().DeleteChannel(BmsWord(10));
			bms.GetChannelManager().DeleteChannel(BmsWord(365));
			*/

			std::wstringstream ss;	ss << bms.GetBMSLength() << "\n";
			Logger::WriteMessage(ss.str().c_str());
			Logger::WriteMessage(bms.GetHeaders().ToString().c_str());
			Assert::AreEqual(L"TEN [LN MASTER]", bms.GetHeaders()[L"TITLE"].c_str());
			Assert::AreEqual(L"2X / obj:ほげぇ", bms.GetHeaders()[L"ARTIST"].c_str());
			Assert::AreEqual(204507, int(bms.GetBMSLength() * 1000));
		}


		TEST_METHOD(BIG_BMS_Test) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\allnightmokugyolonglong.bms";	// this BMS should be loaded about 700ms (release mode)
			BmsParser::Parse(filename, bms, "Shift_JIS");
			bms.CalculateTimeTable();
		}

		// test various bar size
		TEST_METHOD(BIG_BMS_Test2) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\L99999999.bme";				// this BMS should be loaded about 700ms 
			BmsParser::Parse(filename, bms, "Shift_JIS");
			bms.CalculateTimeTable();
			Assert::AreEqual(78230, int(bms.GetBMSLength() * 1000));
		}

		TEST_METHOD(BMS_STOP_Test) {
		}

		TEST_METHOD(BMS_Note_Test) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\47_LNM(TEN).bml";
			BmsParser::Parse(filename, bms, "Shift_JIS");

			BmsNoteContainer bmsnote;
			bms.GetNotes(bmsnote);

			// previous method: 2159 notes
			// TODO new method(2combo per LN) ... we don't know yet.
			Assert::AreEqual(2159, bmsnote.GetNoteCount());
		}
	};
}