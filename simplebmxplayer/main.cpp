#include <tchar.h>
#include <wchar.h>
#include <string>

#include "bmsbel/bms_bms.h"
#include "bmsbel/bms_parser.h"
#include "SDL/SDL.h"
#include "SDL/SDL_FontCache.h"	// extract it to a class
#include "SDL/SDL_timer.h"
#include "image.h"
#include "audio.h"
#include "timer.h"
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
	// bms info
	std::wstring bmspath;
	BmsBms bms;
	BmsResource<Audio, Image> bmsresource;
	Player player;

	// SDL
	SDL_Window* gWindow = NULL;
	SDL_Renderer* renderer = NULL;

	// drawing texture/fonts
	FC_Font* font = NULL;

	// timers
	Timer fpstimer;	int fps = 0;
	Timer gametimer;

	/*
	 * BMS Player part
	 */
	BmsWord bgavalue(0);
	void OnBmsBga(BmsWord v, BmsChannelType::BMSCHANNEL c) {
		if (c == BmsChannelType::BGA)
			bgavalue = v;
	}
	void OnBmsSound(BmsWord v) {
		if (bmsresource.IsWAVLoaded(v.ToInteger())) {
			bmsresource.GetWAV(v.ToInteger())->Stop();
			bmsresource.GetWAV(v.ToInteger())->Play();
			printf("%s (%d)\n", v.ToString().c_str(), v.ToInteger());
		}
	}
	/*
	 * BMS Player part
	 */

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
		// set player
		player.Prepare(&bms, 0, true);
		player.SetOnBga(&OnBmsBga);
		player.SetOnSound(&OnBmsSound);
		return true;
	}

	bool LoadBMSResource() {
		std::wstring bms_dir = IO::get_filedir(bmspath) + PATH_SEPARATOR;
		// load WAV/BMP
		for (unsigned int i = 0; i < BmsConst::WORD_MAX_COUNT; ++i) {
			BmsWord word(i);
			if (bms.GetRegistArraySet()[L"WAV"].IsExists(word)) {
				std::wstring wav_path = bms_dir + bms.GetRegistArraySet()[L"WAV"][word];
				std::wstring alter_ogg_path = bms_dir + IO::substitute_extension(bms.GetRegistArraySet()[L"WAV"][word], L".ogg");
				Audio *audio = NULL;
				if (IO::is_file_exists(alter_ogg_path))
					audio = new Audio(alter_ogg_path, word.ToInteger());
				else if (IO::is_file_exists(wav_path))
					audio = new Audio(wav_path, word.ToInteger());

				if (audio && audio->IsLoaded()) {
					bmsresource.SetWAV(word.ToInteger(), audio);
				}
				else {
					wprintf(L"[Warning] %ls - cannot load WAV file\n", wav_path.c_str());
					if (audio) delete audio;
				}
			}
			if (bms.GetRegistArraySet()[L"BMP"].IsExists(word)) {
				std::wstring bmp_path = bms_dir + bms.GetRegistArraySet()[L"BMP"][word];
				Image *image = new Image(bmp_path);
				if (image->IsLoaded()) {
					bmsresource.SetBMP(word.ToInteger(), image);
				}
				else {
					wprintf(L"[Warning] %ls - cannot load BMP file\n", bmp_path.c_str());
					delete image;
				}
			}
		}
		return true;
	}

	bool PrepareGameResource() {
		font = FC_CreateFont();
		if (!font)
			return false;
		FC_LoadFont(font, renderer, "lazy.ttf", 28, FC_MakeColor(0, 0, 0, 255), TTF_STYLE_NORMAL);
		return true;
	}

	void Start() {
		fpstimer.Start();
		gametimer.Start();
		fps = 0;
	}

	/*
	 * Rendering part
	 */
	void Render_FPS() {
		float avgfps = fps / (fpstimer.GetTick() / 1000.0f);
		fps++;
		// make FPS texture
		//FC_Draw(font, renderer, 0, 0, "%d Frame", fps);
		if (fps % 1000 == 0)
			wprintf(L"%.2f\n", avgfps);
	}

	void Render() {
		// set player time and ...
		player.SetTime(gametimer.GetTick());
		// get note pos
		double notepos = player.GetCurrentPos();
		// render BGA
		if (bmsresource.IsBMPLoaded(bgavalue.ToInteger()))
			SDL_RenderCopy(renderer, bmsresource.GetBMP(bgavalue.ToInteger())->GetPtr(), 0, 0);
	}
	/*
	 * Rendering End
	 */

	void Release() {
		FC_FreeFont(font);
		Game::bmsresource.Clear();
		bms.Clear();
	}
}

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
	Game::gWindow = SDL_CreateWindow("SimpleBMXPlayer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		Settings::width, Settings::height, SDL_WINDOW_SHOWN);
	if (!Game::gWindow) {
		wprintf(L"Failed to create window\n");
		return -1;
	}
	Game::renderer = SDL_CreateRenderer(Game::gWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!Game::renderer) {
		wprintf(L"Failed to create Renderer\n");
		return -1;
	}
	Image::SetRenderer(Game::renderer);

	// prepare Game Resource
	if (Game::PrepareGameResource()) {
		wprintf(L"Game resource prepared\n");
	}
	else {
		wprintf(L"Game resource prepare failed ...\n");
		return -1;
	}
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
	Game::Start();
	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
			if (e.type == SDL_KEYUP) {
				if (Game::bmsresource.IsWAVLoaded(18)) {
					printf("%s\n", BmsWord(18).ToString().c_str());
					//Game::bmsresource.GetWAV(18)->Stop();
					Game::bmsresource.GetWAV(18)->Play();
					printf("%s\n", Mix_GetError());
				}
			}
		}

		SDL_RenderClear(Game::renderer);
		Game::Render();
		Game::Render_FPS();
		SDL_RenderPresent(Game::renderer);
	}

	// release everything
	Game::Release();
	Mix_CloseAudio();
	SDL_DestroyRenderer(Game::renderer);
	SDL_DestroyWindow(Game::gWindow);
	SDL_Quit();

	// end
	return 0;
}