#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder\gl\Texture.h"
#include "CVLCMoviePlayer.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MultipleVideosApp : public App
{
  public:
	void setup();
	void mouseDown( MouseEvent event );
	void keyDown(KeyEvent evt);
	void update();
	void draw();
	void shutdown();

private:

	cvlc::MoviePlayerRef moviePlayer1;
	cvlc::MoviePlayerRef moviePlayer2;

};

void MultipleVideosApp::shutdown()
{

}

void MultipleVideosApp::setup()
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

void MultipleVideosApp::mouseDown(MouseEvent event)
{
}

void MultipleVideosApp::keyDown(KeyEvent evt)
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

void MultipleVideosApp::update()
{

}

void MultipleVideosApp::draw()
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

CINDER_APP(MultipleVideosApp, RendererGl)
