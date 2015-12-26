#include "image.h"

extern "C" {
#include "ffmpeg/libavutil/avutil.h"
}

#define ISLOADED(v) (v != 0)
//
// ffmpeg initalize part
//

bool Image::_initalized = false;
bool Image::_movie_available = false;	// if initalize failed, then false
SDL_Renderer *Image::renderer = 0;

// from: http://dranger.com/ffmpeg/
void Image::_init() {
	// attempt initalize only once
	if (_initalized)
		return;
	_initalized = true;

	// initalize library
	// This registers all available file formats and codecs
	av_register_all();

	// if successfully initalized, then movie is available
	_movie_available = true;
}


//
// general func
//
void Image::SetRenderer(SDL_Renderer* r) {
	renderer = r;
}

Image::Image(std::wstring& filepath, bool loop) : sdltex(0), moviectx(0), loop(loop) {
	// must have renderer
	if (!renderer)
		return;

	// initalize ffmpeg first
	try {
		_init();
	}
	catch (...) {
	}

	char path_utf8[1024];
	ENCODING::wchar_to_utf8(filepath.c_str(), path_utf8, 1024);
	// check is it movie or image
	std::wstring ext = IO::get_fileext(filepath);	COMMON::lower(ext);
	if (ext == L".mpg" || ext == L".avi" || ext == L".mpeg") {
		if (_movie_available) {
			if (!LoadMovie(path_utf8))
				ReleaseMovie();
		}
	}
	else {
		sdltex = IMG_LoadTexture(renderer, path_utf8);
	}
}

bool Image::LoadMovie(char *path) {
	// load movie
	if (avformat_open_input(&moviectx, path, NULL, 0) != 0) {
		// failed to load movie
		return false;
	}
	if (avformat_find_stream_info(moviectx, 0) < 0) {
		// failed to retrieve stream information
		return false;
	}
	// find first video stream
	moviestream = -1;
	for (int i = 0; i < moviectx->nb_streams; i++) if (moviectx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		moviestream = i;
	if (moviestream < 0) {
		return false;
	}

	/*
	* codec start
	*/
	// get codec context pointer
	codecctxorig = moviectx->streams[moviestream]->codec;
	// Find the decoder for the video stream
	codec = avcodec_find_decoder(codecctxorig->codec_id);
	if (codec == NULL) {
		// unsupported codec!
		return false;
	}
	// copy codec context
	codecctx = avcodec_alloc_context3(codec);
	if (avcodec_copy_context(codecctx, codecctxorig) != 0) {
		// failed to copy codec context
		return false;
	}
	// open codec
	if (avcodec_open2(codecctx, codec, NULL) < 0) {
		return false;
	}
	/*
	* codec end
	*/

	// allocate video frame
	frame = av_frame_alloc();

	// create basic texture
	sdltex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,
		SDL_TEXTUREACCESS_STREAMING, codecctx->width, codecctx->height);
	if (sdltex == 0) {
		return false;
	}

	// initialize SWS context for software scaling
	sws_ctx = sws_getContext(codecctx->width, codecctx->height,
		codecctx->pix_fmt, codecctx->width, codecctx->height,
		AV_PIX_FMT_YUV420P,
		SWS_BILINEAR,
		NULL,
		NULL,
		NULL);
	// set up YV12 pixel array (12 bits per pixel)
	size_t yPlaneSz, uvPlaneSz;
	yPlaneSz = codecctx->width * codecctx->height;
	uvPlaneSz = codecctx->width * codecctx->height / 4;
	yPlane = (Uint8*)malloc(yPlaneSz);
	uPlane = (Uint8*)malloc(uvPlaneSz);
	vPlane = (Uint8*)malloc(uvPlaneSz);

	// set video pos to first & render first scene
	Reset();
	Refresh(0);

	return true;
}

void Image::Refresh(double t) {
	int uvPitch = codecctx->width / 2;
	AVPacket packet;
	int frameFinished;
	while (av_read_frame(moviectx, &packet) >= 0) {
		// Is this a packet from the video stream?
		if (packet.stream_index == moviestream) {
			// Decode video frame
			avcodec_decode_video2(codecctx, frame, &frameFinished, &packet);

			// Did we get a video frame?
			if (frameFinished) {
				AVPicture pict;
				pict.data[0] = yPlane;
				pict.data[1] = uPlane;
				pict.data[2] = vPlane;
				pict.linesize[0] = codecctx->width;
				pict.linesize[1] = uvPitch;
				pict.linesize[2] = uvPitch;

				// Convert the image into YUV format that SDL uses
				sws_scale(sws_ctx, (uint8_t const * const *)frame->data,
					frame->linesize, 0, codecctx->height, pict.data,
					pict.linesize);

				SDL_UpdateYUVTexture(
					sdltex,
					NULL,
					yPlane,
					codecctx->width,
					uPlane,
					uvPitch,
					vPlane,
					uvPitch
					);
			}
		}
	}
}

void Image::Reset() {

}

void Image::ReleaseMovie() {
	// we don't release texture, it'll automatically destroyed in ~Image();
	if (frame) { av_frame_free(&frame); frame = 0; }
	if (yPlane) { free(yPlane); yPlane = 0; }
	if (uPlane) { free(uPlane); uPlane = 0; }
	if (vPlane) { free(vPlane); vPlane = 0; }
	if (codecctxorig) { avcodec_close(codecctxorig); codecctxorig = 0; }
	if (codecctx) { avcodec_close(codecctx); codecctx = 0; }
	if (moviectx) { avformat_close_input(&moviectx); moviectx = 0; }
}

Image::~Image() {
	if (ISLOADED(moviectx))
		ReleaseMovie();
	if (ISLOADED(sdltex))
		SDL_DestroyTexture(sdltex);
}

bool Image::IsLoaded() {
	return ISLOADED(sdltex);
}

SDL_Texture* Image::GetPtr() {
	return sdltex;
}