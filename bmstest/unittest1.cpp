#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <stdarg.h>
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
	char buf[1024];
	void WriteLog(const char* t, ...) {
		va_list vl;
		va_start(vl, t);
		vsprintf_s(buf, t, vl);
		Logger::WriteMessage(buf);
	}

	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(BMS_Unicode_Test) {
			Assert::AreEqual(1, BmsUtil::IsFileUTF8("..\\test\\bms\\BBKKBKK(easy)_n_Unicode.bms"));
			Assert::AreEqual(0, BmsUtil::IsFileUTF8("..\\test\\bms\\47_LNM(TEN).bml"));
		}

		TEST_METHOD(BMS_TAG_Length_Test)
		{
			/*
			* Test part
			* - Longnote (#LNTYPE 1)
			* - Header / Unicode (UTF-8)
			*/
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
			/*
			 * Test part
			 * - Very Large bms file (note 32xxx)
			 *
			 * Note: Needs loading time about 3 sec in Release mode
			 * (previous version needs 700ms)
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\allnightmokugyolonglong.bms";	// this BMS should be loaded about 700ms (release mode)
			bms.LoadBmsFile(filename);

			BmsNoteManager note;
			bms.GetNoteData(note);
			WriteLog("NoteCount: %d", note.GetNoteCount());
		}

		// test various bar size
		TEST_METHOD(BIG_BMS_Test2) {
			/*
			 * Test part
			 * - Large bms file (note 32xxx)
			 * - Registheader case (#bpm01) / #BPMxx test
			 * - Measure length
			 *
			 * Note: Needs loading time about 70ms in Release mode
			 * (previous version needs 700ms)
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\L99999999.bme";				// this BMS should be loaded about 700ms 
			bms.LoadBmsFile(filename);

			// bms length test
			int lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar : %d", lastbar);
			Assert::AreEqual(78270, int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 100) * 10);
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
			WriteLog("Note count: %d", bmsnote.GetNoteCount());
		}

		/*
		 * STP test
		 * RANDOM test (song length)
		 * 
		 */
	};
}