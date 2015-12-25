#include <tchar.h>
#include <wchar.h>
#include <string>

#include "bmsbel\bms_bms.h"
#include "SDL\SDL.h"
//#include "SDL\SDL_image.h"
//#include "SDL\SDL_mixer.h"
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

	bool parse(int argc, _TCHAR argv[]) {
		if (argc <= 1)
			return false;
		bms_path = argv[1];
		return true;
	}
}

namespace Settings {
	bool loadsettings() {
		return true;
	}
}

namespace Game {
	BmsBms bms;
	Player player;
	std::wstring status_message;

	bool LoadBMSFile(std::wstring& bmspath) {
		// load bms file

		// load resource

		return true;
	}
}

int _tmain(int argc, _TCHAR argv[]) {
	if (!Parameter::parse(argc, argv)) {
		Parameter::help();
		return -1;
	}
	if (!Settings::loadsettings()) {
		wprintf(L"Cannot load settings files...\n");
		return -1;
	}

	// init & show SDL window
	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		SDL_Window* gWindow = NULL;

		//The surface contained by the window
		SDL_Surface* gScreenSurface = NULL;
		//The image we will load and show on the screen
		SDL_Surface* gHelloWorld = NULL;

		gWindow = SDL_CreateWindow("Test Screen", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1440, 900, SDL_WINDOW_SHOWN);

		// start to load BMS file
		SDL_Delay(2000);

		SDL_DestroyWindow(gWindow);
		SDL_Quit();
	}
	else {
		wprintf(L"Failed to SDL_Init() ...");
		return -1;
	}

	// end
	return 0;
}