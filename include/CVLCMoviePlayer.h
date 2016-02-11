#ifndef __CVLC_MOVIE_PLAYER_H__
#define __CVLC_MOVIE_PLAYER_H__

#include "CVLCCommon.h"
#include "cinder\app\App.h"
#include "cinder\gl\gl.h"
#include "cinder\gl\Texture.h"
#include <memory>

/*
* This is currently a simpler implementation, which does not follow Cinder conventions!
*/

#define MILLISEC_TO_SEC 0.001f
#define SEC_TO_MILLISEC 1000.0f

namespace cvlc
{
	class MoviePlayer;
	typedef std::shared_ptr<MoviePlayer> MoviePlayerRef;

	class MoviePlayer : public IVLCFrameResponder, public IVLCEventResponder
	{
	public:
		MoviePlayer(const ci::fs::path &path);
		virtual ~MoviePlayer();

		static MoviePlayerRef create(const ci::fs::path &path) { return std::shared_ptr<MoviePlayer>(new MoviePlayer(path)); }

		ci::gl::TextureRef getTexture();

		virtual void* lock(void **p_pixels);
		virtual void unlock(void *id, void *const *p_pixels);
		virtual void display(void *id);
		virtual void handle_vlc_event(const struct libvlc_event_t* evt);

		bool checkPlayable();

		//! Returns the width of the movie in pixels
		int32_t		getWidth() const { return mWidth; }
		//! Returns the height of the movie in pixels
		int32_t		getHeight() const { return mHeight; }
		//! Returns the movie's aspect ratio, the ratio of its width to its height
		float getAspectRatio() const { return mWidth / (float)mHeight; }
		//! Returns the movie's pixel aspect ratio. Returns 1.0 if the movie does not contain an explicit pixel aspect ratio.
		//TODO: don't know how to get this info from VLC
		float getPixelAspectRatio() const { return 1.0f;  }
		//! Returns the movie's length measured in seconds
		float getDuration() const;
		//! Returns the movie's framerate measured as frames per second
		float getFramerate() const;
		//! Returns the total number of frames (video samples) in the movie
		int32_t getNumFrames() const;
		//! Returns whether a movie has a new frame available
		bool checkNewFrame();
		//! Returns the current time of a movie in seconds
		float getCurrentTime() const;
		//! Sets the movie to the time \a seconds
		void seekToTime(float seconds);
		//! Sets the movie time to the start time of frame \a frame
		//void		seekToFrame(int frame);
		//! Sets the movie time to its beginning
		void seekToStart();
		//! Sets the movie time to its end
		void seekToEnd();
		//! Sets whether the movie is set to loop during playback. If \a palindrome is true, the movie will "ping-pong" back and forth
		void setLoop(bool loop = true);
		//! Advances the movie by one frame (a single video sample). Ignores looping settings.
		//void		stepForward();
		//! Steps backward by one frame (a single video sample). Ignores looping settings.
		//void		stepBackward();
		//! Sets the playback rate, which begins playback immediately for nonzero values. 1.0 represents normal speed. Negative values indicate reverse playback and \c 0 stops.
		void setRate(float rate);
		//! Gets the requested playback rate. 1.0 represents normal speed. Negative values indicate reverse playback and \c 0 stops.
		float getRate();
		//! Returns whether the movie is currently playing or is paused/stopped.
		bool isPlaying() const;
		//! Returns whether the movie has completely finished playing
		bool isDone() const;
		//! Begins movie playback.
		void	play();
		//! Stops movie playback.
		void	stop();
		//! Pauses movie playback.
		void pause();

	private:
		MoviePlayer();
		static void startVLC();
		static void stopVLC();

		GLuint mTexture;
		ci::gl::TextureRef mVideoTextureRef;

		// What is this? Interesting...
		//#if defined( CINDER_MSW )
		//		gl::TextureCache	mTextureCache;
		//#endif

		uint32_t mWidth;
		uint32_t mHeight;
		int32_t mFrameCount;
		//int64_t mDuration;
		std::atomic<int64_t> mDuration;
		std::atomic<bool> mLoaded;
		std::atomic<bool> mPlayable;
		std::atomic<bool> mPausable;
		std::atomic<bool> mSeekable;
		std::atomic<bool> mPlayingForward;
		std::atomic<bool> mLoop;

		std::mutex mMutex;
		uint8_t* mDataSource;
		std::atomic<bool> hasNewFrame;
		std::atomic<bool> mIsDone;
		
		static libvlc_instance_t *libvlc;
		static std::atomic<int> initialized;

		libvlc_media_player_t *mediaplayer;
		libvlc_media_list_player_t *medialistplayer;
		libvlc_media_list_t *medialist;
		libvlc_event_manager_t* eventManager;
	};
};


#endif //__CVLC_MOVIE_PLAYER_H__