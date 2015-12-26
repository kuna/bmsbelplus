#pragma once

#include "common.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_video.h"

extern "C" {
#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libavformat/avformat.h"
#include "ffmpeg/libavutil/frame.h"
#include "ffmpeg/libswscale/swscale.h"
}

class Image {
private:
	// basics about codecs ...
	// so it must be initalized before you load movie
	static bool _initalized;
	static bool _movie_available;
	static void _init();
	// if you want to use texture, then we need renderer
	static SDL_Renderer *renderer;
public:
	static void SetRenderer(SDL_Renderer* r);

private:
	// also includes movie!
	SDL_Texture *sdltex;
	AVFormatContext *moviectx;			// movie context (handle)
	AVCodecContext *codecctxorig;		// codec context (orig)
	AVCodecContext *codecctx;			// codec context
	AVCodec *codec;						// codec
	AVFrame *frame;						// rendering buffer
	SwsContext *sws_ctx;				// converter (image data -> YUV420)
	Uint8 *yPlane, *uPlane, *vPlane;	// stores YUV data
	int moviestream;
	double movielength;
	bool loop;

	// processing movie is a little difficult
	bool LoadMovie(char *path);
	void ReleaseMovie();
public:
	Image(std::wstring& filepath, bool loop=true);
	~Image();
	bool IsLoaded();
	void Reset();				// reset pos to first one
	void Refresh(double t);		// refreshes texture in case of movie (loop forever if it's longer then movie)
};
