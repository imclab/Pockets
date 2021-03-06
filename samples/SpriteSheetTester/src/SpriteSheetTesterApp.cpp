#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Easing.h"

#include "SpriteSheet.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

class SpriteSheetTesterApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );
  void mouseMove( MouseEvent event );
	void update();
	void draw();
private:
  SpriteSheetRef  mSheet;
  vector<string>  mSpriteNames;
  vector<string>::iterator mCurrentSprite;
  Vec2i           mLoc = Vec2i::zero();
  float           mOffset = 0;
  float           mOffsetDirection = 1.2f;
  double          mLastUpdate = 0;
};

void SpriteSheetTesterApp::setup()
{
  try
  {
    mSheet = SpriteSheet::load( getAssetPath( "sprite_sheet.json" ) );
    mSpriteNames = mSheet->getSpriteNames();
    mCurrentSprite = mSpriteNames.begin();
  } catch ( exception &exc )
  {
    console() << __PRETTY_FUNCTION__ << ": " << exc.what() << endl;
  }
  setWindowSize( 640, 480 );
  mLoc = getWindowCenter();
}

void SpriteSheetTesterApp::mouseMove( MouseEvent event )
{
  mLoc = event.getPos();
}

void SpriteSheetTesterApp::mouseDown( MouseEvent event )
{
  mCurrentSprite++;
  if( mCurrentSprite == mSpriteNames.end() )
  {
    mCurrentSprite = mSpriteNames.begin();
  }
}

void SpriteSheetTesterApp::update()
{
  double now = getElapsedSeconds();
  float delta = now - mLastUpdate;
  mOffset += delta * mOffsetDirection;
  if( mOffset > 1 || mOffset < 0 )
  {
    mOffsetDirection *= -1;
    mOffset += delta * mOffsetDirection;
  }
  mLastUpdate = now;
}

void SpriteSheetTesterApp::draw()
{
	gl::clear( Color::gray( 0.85f ) );
  if( mSheet )
  {
    Rectf bounds( 0, 0, 100, 100 );
    gl::color( Color( 1, 0, 1 ) );
    gl::disable( GL_TEXTURE_2D );
    gl::drawSolidRect( bounds );

    gl::color( Color::white() );
    mSheet->enableAndBind();
    SpriteSheet::SpriteData sprite = mSheet->getSpriteData( *mCurrentSprite );
    Vec2i size = sprite.getSize();
    mSheet->draw( *mCurrentSprite, mLoc - size );
    mSheet->drawScrolled( *mCurrentSprite, mLoc - Vec2i( 0, size.y )
                        , Vec2f( -1, 0 ) * easeInQuad(mOffset) );
    mSheet->drawScrolled( *mCurrentSprite, mLoc - Vec2i( size.x, 0 )
                        , Vec2f( 1, 1 ) * easeInQuint(mOffset) );
    mSheet->drawScrolled( *mCurrentSprite, mLoc
                        , Vec2f( 0, -1 ) * easeInElastic(mOffset, 2.0f, 2.0f) );

    float t = easeInOutQuint( mOffset );
    Vec2i loc( lerp( bounds.getCenter(), bounds.getLowerRight() + sprite.getRegistrationPoint(), t ) );
    mSheet->drawInRect( *mCurrentSprite, loc, bounds );
  }
}

CINDER_APP_NATIVE( SpriteSheetTesterApp, RendererGl )
