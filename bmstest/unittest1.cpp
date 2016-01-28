#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\bmsbel.lib")
#else
#pragma comment(lib, "..\\Release\\bmsbel.lib")
#endif

// https://msdn.microsoft.com/en-us/library/hh598953.aspx
/*
// wanna to ignore channel set?
bms.GetChannelManager().DeleteChannel(BmsWord(4));
bms.GetChannelManager().DeleteChannel(BmsWord(6));
bms.GetChannelManager().DeleteChannel(BmsWord(7));
bms.GetChannelManager().DeleteChannel(BmsWord(10));
bms.GetChannelManager().DeleteChannel(BmsWord(365));
*/

namespace bmstest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(BMS_Unicode_Test) {
			Assert::AreEqual(1, BmsUtil::IsFileUTF8("..\\test\\bms\\BBKKBKK(easy)_n_Unicode.bms"));
			Assert::AreEqual(0, BmsUtil::IsFileUTF8("..\\test\\bms\\47_LNM(TEN).bml"));
		}

		TEST_METHOD(BMS_TAG_Length_Test)
		{
			// BMS read
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\47_LNM(TEN).bml";
			BmsParser::Parse(filename, bms, "Shift_JIS");
			BmsTimeManager timetable;
			bms.CalculateTime(timetable);


			Logger::WriteMessage(bms.GetHeaders().ToWString().c_str());
			Assert::AreEqual(L"TEN [LN MASTER]", bms.GetHeaders()["TITLE"].ToWString().c_str());
			Assert::AreEqual(L"2X / obj:ほげぇ", bms.GetHeaders()["ARTIST"].ToWString().c_str());
			Assert::AreEqual(204507, int(timetable.GetEndTime() * 1000));
		}


		TEST_METHOD(BIG_BMS_Test) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\allnightmokugyolonglong.bms";	// this BMS should be loaded about 700ms (release mode)
			BmsParser::Parse(filename, bms, "Shift_JIS");
		}

		// test various bar size
		TEST_METHOD(BIG_BMS_Test2) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\L99999999.bme";				// this BMS should be loaded about 700ms 
			BmsParser::Parse(filename, bms, "Shift_JIS");

			BmsTimeManager timetable;
			bms.CalculateTime(timetable);
			Assert::AreEqual(78230, int(timetable.GetEndTime() * 1000));
		}

		TEST_METHOD(BMS_STOP_Test) {
		}

		TEST_METHOD(BMS_Nested_Random_Test) {
		}

		TEST_METHOD(BMS_Note_Test) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\47_LNM(TEN).bml";
			BmsParser::Parse(filename, bms, "Shift_JIS");

			BmsNoteContainer bmsnote;
			bms.GetNotes(bmsnote);

			// previous method: 2159 notes
			// TODO new method(2combo per LN) ... we don't prepared about this.
			Assert::AreEqual(2159, bmsnote.GetNoteCount());
		}
	};
}