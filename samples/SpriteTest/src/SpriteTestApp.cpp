#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "pockets/SpriteSheet.h"
#include "pockets/SimpleRenderer.h"
#include "pockets/TriangleRenderer.h"
#include "pockets/Sprite.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace pockets;

/**
 Test of basic Sprite functionality, including:
 Rendering to screen
 - TriangleRenderer
 - SimpleRenderer
 Rotation about registration point
 Sprite Animation (in progress)
 TODO:
 Clipping (Simple rectangular masking)
 Scaling (in Locus2d)
*/

class SpriteTestApp : public AppNative {
public:
  void setup();
  void mouseDown( MouseEvent event );
  void update();
  void draw();
private:
  SpriteSheetRef    mSpriteSheet;
  SpriteAnimation   mSpriteAnimation;
  Sprite            mSprite;
  SimpleRenderer    mSimpleRenderer;
  TriangleRenderer  mTriangleRenderer;
  double            mLastUpdate;
};

void SpriteTestApp::setup()
{
  addAssetDirectory( getAssetPath("") / ".." / ".." / "shared_assets" );
  mSpriteSheet = SpriteSheet::load( getAssetPath("sprite_sheet.json") );
  auto sprites = mSpriteSheet->getSpriteNames();

  for( const string &name : sprites )
  { // hold for equivalent of 2 frames at 24fps
    mSpriteAnimation.addFrame( mSpriteSheet->getSpriteData(name), 2.0f / 24.0f );
  }

  Vec2f left_center = Vec2f{ 0.25f, 0.5f } * Vec2f{ getWindowSize() };
  mSpriteAnimation.getLocus().setLoc( left_center );
  mSprite = mSpriteSheet->getSprite("wood_cracked");
  mSprite.getLocus().setLoc( left_center + Vec2f{ 0, getWindowHeight() * 0.25f } );
  mSprite.setLayer( 5 );

  mSimpleRenderer.add( &mSpriteAnimation );
  mSimpleRenderer.add( &mSprite );
  //
  mTriangleRenderer.add( &mSprite );
  mLastUpdate = getElapsedSeconds();

  // sort by layer
  mSimpleRenderer.sort();
  mTriangleRenderer.sort();
}

void SpriteTestApp::mouseDown( MouseEvent event )
{
}

void SpriteTestApp::update()
{
  float now = getElapsedSeconds();
  float deltaTime = now - mLastUpdate;
  mLastUpdate = now;
  mSpriteAnimation.update( deltaTime );
  mSprite.getLocus().setRotation( mSprite.getLocus().getRotation() + M_PI * 0.01f );
}

void SpriteTestApp::draw()
{
  // clear out the window with black
  gl::clear( Color( 0, 0, 0 ) );
  gl::disable( GL_TEXTURE_2D );
  gl::drawLine( Vec2f{ getWindowWidth() / 2, 0 }, Vec2f{ getWindowWidth() / 2, getWindowHeight() } );
  mSpriteSheet->enableAndBind();
  mSimpleRenderer.render();
  gl::pushModelView();
  gl::translate( Vec2f{ getWindowWidth() / 2, 0.0f } );
  mTriangleRenderer.render();
  gl::popModelView();
}

CINDER_APP_NATIVE( SpriteTestApp, RendererGl )
