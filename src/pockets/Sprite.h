/*
 * Copyright (c) 2013 David Wicks
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once
#include "SimpleRenderer.h"
#include "Locus2d.h"
#include <array>

/**
 Sprite data and a few representations for that data.

 SpriteData: holds information about where a graphic lives on a sheet
 Sprite: renderable sprite
 SpriteAnimation: renderable sequence of sprites
*/
namespace pockets
{
  /**
   SpriteData holds information about a sprite's dimensions and texture coordinates.
  */
  class SpriteData
  {
  public:
    SpriteData(){}; // need default constructor to use in a std::map
    //! construct sprite data from its pixel size and normalized texture coordinates
    SpriteData( const ci::Vec2i &pixel_size, const ci::Rectf &textureBounds, const ci::Vec2i &registration_point=ci::Vec2i::zero() );
    //! returns the Sprite's pixel dimensions
    inline ci::Vec2i getSize() const { return mSize; }
    //! returns the normalized texture coordinates of the sprite graphic
    inline ci::Rectf getTextureBounds() const { return mTextureBounds; }
    //! returns the registration point of the sprite, treated as the origin of the artwork
    inline ci::Vec2i getRegistrationPoint() const { return mRegistrationPoint; }
  private:
    ci::Vec2i mRegistrationPoint;
    ci::Vec2i mSize = ci::Vec2i::zero();
    ci::Rectf mTextureBounds = ci::Rectf(0,0,0,0);
  };

  /**
   Sprite renders a textured rectangle based on SpriteData specifications.
  */
  typedef std::shared_ptr<class Sprite> SpriteRef;
  class Sprite : public SimpleRenderer::IRenderable
  {
  public:
    Sprite() = default;
    Sprite( const SpriteData &data );
    ~Sprite();
    //! clip region with rectangle; only parts contained by rect will be visible
    void clipBy( const ci::Rectf &rect );
    //! set portion of texture to render (used by clipBy)
    void setRegion( const ci::Rectf &portion );
    //! render the sprite to screen
    void render();
    Locus2d& getLocus(){ return mLocus; }
  private:
    SpriteData                mData;
    Locus2d                   mLocus;
    std::array<GLfloat, 8>    mPositions;
    std::array<GLfloat, 8>    mTexCoords;
  };

  /**
   SpriteWithTiming is a SpriteData wrapper to include animation timing.
   I want to rename this.
  */
  struct SpriteWithTiming
  {
    SpriteWithTiming( const SpriteData &sprite, float start, float finish ):
    sprite( sprite )
    , start( start )
    , finish( finish )
    {}
    SpriteData sprite;
    float start;
    float finish;
  };

  /**
   SpriteAnimations hold a collection of SpriteData and render the appropriate
   Sprite given the current time.
  */
  typedef std::shared_ptr<class SpriteAnimation> SpriteAnimationRef;
  class SpriteAnimation : public SimpleRenderer::IRenderable
  {
  public:
    SpriteAnimation();
    SpriteAnimation( const std::vector<SpriteWithTiming> &sprites );
    const SpriteData& currentSprite(){ return currentData().sprite; }
    const SpriteWithTiming& currentData(){ return mData.at(mCurrentIndex); }
    //! add a frame at end of animation
    void addFrame( const SpriteData &sprite, float duration );
    //! update the animation
    void update( float deltaTime );
    //! render the current frame of animation
    void render();
    Locus2d& getLocus(){ return mLocus; }
  private:
    typedef std::vector<SpriteWithTiming>::iterator DataIter;
    Locus2d                             mLocus;
    std::vector<SpriteWithTiming>       mData;
    size_t                              mCurrentIndex;
    float                               mTime = 0;
    float                               mDuration;
    bool                                mLooping = true;
    std::array<GLfloat, 8>              mPositions;
    std::array<GLfloat, 8>              mTexCoords;
    void nextFrame();
  };

} // pockets::