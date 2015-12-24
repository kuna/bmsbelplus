// bmx2wav_remake.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include "bmsbel\bms_bms.h"
#include "bmsbel\bms_parser.h"
#include <tchar.h>
#include <wchar.h>
#include <string>

#define NOT(v) (!(v))
#define CMP(a, b) (wcscmp((a), (b)) == 0)

#ifdef _WIN32
#define PATH_SEPARATOR		L"\\"
#define PATH_SEPARATOR_CHAR	L'\\'
#else
#define PATH_SEPARATOR		L"/"
#define PATH_SEPARATOR_CHAR	L'/'
#endif

namespace BMX2WAVParameter {
	enum OUTPUT_TYPE { OUTPUT_OGG, OUTPUT_WAV } output_type;
	std::wstring bms_path;
	std::wstring output_path;

	std::wstring substitute_extension(const std::wstring& filename, const std::wstring& newext) {
		auto i = filename.find_last_of(L'.');
		if (i == std::wstring::npos)
			return filename + newext;
		return filename.substr(0, i) + newext;
	}

	std::wstring substitute_output_extension(const std::wstring& filename) {
		if (output_type == OUTPUT_OGG) {
			return substitute_extension(filename, L".ogg");
		}
		else {
			return substitute_extension(filename, L".wav");
		}
	}

	std::wstring get_filepath(const std::wstring& filename) {
		return filename.substr(0, filename.find_last_of(PATH_SEPARATOR_CHAR));
	}

	std::wstring get_filename(const std::wstring& filename) {
		auto i = filename.find_last_of(PATH_SEPARATOR_CHAR);
		if (i == std::wstring::npos)
			return L"";
		return filename.substr(i+1);
	}

	int help() {
		printf("input args: (bmx path) -(option) (output path)\n"
			"available option:\n"
			"-oc: output file to current directory (default)\n"
			"-ob: output file to bms directory\n"
			"-o: output file to my custom path (you need to enter more argument)\n"
			"-wav: output audio as wav\n"
			"-ogg: output audio as ogg (default)\n"
			"\n"
			"EASY USE:\n"
			"JUST DRAG FILE to me and there will be *.ogg file!\n"
			);
		return -1;
	}

	int parse(int argc, _TCHAR* argv[]) {
		if (argc <= 1 || CMP(argv[1], L"-?") || CMP(argv[1], L"-h")) {
			return help();
		}

		// default
		bms_path = argv[1];
		output_type = OUTPUT_OGG;
		output_path = substitute_output_extension(get_filepath(argv[0]) + PATH_SEPARATOR + get_filename(bms_path));
		// parse
		for (int i = 2; i < argc; i++) {
			if (CMP(argv[i], L"-oc")) {
				// it's default, so do nothing
			}
			else if (CMP(argv[i], L"-ob")) {
				// destion to bms directory
				output_path = substitute_output_extension(bms_path);
			}
			else if (CMP(argv[i], L"-o")) {
				if (++i == argc)
					return help();
				output_path = argv[i];
				if (output_path.back() == PATH_SEPARATOR_CHAR) {
					// if destination is folder, then automatically add filename
					output_path = output_path + substitute_output_extension(get_filename(bms_path));
				}
			}
			else if (CMP(argv[i], L"-wav")) {
				output_type = OUTPUT_WAV;
				output_path = substitute_output_extension(output_path);
			}
			else if (CMP(argv[i], L"-ogg")) {
				// it's default, so do nothing
			}
		}

		return 0;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	// parse argument
	if (BMX2WAVParameter::parse(argc, argv) == -1)
		return -1;

	// load bms file
	BmsBms bms;
	BmsParser::Parse(BMX2WAVParameter::bms_path, bms);
	BmsTimeManager bms_time;
	bms.CalculateTime(bms_time);
	wprintf(L"BMS Path: %ls\n", BMX2WAVParameter::bms_path.c_str());
	wprintf(L"BMS Title: %ls\n", bms.GetHeaders()[L"TITLE"].c_str());
	wprintf(L"BMS Length: %.03lf (sec)\n", bms_time.GetEndTime());
	wprintf(L"Output Path: %ls\n", BMX2WAVParameter::output_path.c_str());

	// load audio data
	wprintf(L"Loading Audio Data ...\n");

	// encode and save it
	// TODO

	return 0;
}

