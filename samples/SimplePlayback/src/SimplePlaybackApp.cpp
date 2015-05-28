#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder\gl\Texture.h"
#include "CVLCMoviePlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SimplePlaybackApp : public AppNative
{
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown(KeyEvent evt);
	void update();
	void draw();
	void prepareSettings(Settings* settings);
	void shutdown();

private:

	cvlc::MoviePlayerRef moviePlayer;

};

void SimplePlaybackApp::shutdown()
{

}

void SimplePlaybackApp::setup()
{
	fs::path moviePath = app::getAssetPath("trailer_short.mp4");
	moviePlayer = cvlc::MoviePlayerRef(new cvlc::MoviePlayer(moviePath));
	moviePlayer->setLoop(true);
	moviePlayer->play();
}

void SimplePlaybackApp::mouseDown( MouseEvent event )
{
}

void SimplePlaybackApp::keyDown(KeyEvent evt)
{
	if (evt.getCode() == KeyEvent::KEY_SPACE)
	{
		moviePlayer->pause();
	}
	else if (evt.getCode() == KeyEvent::KEY_q)
	{
		quit();
	}
}

void SimplePlaybackApp::update()
{

}

void SimplePlaybackApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	gl::TextureRef mVideoTextureRef = moviePlayer->getTexture();

	if (mVideoTextureRef)
	{
		gl::draw(mVideoTextureRef, getWindowBounds());
	}
}

void SimplePlaybackApp::prepareSettings(Settings* settings)
{
	float mGlobalScale = 1.0f;

	int32_t width = static_cast<int32_t>(1920.0f * mGlobalScale);
	int32_t height = (int32_t)((float)width * 9.0f / 16.0f);

	settings->prepareWindow(Window::Format().size(width, height)
		.title("Video Sample")
		.borderless(true)
		.pos(-width, 0));
	settings->setFrameRate(60.0f);

}

CINDER_APP_NATIVE( SimplePlaybackApp, RendererGl )
