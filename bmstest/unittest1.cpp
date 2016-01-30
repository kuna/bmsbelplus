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

//
// http://hitkey.nekokan.dyndns.info/edges.htm
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
			 * - Very Large bms file (note 1024000 ?)
			 * - Extreme large measure ratio & count (256 * 1000)
			 *
			 * Note: Needs loading time about 4 sec in Release mode
			 * (previous version needs 700ms)
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\allnightmokugyolonglong.bms";	// this BMS should be loaded about 700ms (release mode)
			bms.LoadBmsFile(filename);

			BmsNoteManager note;
			bms.GetNoteData(note);
			WriteLog("NoteCount: %d", note.GetNoteCount());

			barindex lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar: %ld", lastbar);
			WriteLog("Time(sec): %.3f", bms.GetTimeManager().GetTimeFromBar(lastbar)); 

			//WriteLog("bar1 %lld", bms.GetBarManager()[1]);
			//WriteLog("bar2 %lld", bms.GetBarManager()[998]);
			//WriteLog("bar last %lld", bms.GetBarManager().GetBarNumberByMeasure(999));

			// 177 days == 15344627 secs
			Assert::AreEqual(1086912, note.GetNoteCount());
			Assert::IsTrue(15344627 == bms.GetTimeManager().GetTimeFromBar(lastbar));
		}

		TEST_METHOD(BIG_BMS_Test2) {
			/*
			 * Test part
			 * - Large bms file (note 32678)
			 * - Registheader case (#bpm01) / #BPMxx test
			 * - Measure length
			 *
			 * Note: Needs loading time about 80ms in Release mode
			 * (previous version needs 700ms)
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\L99999999.bme";				// this BMS should be loaded about 700ms 
			bms.LoadBmsFile(filename);

			BmsNoteManager note;
			bms.GetNoteData(note);
			//WriteLog("NoteCount: %d", note.GetNoteCount());
			Assert::AreEqual(32678, note.GetNoteCount());

			// bms length test
			int lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar : %d", lastbar);
			WriteLog("Time: %d", int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 1000));
			Assert::AreEqual(78260, int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 100) * 10);
		}

		TEST_METHOD(BMS_STOP_Test) {
			/* < BMS art >
			 * Test part
			 * - STOP / sudden BPM change
			 * - BMS Length test
			 * - Longnote
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\engine_XYZ.bms";
			bms.LoadBmsFile(filename);

			int lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar: %d", lastbar);
			WriteLog("Time(sec): %.3f", bms.GetTimeManager().GetTimeFromBar(lastbar));
			Assert::AreEqual(126867, int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 1000));
		}

		TEST_METHOD(BMS_Random_Test) {
			/* 
			 * Test part
			 * - RANDOM
			 * - Length (it has regular song length)
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\__litmus_slotmachine_foon.bme";
			BmsParser::Parser p(bms, { true, 0 });
			p.Load(filename);

			// notecount should vary at every test
			BmsNoteManager note;
			bms.GetNoteData(note);
			WriteLog("NoteCount: %d", note.GetNoteCount());

			int lastbar = bms.GetObjectExistsMaxBar();
			// 97.846
			WriteLog("Last Bar: %d", lastbar);
			WriteLog("Time(sec): %.3f", bms.GetTimeManager().GetTimeFromBar(lastbar));
			Assert::AreEqual(97846, int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 1000));
			Assert::AreEqual(727, note.GetNoteCount());
		}

		TEST_METHOD(BMS_Nested_Random_Test) {
			/*
			* Test part
			* - Very Big Bms (8MiB)
			* - Nested Random
			*/
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\ubmchallenge.bms";
			bms.LoadBmsFile(filename);

			int lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar: %d", lastbar);
			WriteLog("Time(sec): %.3f", bms.GetTimeManager().GetTimeFromBar(lastbar));
		}

		TEST_METHOD(BMS_Decimal_Test) {
			/*
			 * Test part
			 * - Fractional Bpm / Total
			 * - Bar position test
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\Uni_birth_h.bms";
			bms.LoadBmsFile(filename);

			int lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar: %d", lastbar);
			WriteLog("Base BPM: %f", bms.GetBaseBPM());
			WriteLog("Time(sec): %.3f", bms.GetTimeManager().GetTimeFromBar(lastbar));
			Assert::AreEqual(88764, int(bms.GetTimeManager().GetTimeFromBar(lastbar) * 1000));
			Assert::AreEqual(143.3, bms.GetBaseBPM());
			WriteLog("Bar of 0s: %.1f", bms.GetTimeManager().GetBarFromTime(0));
			WriteLog("Bar of 10s: %.1f", bms.GetTimeManager().GetBarFromTime(10));
			WriteLog("Bar of 50s: %.1f", bms.GetTimeManager().GetBarFromTime(50));
			WriteLog("Bar of 60s: %.1f", bms.GetTimeManager().GetBarFromTime(60));
			WriteLog("Bar of 90s: %.1f", bms.GetTimeManager().GetBarFromTime(90));
			WriteLog("Bar of 100s: %.1f", bms.GetTimeManager().GetBarFromTime(100));
		}

		TEST_METHOD(BMS_Key_Test) {
			/*
			 * Test part
			 * - 5 / 7 / 9 / 14k
			 */
			BmsBms bms;
			bms.LoadBmsFile(L"..\\test\\bms\\22_brain_5.bms");
			Assert::AreEqual(5, bms.GetKey());
			bms.LoadBmsFile(L"..\\test\\bms\\01_brain_7n.bme");
			Assert::AreEqual(7, bms.GetKey());
			// (TODO)
			// PMS's channel construction is a little different;
			// - If extension is BME, then it's BME type pms
			// - If extension is PMS, then it's always pms
			// may need some distingishing variable.
			//bms.LoadBmsFile(L"..\\test\\bms\\_9keyN.pms");
			//Assert::AreEqual(9, bms.GetKey());
			bms.LoadBmsFile(L"..\\test\\bms\\_DPN.bms");
			Assert::AreEqual(14, bms.GetKey());
		}

		TEST_METHOD(BMS_Save_Test) {
			/*
			 * Test part
			 * - Save
			 * (COMMENT: if you want to save withou syntax being parsed, 
			    you should get syntax line from parser and append it to this file.)
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\engine_XYZ.bms";
			wchar_t filename_out[] = L"..\\test\\bms\\engine_out.bms";
			bms.LoadBmsFile(filename);
			bms.SaveBmsFile(filename_out);
		}

		TEST_METHOD(BMS_Negative_BPM) {
			/*
			 * Test part
			 * - Negative BPM
			 * - undefined STOP(FD)
			 * - setrandom
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\u9.bml";
			bms.LoadBmsFile(filename);

			int lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar: %d", lastbar);
			WriteLog("Time(sec): %.3f", bms.GetTimeManager().GetTimeFromBar(lastbar));
			WriteLog("Bar of 10s: %.1f", bms.GetTimeManager().GetBarFromTime(10));
			WriteLog("Bar of 50s: %.1f", bms.GetTimeManager().GetBarFromTime(50));
			WriteLog("Bar of 60s: %.1f", bms.GetTimeManager().GetBarFromTime(60));
			WriteLog("Bar of 70s: %.1f", bms.GetTimeManager().GetBarFromTime(70));
			WriteLog("Bar of 80s: %.1f", bms.GetTimeManager().GetBarFromTime(80));
			WriteLog("Bar of 100s: %.1f", bms.GetTimeManager().GetBarFromTime(100));
		}


#if 0	// thinking of test case, but yet...
		TEST_METHOD(BMS_Note_Test) {
			/*
			 * Test part
			 * - RANDOM
			 * - Training mode
			 * - S-RANDOM
			 */
		}

		TEST_METHOD(BMS_LNTYPE2) {
			/*
			 * Test part
			 * - LNTYPE 2
			 * - Note count (hellcharge)
			 */
		}
#endif

		TEST_METHOD(BMS_Switch_Test) {
			/*
			 * Test part
			 * - SWITCH ~ ENDSW part
			 * (look carefully with note count & end time)
			 */
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\switch_test.bms";
			bms.LoadBmsFile(filename);

			BmsNoteManager bmsnote;
			bms.GetNoteData(bmsnote);

			WriteLog("Note count: %d", bmsnote.GetNoteCount());
			int lastbar = bms.GetObjectExistsMaxBar();
			WriteLog("Last Bar: %d", lastbar);
			WriteLog("Time(sec): %.3f", bms.GetTimeManager().GetTimeFromBar(lastbar));
			Assert::AreEqual(4, bmsnote.GetNoteCount());
		}

		TEST_METHOD(BMS_Note_Test) {
			/*
			 * Test part
			 * - Longnote count
			 */
			// bms note count test
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\47_LNM(TEN).bml";
			bms.LoadBmsFile(filename);

			BmsNoteManager bmsnote;
			bms.GetNoteData(bmsnote);

			// previous method: 2159 notes
			// new method (count LNEND): 2971 notes
			//Assert::AreEqual(2159, bmsnote.GetNoteCount(false, false));
			Assert::AreEqual(2971, bmsnote.GetNoteCount());
			WriteLog("Note count: %d", bmsnote.GetNoteCount());
		}
	};
}