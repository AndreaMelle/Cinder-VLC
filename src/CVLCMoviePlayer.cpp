#include "CVLCMoviePlayer.h"

using namespace cvlc;

libvlc_instance_t* MoviePlayer::libvlc = NULL;
std::atomic<int> MoviePlayer::initialized = 0;

void MoviePlayer::startVLC()
{
	if (initialized == 0)
	{
		char const *vlc_argv[] = {
			"-I", "dumy",      // No special interface
			"--ignore-config", // Don't use VLC's config
			"--no-audio",
			"--no-xlib",
			"--plugin-path=./plugins",
			//"--video-filter", "sepia",
			//"--sepia-intensity=200"
		};

		int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

		libvlc = libvlc_new(vlc_argc, vlc_argv);

		if (NULL == libvlc)
		{
			throw "LibVLC initialization failure";
		}
	}

	initialized++;
}

void MoviePlayer::stopVLC()
{
	initialized--;

	if (initialized <= 0)
	{
		libvlc_release(libvlc);
		initialized = 0;
	}	
}

MoviePlayer::MoviePlayer(const ci::fs::path &path) :
hasNewFrame(false),
mVideoTextureRef(NULL),
mPlayable(false),
mLoaded(false),
mDuration(-1),
mWidth(0),
mHeight(0),
mIsDone(true),
mSeekable(false),
mFrameCount(0),
mPlayingForward(true),
mLoop(false)
{
	MoviePlayer::startVLC();

	std::string moviePathStr = path.string();
	libvlc_media_t *media;
	media = libvlc_media_new_path(libvlc, moviePathStr.c_str());
	
	libvlc_media_parse(media);

	libvlc_media_track_t** tracks = NULL;
	unsigned int track_count = libvlc_media_tracks_get(media, &tracks);

	if (tracks != NULL)
	{
		for (unsigned int i = 0; i < track_count; ++i)
		{
			libvlc_media_track_t* track = tracks[i];

			if (track->i_type == libvlc_track_video)
			{
				libvlc_video_track_t* videoTrack = track->video;
				mWidth = videoTrack->i_width;
				mHeight= videoTrack->i_height;
			}
		}
	}

	libvlc_media_tracks_release(tracks, track_count);
	
	medialist = libvlc_media_list_new(libvlc);
	libvlc_media_list_add_media(medialist, media);

	//mediaplayer = libvlc_media_player_new_from_media(media);

	mediaplayer = libvlc_media_player_new(libvlc);
	medialistplayer = libvlc_media_list_player_new(libvlc);

	libvlc_media_list_player_set_media_list(medialistplayer, medialist);
	libvlc_media_list_player_set_media_player(medialistplayer, mediaplayer);

	libvlc_media_release(media);

	SetVideoCallbacks(mediaplayer, this);
	libvlc_video_set_format(mediaplayer, "RGBA", mWidth, mHeight, mWidth * 4); //RV16	

	eventManager = libvlc_media_player_event_manager(mediaplayer);

	SetVideoCallback(eventManager, libvlc_MediaPlayerMediaChanged, this);
	//libvlc_MediaPlayerNothingSpecial
	//libvlc_MediaPlayerOpening
	//libvlc_MediaPlayerBuffering
	SetVideoCallback(eventManager, libvlc_MediaPlayerPlaying, this);
	//libvlc_MediaPlayerPaused
	//libvlc_MediaPlayerStopped
	//libvlc_MediaPlayerForward
	//libvlc_MediaPlayerBackward
	SetVideoCallback(eventManager, libvlc_MediaPlayerEndReached, this);
	SetVideoCallback(eventManager, libvlc_MediaPlayerEncounteredError, this);
	//libvlc_MediaPlayerTimeChanged
	//libvlc_MediaPlayerPositionChanged
	SetVideoCallback(eventManager, libvlc_MediaPlayerSeekableChanged, this);
	SetVideoCallback(eventManager, libvlc_MediaPlayerPausableChanged, this);
	//libvlc_MediaPlayerTitleChanged
	//libvlc_MediaPlayerSnapshotTaken
	SetVideoCallback(eventManager, libvlc_MediaPlayerLengthChanged, this);
	//libvlc_MediaPlayerVout
	//libvlc_MediaPlayerScrambledChanged
	//libvlc_MediaPlayerESAdded
	//libvlc_MediaPlayerESDeleted
	//libvlc_MediaPlayerESSelected
	
	mDataSource = new BYTE[mWidth * mHeight * 4 * sizeof(BYTE)];
	memset(mDataSource, 0, mWidth * mHeight * 4 * sizeof(BYTE));

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, mWidth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, mDataSource);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	mVideoTextureRef = ci::gl::Texture::create(GL_TEXTURE_2D, mTexture, mWidth, mHeight, true);

	mLoaded = true;
}

MoviePlayer::~MoviePlayer()
{
	libvlc_media_player_stop(mediaplayer);
	libvlc_media_player_release(mediaplayer);

	libvlc_media_list_release(medialist);
	libvlc_media_list_player_release(medialistplayer);

	glDeleteTextures(1, &mTexture);

	if (mDataSource)
	{
		delete[] mDataSource;
		mDataSource = NULL;
	}

	MoviePlayer::stopVLC();
}

void MoviePlayer::setLoop(bool loop)
{
	mLoop = loop;
	mPlayingForward = !loop;

	if (loop)
		libvlc_media_list_player_set_playback_mode(medialistplayer, libvlc_playback_mode_loop);
	else
		libvlc_media_list_player_set_playback_mode(medialistplayer, libvlc_playback_mode_default);
}

void MoviePlayer::play()
{
	//libvlc_media_player_play(mediaplayer);
	libvlc_media_list_player_play(medialistplayer);
}

void MoviePlayer::stop()
{
	//libvlc_media_player_stop(mediaplayer);
	libvlc_media_list_player_stop(medialistplayer);
}

void MoviePlayer::pause()
{
	if (!mPausable)
	{
		mPausable = (libvlc_media_player_can_pause(mediaplayer) ? true : false);
	}

	if (mPausable)
	{
		//libvlc_media_player_pause(mediaplayer);
		libvlc_media_list_player_pause(medialistplayer);
	}
}

bool MoviePlayer::isPlaying() const
{
	//return (libvlc_media_player_is_playing(mediaplayer) ? true : false);
	return (libvlc_media_list_player_is_playing(medialistplayer) ? true : false);
}

bool MoviePlayer::isDone() const
{
	return mIsDone;
}

bool MoviePlayer::checkPlayable()
{
	if (mPlayable) return true;
	mPlayable = (libvlc_media_player_will_play(mediaplayer) ? true : false);
	return mPlayable;
}

float MoviePlayer::getFramerate() const
{
	return libvlc_media_player_get_fps(mediaplayer);
}

int32_t MoviePlayer::getNumFrames() const
{
	return mFrameCount;
}

float MoviePlayer::getCurrentTime() const
{
	libvlc_time_t time = libvlc_media_player_get_time(mediaplayer);
	return (float)time * MILLISEC_TO_SEC;
}

void MoviePlayer::setRate(float rate)
{
	libvlc_media_player_set_rate(mediaplayer, rate);
}

float MoviePlayer::getRate()
{
	return libvlc_media_player_get_rate(mediaplayer);
}

bool MoviePlayer::checkNewFrame()
{
	return hasNewFrame;
}

void MoviePlayer::seekToTime(float seconds)
{
	if (!mSeekable)
	{
		mSeekable = (libvlc_media_player_is_seekable(mediaplayer) ? true : false);
	}

	if (mSeekable)
	{
		libvlc_time_t t = (int64_t)(seconds * SEC_TO_MILLISEC);
		libvlc_media_player_set_time(mediaplayer, t);
	}
}

void MoviePlayer::seekToStart()
{
	this->seekToTime(0);
}

void MoviePlayer::seekToEnd()
{
	this->seekToTime(this->getDuration());
}

void MoviePlayer::handle_vlc_event(const struct libvlc_event_t* evt)
{
	const int e = evt->type;

	if (e == (int)libvlc_MediaPlayerPlaying)
	{
		mIsDone = false;
	}
	else if (e == (int)libvlc_MediaPlayerTimeChanged)
	{

	}
	else if (e == (int)libvlc_MediaPlayerEncounteredError)
	{

	}
	else if (e == (int)libvlc_MediaPlayerPausableChanged)
	{
		mPausable = (libvlc_media_player_can_pause(mediaplayer) ? true : false);
	}
	else if (e == (int)libvlc_MediaPlayerMediaChanged)
	{
		mSeekable = (libvlc_media_player_is_seekable(mediaplayer) ? true : false);
	}
	else if (e == (int)libvlc_MediaPlayerEndReached)
	{
		mIsDone = true;
	}
	else if (e == (int)libvlc_MediaPlayerLengthChanged)
	{
		libvlc_time_t duration_ms = libvlc_media_player_get_length(mediaplayer);
		mDuration = duration_ms;
		mFrameCount = (int32_t)std::floorf(mDuration * MILLISEC_TO_SEC * this->getFramerate());
	}
	else if (e == (int)libvlc_MediaPlayerSeekableChanged)
	{

	}
}

float MoviePlayer::getDuration() const
{
	return ((float)mDuration * MILLISEC_TO_SEC);
}

void* MoviePlayer::lock(void **p_pixels)
{
	mMutex.lock();
	*p_pixels = (void*)mDataSource;
	return NULL;
}

void MoviePlayer::unlock(void *id, void *const *p_pixels)
{
	uint8_t *pixels = (uint8_t *)*p_pixels;
	mDataSource = pixels;
	hasNewFrame = true;
	mMutex.unlock();
}

void MoviePlayer::display(void *id) { }


ci::gl::TextureRef MoviePlayer::getTexture()
{
	if (hasNewFrame)
	{
		glBindTexture(GL_TEXTURE_2D, mTexture);
		mMutex.lock();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGBA, GL_UNSIGNED_BYTE, mDataSource);
		hasNewFrame = false;
		mMutex.unlock();
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	return mVideoTextureRef;
}