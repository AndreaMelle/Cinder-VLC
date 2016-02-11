#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder\gl\Texture.h"
#include "CVLCMoviePlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SimplePlaybackApp : public App
{
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown(KeyEvent evt);
	void update();
	void draw();
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

CINDER_APP(SimplePlaybackApp, RendererGl)
