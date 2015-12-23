#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef _DEBUG
#pragma comment(lib, "Debug\\bmsbel.lib")
#else
#pragma comment(lib, "Release\\bmsbel.lib")
#endif

namespace bmstest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(BMS_Unicode_Test) {
			Assert::AreEqual(true, (BmsTextFileReader::IsFileUTF8(L"..\\test\\bms\\BBKKBKK(easy)_n_Unicode.bms") == 0));
			Assert::AreEqual(false, (BmsTextFileReader::IsFileUTF8(L"..\\test\\bms\\47_LNM(TEN).bml") == 0));
		}

		TEST_METHOD(BMS_TAG_Length_Test)
		{
			// https://msdn.microsoft.com/en-us/library/hh598953.aspx

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

			std::wstringstream ss;	ss << bms.GetBMSLength() << "\n";
			Logger::WriteMessage(ss.str().c_str());
			Logger::WriteMessage(bms.GetHeaders().ToString().c_str());
			Assert::AreEqual(L"TEN [LN MASTER]", bms.GetHeaders()[L"TITLE"].c_str());
			Assert::AreEqual(L"2X / obj:ほげぇ", bms.GetHeaders()[L"ARTIST"].c_str());
			Assert::AreEqual(204507, int(bms.GetBMSLength() * 1000));
		}


		TEST_METHOD(BIG_BMS_Test) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\allnightmokugyolonglong.bms";	// this BMS should be loaded about 1 min.
			BmsParser::Parse(filename, bms, "Shift_JIS");
			bms.CalculateTimeTable();
		}

		// test various bar size
		TEST_METHOD(BIG_BMS_Test2) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\L99999999.bme";				// this BMS should be loaded about 10 secs.
			bms.Clear();
			BmsParser::Parse(filename, bms, "Shift_JIS");
			bms.CalculateTimeTable();
			Assert::AreEqual(78230, int(bms.GetBMSLength() * 1000));
		}

		TEST_METHOD(BMS_STOP_Test) {
		}

		TEST_METHOD(BMS_Note_Test) {
			BmsBms bms;
			wchar_t filename[] = L"..\\test\\bms\\BBKKBKK(easy)_n.bms";
			//wchar_t filename[] = L"..\\test\\bms\\L99999999.bme";
			//wchar_t filename[] = L"..\\test\\bms\\allnightmokugyolonglong.bms";

			// load bms file
			BmsParser::Parse(filename, bms, "Shift_JIS");

			//
			// get BPM, STOPs
			//
			double bpm;
			std::map<BmsWord, double> extended_bpm_table;
			std::map<BmsWord, int> stop_sequence_table;
			bpm = bms.GetBaseBPM();
			bms.GetBPMtable(extended_bpm_table);
			bms.GetSTOPtable(stop_sequence_table);

			//
			// process notes
			//
			/*
			double duration = 0.0;

			int start_channel_position = static_cast<int>(
				bms.GetBarManager().GetChannelPositionByBarNumber(0));
			int end_channel_position = 
				bms.GetChannelManager().GetObjectExistsMaxPosition(&BmsChannel::IsShouldPlayWavChannel);

			double triming_start_wav_position = std::numeric_limits<double>::max();
			double triming_end_wav_position = std::numeric_limits<double>::min();


			int LNtype = 0;
			if (bms.GetHeaders().IsExists("LNTYPE") &&
				NOT(BmsUtil::StringToInteger(bms.GetHeaders()["LNTYPE"], &LNtype, 10))) {
				//throw InvalidLongNoteType();
			}
			BmsWord lnobj_word(0);
			if (bms.GetHeaders().IsExists("LNOBJ") && BmsWord::CheckConstruction(bms.GetHeaders()["LNOBJ"])) {
				lnobj_word = BmsWord(bms.GetHeaders()["LNOBJ"]);
			}

			//
			// BMS is parsed like this:
			// - get all channel's word count and get LCM of it
			// - and convert words to array
			// (array size: (LCM x barNumber;999) x availableChannels)
			// - and scan each row to calculate timing
			//
			double beat = 0;
			double beat_per_each_row = 1.0 / bms.GetBarManager().GetBarDivisionCount();
			double absbeat = 0;
			for (int i = start_channel_position; i <= end_channel_position; ++i) {
				double stop_sequence_time = 0.0;
				for (BmsChannelManager::ConstIterator it = bms.GetChannelManager().Begin(); it != bms.GetChannelManager().End(); ++it) {
					BmsChannel& current_channel = *it->second;
					for (BmsChannel::ConstIterator it2 = current_channel.Begin(); it2 != current_channel.End(); it2++) {
						BmsChannelBuffer& current_buffer = **it2;
						BmsWord current_word(current_buffer[i]);
						// NOTE/LNNOTE
						if (current_channel.IsFirstPlayerNoteChannel() || current_channel.IsSecondPlayerNoteChannel() &&
							current_word != BmsWord::MIN) {
							// check if it's LNOBJ registered note
							if (current_word == lnobj_word) {
								// set previous note as LongNote
								if (NOT(prev_bmsobject[current_word.ToInteger()]) ||
									prev_bmsobject[current_word.ToInteger()]->type == BmsObject::NOTE_LN) {
									// exception: longnote was double closed
									// or there's no Longnote start position(previous note)
									throw BmsLongNoteObjectInvalidEncloseException(it->first.ToInteger());
								}
								prev_bmsobject[current_word.ToInteger()]->type = BmsObject::NOTE_LN;
								prev_bmsobject[current_word.ToInteger()]->endbeat = beat;
								prev_bmsobject[current_word.ToInteger()]->endtime = duration;
								prev_bmsobject[current_word.ToInteger()]->endvalue = current_word;
							}
							else {
								// add normal note
								bms_notes.push_back(BmsObject(BmsObject::NOTE_NORMAL, it->first, current_word.ToKey(), beat, duration, absbeat));
								prev_bmsobject[current_word.ToInteger()] = &bms_notes.back();
							}
						}
						if (current_channel.IsLongNoteChannel() && LNtype == 1 && current_word != BmsWord::MIN) {
							// start longnote ONLY IF there's no previous note, or previous note is LN.
							if (NOT(prev_bmsobject[current_word.ToInteger()]) ||
								prev_bmsobject[current_word.ToInteger()]->type == BmsObject::NOTE_LN) {
								bms_notes.push_back(BmsObject(BmsObject::NOTE_NORMAL, it->first, current_word.ToKey(), beat, duration, absbeat));
								prev_bmsobject[current_word.ToInteger()] = &bms_notes.back();
							}
							else {
								// make previous note complete
								prev_bmsobject[current_word.ToInteger()]->type = BmsObject::NOTE_LN;
								prev_bmsobject[current_word.ToInteger()]->endbeat = beat;
								prev_bmsobject[current_word.ToInteger()]->endtime = duration;
								prev_bmsobject[current_word.ToInteger()]->endvalue = current_word;
							}
						}
						// BOMB
						if (current_channel.IsBombChannel() && current_word != BmsWord::MIN) {

						}
						// BPM change
						else if (current_channel.IsBpmChangeChannel() && current_word != BmsWord::MIN) {
							int new_bpm;
							if (NOT(BmsUtil::StringToInteger(current_word.ToCharPtr(), &new_bpm, 16))) {
							//	throw InvalidFormatAsBpmChangeValueException(current_word);
							}
							bpm = static_cast<double>(new_bpm);
						}
						// extended BPM change
						else if (current_channel.IsExtendedBpmChangeChannel() && current_word != BmsWord::MIN) {
							if (extended_bpm_table.find(current_word) == extended_bpm_table.end()) {
							//	if (NOT(receiver.AtScrollObjectEntryIsEmpty(current_word, "BMP"))) {	... BPM?
							//		throw ExtendedBpmChangeEntryNotExistException(current_word);
							//	}
							}
							else {
								bpm = extended_bpm_table[current_word];
							}
						}
						// STOP sequence
						else if (current_channel.IsStopSequenceChannel() && current_word != BmsWord::MIN) {
							if (stop_sequence_table.find(current_word) == stop_sequence_table.end()) {
								//if (NOT(receiver.AtScrollObjectEntryIsEmpty(current_word, "STOP"))) {
								//	throw StopSequenceEntryNotExistException(current_word);
								//}
							}
							else {
								// 192 指定で 1 小節分停止
								double new_stop = (static_cast<double>(stop_sequence_table[current_word]) / 192.0 * 4.0) * (60.0 / bpm);
								if (stop_sequence_time == 0.0 || new_stop > stop_sequence_time) {
									stop_sequence_time = new_stop;
								}
							}
						}
					}
				}
				beat += beat_per_each_row;
				absbeat += beat_per_each_row * (LENGTH);
				duration += ((60.0 / bpm) /
					(static_cast<double>(bms.GetBarManager().GetBarDivisionCount()) / 4.0));
				duration += stop_sequence_time;
			}*/

			//
			// check longnote is end properly
			// (TODO) (but we don't have to do it strictly, maybe)
			//
		}
	};
}