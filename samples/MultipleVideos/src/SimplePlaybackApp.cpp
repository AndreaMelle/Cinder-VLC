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

	cvlc::MoviePlayerRef moviePlayer1;
	cvlc::MoviePlayerRef moviePlayer2;

};

void SimplePlaybackApp::shutdown()
{

}

void SimplePlaybackApp::setup()
{
	fs::path moviePath1 = app::getAssetPath("trailer_short_colorized.mp4");
	fs::path moviePath2 = app::getAssetPath("trailer_short.mp4");

	moviePlayer1 = cvlc::MoviePlayerRef(new cvlc::MoviePlayer(moviePath1));
	moviePlayer2 = cvlc::MoviePlayerRef(new cvlc::MoviePlayer(moviePath2));

	moviePlayer1->setLoop(true);
	moviePlayer2->setLoop(true);

	moviePlayer1->play();
	moviePlayer2->play();
}

void SimplePlaybackApp::mouseDown( MouseEvent event )
{
}

void SimplePlaybackApp::keyDown(KeyEvent evt)
{
	if (evt.getCode() == KeyEvent::KEY_SPACE)
	{
		moviePlayer1->pause();
		moviePlayer2->pause();
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

	gl::TextureRef mVideoTextureRef1 = moviePlayer1->getTexture();
	gl::TextureRef mVideoTextureRef2 = moviePlayer2->getTexture();

	Rectf rect1 = Rectf(0, 0, getWindowWidth() / 2.0f, getWindowHeight() / 2.0f);
	Rectf rect2 = Rectf(0, 0, getWindowWidth() / 2.0f, getWindowHeight() / 2.0f);

	if (mVideoTextureRef1)
	{
		gl::draw(mVideoTextureRef1, rect1);
	}

	gl::pushMatrices();
	gl::translate(getWindowWidth() / 2.0f, 0);

	if (mVideoTextureRef2)
	{
		gl::draw(mVideoTextureRef2, rect2);
	}
	gl::popMatrices();
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
