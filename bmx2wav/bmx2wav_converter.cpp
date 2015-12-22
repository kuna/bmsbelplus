#include <string>
#include <vector>
#include <map>

#include "tt_path.h"
#include "tt_util.h"
#include "tt_process.h"

#include "bms_word.h"
#include "bms_bms.h"
#include "bms_parser.h"
#include "bms_exception.h"

#include "bmx2wav_exception.h"
#include "bmx2wav_wav_maker.h"
#include "bmx2wav_template_path_converter.h"

#include "bmx2wav_converter.h"

using namespace Bmx2Wav;

// -- ProgressReceiver -------------------------------------------------
Converter::ProgressReceiver
Converter::ProgressReceiver::DefaultObject;

Converter::ProgressReceiver::ProgressReceiver(void)
{
}

Converter::ProgressReceiver::~ProgressReceiver()
{
}

void
Converter::ProgressReceiver::AtConvertStart(void)
{
}

BmsParser::Reactor&
Converter::ProgressReceiver::GetBmsParserReactor(void)
{
	return BmsParser::Reactor::DefaultObject;
}

void
Converter::ProgressReceiver::AtBmsParseStart(const std::string& filename)
{
	NOT_USE_VAR(filename);
}

void
Converter::ProgressReceiver::AtBmsParseEnd(const std::string& filename, BmsBms& bms)
{
	NOT_USE_VAR(filename);
	NOT_USE_VAR(bms);
}

WavMaker::Reactor&
Converter::ProgressReceiver::GetWavMakerReactor(void)
{
	return WavMaker::Reactor::DefaultObject;
}

void
Converter::ProgressReceiver::AtWavFileReadStart(unsigned int file_count)
{
	NOT_USE_VAR(file_count);
}

void
Converter::ProgressReceiver::AtOneWavFileReadStart(BmsWord word, const std::string& filename)
{
	NOT_USE_VAR(word);
	NOT_USE_VAR(filename);
}

bool
Converter::ProgressReceiver::AtWavFileNotExists(BmsWord word, const std::string& filename, bool wav_was_used)
{
	NOT_USE_VAR(word);
	NOT_USE_VAR(filename);
	NOT_USE_VAR(wav_was_used);
	return false;
}

bool
Converter::ProgressReceiver::AtWavFileFormatError(BmsWord word, InvalidWavFileFormatException& e)
{
	NOT_USE_VAR(word);
	NOT_USE_VAR(e);
	return false;
}

bool
Converter::ProgressReceiver::AtWavFileError(BmsWord word, WavFileError& e)
{
	NOT_USE_VAR(word);
	NOT_USE_VAR(e);
	return false;
}

void
Converter::ProgressReceiver::AtOneWavFileReadEnd(BmsWord word, const std::string& filename)
{
	NOT_USE_VAR(word);
	NOT_USE_VAR(filename);
}

void
Converter::ProgressReceiver::AtWavFileReadEnd(unsigned int file_count)
{
	NOT_USE_VAR(file_count);
}

bool
Converter::ProgressReceiver::AtScrollObjectDefineError(BmsWord word, const std::string& name, const std::string& value)
{
	NOT_USE_VAR(word);
	NOT_USE_VAR(name);
	NOT_USE_VAR(value);
	return false;
}

void
Converter::ProgressReceiver::AtMixStart(unsigned int start_position, unsigned int end_position)
{
	NOT_USE_VAR(start_position);
	NOT_USE_VAR(end_position);
}

bool
Converter::ProgressReceiver::AtScrollObjectEntryIsEmpty(BmsWord word, const std::string& name)
{
	NOT_USE_VAR(word);
	NOT_USE_VAR(name);
	return false;
}

void
Converter::ProgressReceiver::AtMixProgress(unsigned int current_position)
{
	NOT_USE_VAR(current_position);
}

void
Converter::ProgressReceiver::AtMixEnd(unsigned int end_position)
{
	NOT_USE_VAR(end_position);
}

void
Converter::ProgressReceiver::AtNormalizeStart(void)
{
}

void
Converter::ProgressReceiver::AtNormalizeEnd(double change_ratio)
{
	NOT_USE_VAR(change_ratio);
}

void
Converter::ProgressReceiver::AtCompressStart(double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead)
{
	NOT_USE_VAR(threshold);
	NOT_USE_VAR(ratio);
	NOT_USE_VAR(attack_time);
	NOT_USE_VAR(release_time);
	NOT_USE_VAR(look_ahead);
}

void
Converter::ProgressReceiver::AtCompressEnd(double threshold, double ratio, unsigned int attack_time, unsigned int release_time, int look_ahead)
{
	NOT_USE_VAR(threshold);
	NOT_USE_VAR(ratio);
	NOT_USE_VAR(attack_time);
	NOT_USE_VAR(release_time);
	NOT_USE_VAR(look_ahead);
}

void
Converter::ProgressReceiver::AtVolumeChangeStart(int percentage)
{
	NOT_USE_VAR(percentage);
}

void
Converter::ProgressReceiver::AtVolumeChangeEnd(int percentage)
{
	NOT_USE_VAR(percentage);
}

void
Converter::ProgressReceiver::AtWriteToFileStart(const std::string& filename)
{
	NOT_USE_VAR(filename);
}

void
Converter::ProgressReceiver::AtWriteToFileEnd(const std::string& filename)
{
	NOT_USE_VAR(filename);
}

void
Converter::ProgressReceiver::AtAfterProcessStart(TtProcess::CreateInfo& info)
{
	NOT_USE_VAR(info);
}

void
Converter::ProgressReceiver::AtCreatedProcess(TtProcess& process, TtProcess::CreateInfo& info)
{
	NOT_USE_VAR(process);
	NOT_USE_VAR(info);
}

void
Converter::ProgressReceiver::AtWaitForProcessStart(TtProcess& process)
{
	NOT_USE_VAR(process);
}

void
Converter::ProgressReceiver::AtWaitForProcessEnd(TtProcess& process)
{
	NOT_USE_VAR(process);
}

void
Converter::ProgressReceiver::AtDeleteFileStart(const std::string& filename)
{
	NOT_USE_VAR(filename);
}

void
Converter::ProgressReceiver::AtDeleteFileEnd(const std::string& filename, bool result)
{
	NOT_USE_VAR(filename);
	NOT_USE_VAR(result);
}

void
Converter::ProgressReceiver::AtAfterProcessEnd(TtProcess& process, TtProcess::CreateInfo& info, const std::string& outputed_filename, BmsBms& bms)
{
	NOT_USE_VAR(process);
	NOT_USE_VAR(info);
	NOT_USE_VAR(outputed_filename);
	NOT_USE_VAR(bms);

}

void
Converter::ProgressReceiver::AtFailStartProcess(TtProcess::CreateInfo& info)
{
	NOT_USE_VAR(info);
}

void
Converter::ProgressReceiver::AtConvertEnd(void)
{
}

// -- Converter --------------------------------------------------------
namespace {
	class WavTable {
	public:
		explicit WavTable(WavMaker& maker) : table_(), maker_(maker) {}

		~WavTable() {
			for (std::map<BmsWord, HQWav*>::iterator it = table_.begin(); it != table_.end(); ++it) {
				delete it->second;
			}
		}

		void MakeNewWav(BmsWord pos, const std::string& filename, bool overlook_error) {
			try {
				HQWav* tmp = maker_.MakeNewWavFromWavFile(filename, overlook_error);
				table_[pos] = tmp;
			}
			catch (TtFileOpenException& e) {
				throw EntriedWavFileNotFoundException(pos, filename);
			}
		}

		void MakeNewWavFromOgg(BmsWord pos, const std::string& filename) {
			try {
				HQWav* tmp = maker_.MakeNewWavFromOggFile(filename);
				table_[pos] = tmp;
			}
			catch (TtFileOpenException& e) {
				throw EntriedWavFileNotFoundException(pos, filename);
			}
		}

		void MakeNewWavEmpty(BmsWord pos) {
			HQWav* tmp = new HQWav();
			table_[pos] = tmp;
		}

		unsigned int GetCount(void) const {
			return table_.size();
		}

		bool IsExist(BmsWord pos) const {
			return table_.find(pos) != table_.end();
		}

		bool IsNotExist(BmsWord pos) const {
			return NOT(this->IsExist(pos));
		}

		HQWav& operator [](BmsWord pos) {
			return *table_[pos];
		}

	private:
		std::map<BmsWord, HQWav*> table_;
		WavMaker&                 maker_;
	};
}

ParameterSet&
Converter::GetParameterSet(void)
{
	return parameter_set_;
}


void
Converter::SetProgressReceiver(ProgressReceiver* receiver)
{
	receiver_ = receiver;
}


void
Converter::Convert(void)
{
	ProgressReceiver& receiver = receiver_ != NULL ? *receiver_ : ProgressReceiver::DefaultObject;
	receiver.AtConvertStart();

	// パラメタ整合性チェック
	if (parameter_set_.convert_start_bar_number_ > parameter_set_.convert_end_bar_number_) {
		throw InvalidConvertBarException(parameter_set_.convert_start_bar_number_, parameter_set_.convert_end_bar_number_);
	}
	if (parameter_set_.triming_start_bar_number_ > parameter_set_.triming_end_bar_number_) {
		throw InvalidTrimingBarException(parameter_set_.triming_start_bar_number_, parameter_set_.triming_end_bar_number_);
	}

	BmsBms bms;
	BmsRandom::RootStatement root_statement;
	BmsParser::StartInfo info(bms, root_statement, receiver.GetBmsParserReactor());
	info.make_syntax_tree_ = false;

	if (parameter_set_.ignore_bga_channel_) {
		info.ignore_channel_set_.insert(BmsWord(4));
		info.ignore_channel_set_.insert(BmsWord(6));
		info.ignore_channel_set_.insert(BmsWord(7));
		info.ignore_channel_set_.insert(BmsWord(10));
		info.ignore_channel_set_.insert(BmsWord(365));
	}

	// BMS 解析処理
	receiver.AtBmsParseStart(parameter_set_.input_filename_);
	BmsParser::Parse(parameter_set_.input_filename_, info);
	receiver.AtBmsParseEnd(parameter_set_.input_filename_, bms);

	// ロングノート関連の前処理
	for (BmsChannelManager::ConstIterator it = bms.GetChannelManager().Begin(); it != bms.GetChannelManager().End(); ++it) {
		BmsChannel& current_channel = *it->second;
		if (current_channel.IsLongNoteChannel()) {
			for (BmsChannel::ConstIterator current_buffer = current_channel.Begin(); current_buffer != current_channel.End(); ++current_buffer) {
				bool on_note = false;
				unsigned int position = 0;
				BmsWord on_note_value = BmsWord::MIN;
				for (BmsBuffer::Iterator obj = (*current_buffer)->Begin(); obj != (*current_buffer)->End(); ++obj) {
					if (*obj != BmsWord::MIN) {
						if (on_note) {
							if (parameter_set_.different_long_note_enclosed_object_is_error_ && *obj != on_note_value) {
								throw LongNoteObjectInvalidEncloseException(on_note_value, *obj, bms.GetBarManager().GetBarNumberByChannelPosition(position));
							}
							obj->SetByWord(BmsWord::MIN);
							on_note = false;
						}
						else {
							on_note = true;
							on_note_value = *obj;
						}
					}
					position += 1;
				}
				if (on_note) {
					throw LongNoteObjectNotEnclosedException();
				}
			}
		}
		if (parameter_set_.not_sound_object_specified_by_lnobj_ &&
			(current_channel.IsFirstPlayerNoteChannel() || current_channel.IsSecondPlayerNoteChannel()) &&
			bms.GetHeaders().IsExists("LNOBJ") && BmsWord::CheckConstruction(bms.GetHeaders()["LNOBJ"])) {
			BmsWord lnobj_word(bms.GetHeaders()["LNOBJ"]);
			for (BmsChannel::ConstIterator current_buffer = current_channel.Begin(); current_buffer != current_channel.End(); ++current_buffer) {
				bool on_note = false;
				for (BmsBuffer::Iterator obj = (*current_buffer)->Begin(); obj != (*current_buffer)->End(); ++obj) {
					if (*obj == lnobj_word && on_note) {
						obj->SetByWord(BmsWord::MIN);
						on_note = false;
					}
					else if (*obj != BmsWord::MIN) {
						on_note = true;
					}
				}
			}
		}
	}

	// BMS ファイルのディレクトリ取得
	std::string bms_dir(TtPath::EditPath(parameter_set_.input_filename_, true, true, false, false));

	// 検証処理
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		BmsWord word(i);
		if (bms.GetRegistArraySet()["WAV"].IsExists(word) && parameter_set_.check_wav_file_exists_) {
			std::string path(bms_dir + bms.GetRegistArraySet()["WAV"][word]);
			std::string ogg_path = TtPath::ChangeExtension(path, "ogg");
			if (NOT(TtPath::FileExists(path)) && NOT(TtPath::FileExists(ogg_path))) {
				throw EntriedWavFileNotFoundException(word, path);
			}
		}
		if (bms.GetRegistArraySet()["BMP"].IsExists(word) && parameter_set_.check_bmp_file_exists_) {
			std::string path(bms_dir + bms.GetRegistArraySet()["BMP"][word]);
			std::string png_path = TtPath::ChangeExtension(path, "png");
			if (NOT(TtPath::FileExists(path)) && NOT(TtPath::FileExists(png_path))) {
				throw EntriedBmpFileNotFoundException(word, path);
			}
		}

		if (bms.GetRegistArraySet()["WAV"].IsExists(word) && parameter_set_.check_wav_used_) {
			if (NOT(bms.GetChannelManager().Contains(word, BmsChannel::IsWavChannel))) {
				throw EntriedRegistArrayValueNotUsedException("WAV", word);
			}
		}
		if (bms.GetRegistArraySet()["BMP"].IsExists(word) && parameter_set_.check_bmp_used_) {
			if (NOT(bms.GetChannelManager().Contains(word, BmsChannel::IsBmpChannel))) {
				throw EntriedRegistArrayValueNotUsedException("BMP", word);
			}
		}
		if (bms.GetRegistArraySet()["BPM"].IsExists(word) && parameter_set_.check_bpm_used_) {
			if (NOT(bms.GetChannelManager().Contains(word, BmsChannel::IsExtendedBpmChangeChannel))) {
				throw EntriedRegistArrayValueNotUsedException("BPM", word);
			}
		}
		if (bms.GetRegistArraySet()["STOP"].IsExists(word) && parameter_set_.check_stop_used_) {
			if (NOT(bms.GetChannelManager().Contains(word, BmsChannel::IsStopSequenceChannel))) {
				throw EntriedRegistArrayValueNotUsedException("STOP", word);
			}
		}
	}
	if (parameter_set_.check_stage_file_exists_ && bms.GetHeaders().IsExists("STAGEFILE")) {
		std::string path(bms_dir + bms.GetHeaders()["STAGEFILE"]);
		if (NOT(TtPath::FileExists(path))) {
			throw EntriedStageFileNotFoundException(path);
		}
	}

	// WAV の読み込み
	WavMaker maker(receiver.GetWavMakerReactor(), parameter_set_.not_use_low_pass_filter_);
	WavTable wav_table(maker);
	std::vector<unsigned int> last_used_wav_pos(BmsConst::WORD_MAX_COUNT, 0);
	receiver.AtWavFileReadStart(bms.GetRegistArraySet()["WAV"].GetExistCount());
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
		BmsWord word(i);
		if (bms.GetRegistArraySet()["WAV"].IsExists(word)) {
			std::string path(bms_dir + bms.GetRegistArraySet()["WAV"][word]);
			std::string ogg_path = TtPath::EditPath(path, true, true, true, false) + ".ogg";

			if (path == parameter_set_.output_filename_) {
				throw OutputFileIsInputWavFileException(word, path);
			}

			bool path_exists = TtPath::FileExists(path);
			bool ogg_path_exists = TtPath::FileExists(ogg_path);

			if (NOT(path_exists) && NOT(ogg_path_exists)) {
				if (NOT(receiver.AtWavFileNotExists(
					word, path, bms.GetChannelManager().Contains(word, BmsChannel::IsWavChannel)))) {
					throw EntriedWavFileNotFoundException(word, path);
				}
				wav_table.MakeNewWavEmpty(word);
			}
			else {
				try {
					if (path_exists && TtPath::FindExtension(path) != ".ogg") {
						try {
							receiver.AtOneWavFileReadStart(word, path);
							wav_table.MakeNewWav(word, path, false);
							receiver.AtOneWavFileReadEnd(word, path);
						}
						catch (InvalidWavFileFormatException& e) {
							if (NOT(receiver.AtWavFileFormatError(word, e))) {
								throw;
							}
							receiver.AtOneWavFileReadStart(word, path);
							wav_table.MakeNewWav(word, path, true);
							receiver.AtOneWavFileReadEnd(word, path);
						}
					}
					else if (ogg_path_exists) {
						receiver.AtOneWavFileReadStart(word, ogg_path);
						wav_table.MakeNewWavFromOgg(word, ogg_path);
						receiver.AtOneWavFileReadEnd(word, ogg_path);
					}
					else {
						throw BMX2WAV_INTERNAL_EXCEPTION;
					}
				}
				catch (WavFileError& e) {
					if (NOT(receiver.AtWavFileError(word, e))) {
						throw;
					}
					wav_table.MakeNewWavEmpty(word);
				}
			}

			last_used_wav_pos[i] = -1 * static_cast<int>(wav_table[word].GetLength());
		}
	}
	receiver.AtWavFileReadEnd(bms.GetRegistArraySet()["WAV"].GetExistCount());

	// BPM の読み込み
	double bpm;
	if (bms.GetHeaders().IsExists("BPM")) {
		if (NOT(TtUtil::StringToDouble(bms.GetHeaders()["BPM"], &bpm)) || bpm == 0.0) {
			throw InvalidFormatAsBpmHeaderException();
		}
	}
	else {
		bpm = 120.0;
	}

	// 拡張 BPM, STOP sequence の読み込み
	std::map<BmsWord, double> extended_bpm_table;
	std::map<BmsWord, int> stop_sequence_table;
	for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; i++) {
		BmsWord current_word(i);
		if (bms.GetRegistArraySet()["BPM"].IsExists(current_word)) {
			double tmp;
			if (NOT(TtUtil::StringToDouble(bms.GetRegistArraySet()["BPM"][current_word], &tmp)) || tmp == 0.0) {
				if (NOT(receiver.AtScrollObjectDefineError(current_word, "BPM", bms.GetRegistArraySet()["BPM"][current_word]))) {
					throw InvalidFormatAsExtendedBpmChangeValueException(current_word, bms.GetRegistArraySet()["BPM"][current_word]);
				}
			}
			else {
				extended_bpm_table[current_word] = tmp;
			}
		}
		if (bms.GetRegistArraySet()["STOP"].IsExists(BmsWord(i))) {
			int tmp;
			if (NOT(TtUtil::StringToInteger(bms.GetRegistArraySet()["STOP"][current_word], &tmp, 10))) {
				if (NOT(receiver.AtScrollObjectDefineError(current_word, "STOP", bms.GetRegistArraySet()["STOP"][current_word]))) {
					throw InvalidFormatAsStopSequenceException(current_word, bms.GetRegistArraySet()["STOP"][current_word]);
				}
			}
			else {
				stop_sequence_table[current_word] = tmp;
			}
		}
	}

	double pos = 0.0;

	int convert_start_channel_position = static_cast<int>(
		bms.GetBarManager().GetChannelPositionByBarNumber(parameter_set_.convert_start_bar_number_));
	int triming_start_channel_position = static_cast<int>(
		bms.GetBarManager().GetChannelPositionByBarNumber(parameter_set_.triming_start_bar_number_));
	int start_channel_position = std::min(convert_start_channel_position, triming_start_channel_position);

	int convert_last_channel_position = static_cast<int>(
		bms.GetBarManager().GetChannelPositionByBarNumber(parameter_set_.convert_end_bar_number_) +
		bms.GetBarManager()[parameter_set_.convert_end_bar_number_].GetLength() - 1);
	int triming_end_channel_position = static_cast<int>(
		bms.GetBarManager().GetChannelPositionByBarNumber(parameter_set_.triming_end_bar_number_) +
		bms.GetBarManager()[parameter_set_.triming_end_bar_number_].GetLength());
	int object_exists_max_position =
		bms.GetChannelManager().GetObjectExistsMaxPosition(BmsChannel::IsShoudPlayWavChannel);
	if (object_exists_max_position == -1) {
		throw ShoudPlayObjectNotExistsException();
	}
	int end_channel_position = std::max(
		std::min(convert_last_channel_position, object_exists_max_position), triming_end_channel_position);

	double triming_start_wav_position = std::numeric_limits<double>::max();
	double triming_end_wav_position = std::numeric_limits<double>::min();

	HQWav result;

	receiver.AtMixStart(convert_start_channel_position, std::min(convert_last_channel_position, object_exists_max_position));
	for (int i = start_channel_position; i <= end_channel_position; ++i) {
		if (i >= triming_start_channel_position && i <= triming_end_channel_position &&
			pos < triming_start_wav_position) {
			triming_start_wav_position = pos;
		}
		if (i >= triming_start_channel_position && i <= triming_end_channel_position &&
			pos > triming_end_wav_position) {
			triming_end_wav_position = pos;
		}
		if (i < convert_start_channel_position || i > convert_last_channel_position ||
			i > object_exists_max_position) {
			pos += ((static_cast<double>(HQWav::FREQUENCY * 60) / bpm) /
				(static_cast<double>(bms.GetBarManager().GetBarDivisionCount()) / 4.0));
			continue;
		}

		double stop_sequence_time = 0.0;
		for (BmsChannelManager::ConstIterator it = bms.GetChannelManager().Begin(); it != bms.GetChannelManager().End(); ++it) {
			BmsChannel& current_channel = *it->second;
			for (BmsChannel::ConstIterator it2 = current_channel.Begin(); it2 != current_channel.End(); it2++) {
				BmsChannelBuffer& current_buffer = **it2;
				BmsWord current_word = current_buffer[i];
				if (current_channel.IsShoudPlayWavChannel() && current_word != BmsWord::MIN) {
					if (wav_table.IsNotExist(current_word)) {
						if (parameter_set_.error_in_not_entried_wav_was_used_) {
							throw NotEntriedWavWasUsedException(current_word);
						}
						continue;
					}
					// turn off previous WAV if same one is playing
					if (parameter_set_.cancel_at_resounding_ &&
						static_cast<unsigned int>(pos)-last_used_wav_pos[current_word.ToInteger()] < wav_table[current_word].GetLength()) {
						result.DeductAt(static_cast<int>(pos), wav_table[current_word],
							static_cast<int>(pos)-last_used_wav_pos[current_word.ToInteger()]);
					}
					result.MixinAt(static_cast<int>(pos), wav_table[current_word]);
					last_used_wav_pos[current_word.ToInteger()] = static_cast<int>(pos);
				}
				// BPM change
				else if (current_channel.IsBpmChangeChannel() && current_word != BmsWord::MIN) {
					int new_bpm;
					if (NOT(TtUtil::StringToInteger(current_word.ToCharPtr(), &new_bpm, 16))) {
						throw InvalidFormatAsBpmChangeValueException(current_word);
					}
					bpm = static_cast<double>(new_bpm);
				}
				// extended BPM change
				else if (current_channel.IsExtendedBpmChangeChannel() && current_word != BmsWord::MIN) {
					if (extended_bpm_table.find(current_word) == extended_bpm_table.end()) {
						if (NOT(receiver.AtScrollObjectEntryIsEmpty(current_word, "BMP"))) {
							throw ExtendedBpmChangeEntryNotExistException(current_word);
						}
					}
					else {
						bpm = extended_bpm_table[current_word];
					}
				}
				// STOP sequence
				else if (current_channel.IsStopSequenceChannel() && current_word != BmsWord::MIN) {
					if (stop_sequence_table.find(current_word) == stop_sequence_table.end()) {
						if (NOT(receiver.AtScrollObjectEntryIsEmpty(current_word, "STOP"))) {
							throw StopSequenceEntryNotExistException(current_word);
						}
					}
					else {
						// 192 指定で 1 小節分停止
						double new_stop = (static_cast<double>(stop_sequence_table[current_word]) / 192.0 * 4.0) * (static_cast<double>(HQWav::FREQUENCY * 60) / bpm);
						if (stop_sequence_time == 0.0 || new_stop > stop_sequence_time) {
							stop_sequence_time = new_stop;
						}
					}
				}
			}
		}
		pos += ((static_cast<double>(HQWav::FREQUENCY * 60) / bpm) /
			(static_cast<double>(bms.GetBarManager().GetBarDivisionCount()) / 4.0));
		pos += stop_sequence_time;
		receiver.AtMixProgress(i);
	}
	receiver.AtMixProgress(std::min(convert_last_channel_position, object_exists_max_position));
	receiver.AtMixEnd(std::min(convert_last_channel_position, object_exists_max_position));

	// トリミング
	// trimming
	if (parameter_set_.do_triming_) {
		result.Trim(
			std::min(static_cast<int>(result.GetLength()), static_cast<int>(triming_start_wav_position)),
			std::min(static_cast<int>(result.GetLength()), static_cast<int>(triming_end_wav_position)));
	}

	// ノーマライズ
	// normalize
	switch (parameter_set_.normalize_kind_) {
	case Parameter::Normalize::PEAK: {
		double change_ratio;
		receiver.AtNormalizeStart();
		result.PeakNormalize(&change_ratio);
		receiver.AtNormalizeEnd(change_ratio);
		break;
	}

	case Parameter::Normalize::AVERAGE: {
		double change_ratio;
		receiver.AtNormalizeStart();
		result.AverageNormalize(&change_ratio);
		receiver.AtNormalizeEnd(change_ratio);
		break;
	}

	case Parameter::Normalize::DEFAULT:
	case Parameter::Normalize::OVER: {
		double change_ratio;
		double over_ratio = parameter_set_.normalize_over_ppm_ * 0.000001;
		receiver.AtNormalizeStart();
		result.OverNormalize(over_ratio, &change_ratio);
		receiver.AtNormalizeEnd(change_ratio);
		break;
	}

	case Parameter::Normalize::NONE:
	default:
		break;
	}

	// コンプレッサ
	// compression
	if (parameter_set_.use_compressor_) {
		receiver.AtCompressStart(parameter_set_.compress_threshold_,
			parameter_set_.compress_ratio_,
			parameter_set_.compress_attack_time_,
			parameter_set_.compress_release_time_,
			parameter_set_.compress_look_ahead_);
		result.Compress(parameter_set_.compress_threshold_,
			parameter_set_.compress_ratio_,
			parameter_set_.compress_attack_time_,
			parameter_set_.compress_release_time_,
			parameter_set_.compress_look_ahead_);
		receiver.AtCompressEnd(parameter_set_.compress_threshold_,
			parameter_set_.compress_ratio_,
			parameter_set_.compress_attack_time_,
			parameter_set_.compress_release_time_,
			parameter_set_.compress_look_ahead_);
	}

	// 音量調整
	// volume
	if (parameter_set_.volume_ != 100) {
		receiver.AtVolumeChangeStart(parameter_set_.volume_);
		result.ChangeVolume(static_cast<double>(parameter_set_.volume_) / 100);
		receiver.AtVolumeChangeEnd(parameter_set_.volume_);
	}

	// ファイル書き出し
	// file output(write)
	std::string outputed_filename = parameter_set_.output_filename_;
	{
		if (parameter_set_.use_output_template_) {
			TemplatePathConverter path_converter(parameter_set_.input_filename_,
				parameter_set_.output_filename_,
				bms);
			path_converter.SetRemoveCanNotUseCharAsFilePath(parameter_set_.remove_can_not_use_char_as_file_path_);
			outputed_filename = path_converter.Convert(parameter_set_.output_template_source_);
		}
		receiver.AtWriteToFileStart(outputed_filename);
		result.WriteToFile(outputed_filename);
		receiver.AtWriteToFileEnd(outputed_filename);
	}

	// 後処理
	if (parameter_set_.execute_after_process_) {
		TtProcess::CreateInfo info(parameter_set_.after_process_execute_file_);
		TemplatePathConverter path_converter(parameter_set_.input_filename_,
			outputed_filename,
			bms);
		path_converter.SetRemoveCanNotUseCharAsFilePath(true);
		info.SetArguments(path_converter.Convert(parameter_set_.after_process_execute_arguments_));
		info.SetCurrentDirectory(TtPath::DirName(info.GetFileName()));
		info.SetInheritHandles(true);
		info.SetInheritStandardStreams(true);
		if (parameter_set_.after_process_start_on_background_) {
			info.SetShowState(TtWindow::ShowState::SHOW_MINIMIZED_NO_ACTIVE);
		}

		receiver.AtAfterProcessStart(info);
		TtProcess process;
		try {
			process.Create(info);
			receiver.AtCreatedProcess(process, info);

			if (parameter_set_.after_process_wait_for_process_exit_) {
				receiver.AtWaitForProcessStart(process);
				process.Start();
				process.Wait();
				receiver.AtWaitForProcessEnd(process);
			}
			else {
				process.Start();
			}
			if (parameter_set_.after_process_delete_wav_file_) {
				receiver.AtDeleteFileStart(outputed_filename);
				BOOL ret = ::DeleteFile(outputed_filename.c_str());
				receiver.AtDeleteFileEnd(outputed_filename, ret ? true : false);
			}
			receiver.AtAfterProcessEnd(process, info, outputed_filename, bms);
		}
		catch (TtWindowsSystemCallException& e) {
			receiver.AtFailStartProcess(info);
			throw StartProcessException(info.GetFileName(), info.GetArguments(), e.GetSystemErrorMessage());
		}
	}

	receiver.AtConvertEnd();
}
