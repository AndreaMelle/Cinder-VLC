#ifndef __CVLC_COMMON_H__
#define __CVLC_COMMON_H__

#include "vlc/vlc.h"

namespace cvlc
{
	class IVLCFrameResponder
	{
	public:
		virtual void* lock(void **p_pixels) = 0; // lock the buffer, assign the write pointer
		virtual void unlock(void *id, void *const *p_pixels) = 0; //unlock the buffer, writing done
		virtual void display(void *id) = 0;
	};

	// TODO
	class IVLCEventResponder
	{
	public:
		virtual void handle_vlc_event(const struct libvlc_event_t* evt) = 0;
	};

	// static callbacks
	// VLC prepares to render a video frame.
	static void *lock(void *data, void **p_pixels)
	{
		return ((IVLCFrameResponder*)data)->lock(p_pixels);
	}

	// VLC just rendered a video frame.
	static void unlock(void *data, void *id, void *const *p_pixels)
	{
		((IVLCFrameResponder*)data)->unlock(id, p_pixels);
	}

	// display callback, not used here
	static void display(void *data, void *id)
	{
		((IVLCFrameResponder*)data)->display(id);
	}

	static void handle_vlc_event(const struct libvlc_event_t* evt, void* data)
	{
		((IVLCEventResponder*)data)->handle_vlc_event(evt);
	}

	static void SetVideoCallbacks(libvlc_media_player_t* mediaplayer, IVLCFrameResponder* responder)
	{
		libvlc_video_set_callbacks(mediaplayer, lock, unlock, display, responder);
	}

	static void SetVideoCallback(libvlc_event_manager_t* eventManager, libvlc_event_e eventType, IVLCEventResponder* responder)
	{
		libvlc_event_attach(eventManager, eventType, handle_vlc_event, responder);
	}

};

#endif //__CVLC_COMMON_H__