#include "SDL/SDL_ttf.h"
#include "SDL/SDL_image.h"
#include <string>

/*
 * stores font and it's texture related information
 */
class Font {
private:
	void CreateFontTexture();
public:
	Font(std::wstring& path, int size);
	Font(char *path, int size);
};