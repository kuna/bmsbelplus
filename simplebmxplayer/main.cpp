#include <tchar.h>
#include <wchar.h>
#include <string>

#include "bmsbel/bms_bms.h"
#include "bmsbel/bms_parser.h"
#include "SDL/SDL.h"
#include "image.h"
#include "audio.h"
#include "bmsresource.h"
#include "player.h"

namespace Parameter {
	std::wstring bms_path;

	void help() {
		wprintf(L"SimpleBmxPlayer\n================\n-- How to use -- \n\n"
			L"argument: (bmx file) (options ...)\n"
			L"options:"
			L"-noimage: don't load image files\n"
			L"-ms: start from n-th measure\n"
			L"-repeat: repeat bms for n-times\n"
			L"keys:\n"
			L"default key config is -\n(1P) LS Z S X D C F V (2P) M K , L . ; / RS\nyou can change it by changing preset files.\n"
			L"Press F5 to reload BMS file only.\n"
			L"Press F6 to reload BMS Resource file only.\n"
			L"Press - to move -5 measures.\n"
			L"Press + to move +5 measures.\n"
			L"Press Up/Down to change speed.\n"
			L"Press Right/Left to change lane.\n"
			L"Press Shift+Right/Left to change lift.\n");
	}

	bool parse(int argc, _TCHAR **argv) {
		if (argc <= 1)
			return false;
		bms_path = argv[1];
		return true;
	}
}

namespace Settings {
	int width;
	int height;

	void defaultsettings() {
		width = 1440;
		height = 900;
	}

	bool loadsettings() {
		// implement this ...
		return false;
	}
}

namespace Game {
	std::wstring bmspath;
	BmsBms bms;
	BmsResource<Audio, Image> bmsresource;
	Player player;
	std::wstring status_message;

	bool LoadBMSFile(std::wstring& bmspath) {
		// load bms file
		try {
			Game::bmspath = bmspath;
			BmsParser::Parse(bmspath, bms);
		}
		catch (BmsException &e) {
			wprintf(L"%ls\n", e.Message());
			return false;
		}
		return true;
	}

	bool LoadBMSResource() {
		std::wstring bms_dir = IO::get_filedir(bmspath) + PATH_SEPARATOR;
		// load WAV/BMP
		for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
			BmsWord word(i);
			if (bms.GetRegistArraySet()[L"WAV"].IsExists(word)) {
				std::wstring wav_path = bms_dir + bms.GetRegistArraySet()[L"WAV"][word];
				Audio *audio = new Audio(wav_path);
				if (audio->IsLoaded()) {
					bmsresource.SetWAV(word.ToInteger(), audio);
				}
				else {
					wprintf(L"[Warning] %ls - cannot load WAV file\n", wav_path);
					delete audio;
				}
			}
			if (bms.GetRegistArraySet()[L"BMP"].IsExists(word)) {
				std::wstring bmp_path = bms_dir + bms.GetRegistArraySet()[L"BMP"][word];
				Image *image = new Image(bmp_path);
				if (image->IsLoaded()) {
					bmsresource.SetBMP(word.ToInteger(), image);
				}
				else {
					wprintf(L"[Warning] %ls - cannot load BMP file\n", bmp_path);
					delete image;
				}
			}
		}
		return true;
	}

	void Release() {

		bms.Clear();
	}
}

// globals
SDL_Window* gWindow = NULL;
SDL_Renderer* renderer = NULL;

int _tmain(int argc, _TCHAR **argv) {
	if (!Parameter::parse(argc, argv)) {
		Parameter::help();
		return -1;
	}
	if (!Settings::loadsettings()) {
		wprintf(L"Cannot load settings files... Use Default settings...\n");
		Settings::defaultsettings();
	}

	/* initalization first */
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		wprintf(L"Failed to SDL_Init() ...\n");
		return -1;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
		wprintf(L"Failed to Open Audio ...\n");
		return -1;
	}
	gWindow = SDL_CreateWindow("SimpleBMXPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		Settings::width, Settings::height, SDL_WINDOW_SHOWN);
	if (!gWindow) {
		wprintf(L"Failed to create window");
		return -1;
	}
	renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		wprintf(L"Failed to create Renderer");
		return -1;
	}
	Image::SetRenderer(renderer);

	// start to load BMS file
	if (Game::LoadBMSFile(Parameter::bms_path)) {
		wprintf(L"Loaded BMS file successfully\n");
	}
	else {
		wprintf(L"cannot load BMS file\n");
		return -1;
	}
	// start to load BMS resource file
	if (Game::LoadBMSResource()) {
		wprintf(L"Loaded BMS resources successfully\n");
	}
	else {
		wprintf(L"cannot load BMS resource file\n");
		return -1;
	}
	
	// Let's render something ...
	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		SDL_RenderClear(renderer);
		//SDL_RenderCopy(renderer, bitmapTex, 0, 0);
		SDL_RenderPresent(renderer);
	}

	// release everything
	Game::bmsresource.Clear();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();

	// end
	return 0;
}