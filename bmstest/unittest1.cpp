#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "bmsbel\bms_bms.h"

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
			bms.LoadBmsFile(filename);

			// check metadata / headers
			Logger::WriteMessage(bms.GetHeaders().ToWString().c_str());
			Assert::AreEqual(L"TEN [LN MASTER]", bms.GetHeaders()["TITLE"].ToWString().c_str());
			Assert::AreEqual(L"2X / obj:ほげぇ", bms.GetHeaders()["ARTIST"].ToWString().c_str());

			// check song end time
			int lastbar = bms.GetObjectExistsMaxBar();
			Assert::AreEqual(204507, int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 1000));
		}


		TEST_METHOD(BIG_BMS_Test) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\allnightmokugyolonglong.bms";	// this BMS should be loaded about 700ms (release mode)
			bms.LoadBmsFile(filename);
		}

		// test various bar size
		TEST_METHOD(BIG_BMS_Test2) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\L99999999.bme";				// this BMS should be loaded about 700ms 
			bms.LoadBmsFile(filename);

			// bms length test
			int lastbar = bms.GetObjectExistsMaxBar();
			Assert::AreEqual(78230, int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 1000));
		}

		TEST_METHOD(BMS_STOP_Test) {
			// bms length test
		}

		TEST_METHOD(BMS_Nested_Random_Test) {
		}

		TEST_METHOD(BMS_Note_Test) {
			// bms note count test
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\47_LNM(TEN).bml";
			bms.LoadBmsFile(filename);

			BmsNoteManager bmsnote;
			bms.GetNoteData(bmsnote);

			// previous method: 2159 notes
			// TODO new method(2combo per LN) ... we don't prepared about this.
			//Assert::AreEqual(2159, bmsnote.GetNoteCount());
			char buf[1024];	sprintf(buf, "%d", bmsnote.GetNoteCount());
			Logger::WriteMessage(buf);
		}

		/*
		 * STP test
		 * RANDOM test (song length)
		 * 
		 */
	};
}