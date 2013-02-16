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

#include "ImagePacker.h"
#include "cinder/Text.h"
#include "cinder/ip/Trim.h"

using namespace ci;
using namespace std;
using namespace pockets;

ImagePacker::ImagePacker()
{}

ImagePacker::~ImagePacker()
{}

void ImagePacker::addImage( const std::string &id, ci::Surface surface, bool trim_alpha )
{
  if( trim_alpha )
  {
    Area bounds = ip::findNonTransparentArea( surface, surface.getBounds() );
    assert( bounds.getWidth() <= surface.getWidth() );
    assert( bounds.getHeight() <= surface.getHeight() );
    Surface trimmed_copy( bounds.getWidth(), bounds.getHeight(), true, SurfaceChannelOrder::RGBA );
    trimmed_copy.copyFrom( surface, bounds, -bounds.getUL() );
    surface = trimmed_copy;
  }
  mImages.push_back( ImageData( surface, id ) );
}

void ImagePacker::addGlyphs( const ci::Font &font, const std::string &glyphs, bool trim_alpha )
{
  for( const char glyph : glyphs )
  {
    addString( toString<char>(glyph), font, toString<char>(glyph), trim_alpha );
  }
}

void ImagePacker::addString( const string &id, const Font &font, const string &str, bool trim_alpha )
{
  TextLayout layout;
  layout.clear( ColorA( 0, 0, 0, 0 ) );
  layout.setFont( font );
  layout.setColor( Color::white() );
  layout.addLine( str );
  Surface image = layout.render( true );
  addImage( id, image, trim_alpha );
}

JsonTree ImagePacker::surfaceDescription()
{
  JsonTree description;
  JsonTree metaData = JsonTree::makeObject("meta");
  metaData.pushBack( JsonTree("created_by", "David Wicks' pockets::ImagePacker") );
  metaData.pushBack( JsonTree("width", mWidth ) );
  metaData.pushBack( JsonTree("height", mHeight ) );
  description.pushBack( metaData );

  JsonTree sprites = JsonTree::makeArray("sprites");
  for( ImageData &sprite : mImages )
  {
    sprites.pushBack( sprite.toJson() );
  }
  description.pushBack( sprites );

  return description;
}

Surface ImagePacker::packedSurface()
{
  Surface output( mWidth, mHeight, true, SurfaceChannelOrder::RGBA );
  for( ImageData &sprite : mImages )
  {
    output.copyFrom( sprite.getSurface(), sprite.getBounds(), sprite.getLoc() );
  }
  return output;
}

void ImagePacker::calculatePositions()
{
//  auto rev_area_compare = []( const ImageData &lhs, const ImageData &rhs )
//  {
//    return lhs.getBounds().calcArea() > rhs.getBounds().calcArea();
//  };
  auto rev_height_compare = []( const ImageData &lhs, const ImageData &rhs )
  {
    return lhs.getBounds().getHeight() > rhs.getBounds().getHeight();
  };
  sort( mImages.begin(), mImages.end(), rev_height_compare );
  Vec2i loc( 0, 0 );
  Vec2i padding( 1, 1 );
  int bottom_y = 0;
  for( ImageData &sprite : mImages )
  {
    if( loc.x + sprite.getBounds().getWidth() > mWidth )
    {
      loc.y = bottom_y + padding.y;
      loc.x = 0;
    }
    sprite.setLoc( loc );
    loc.x += sprite.getBounds().getWidth() + padding.x;
    bottom_y = math<int>::max( sprite.getBounds().getHeight() + loc.y, bottom_y );
  }
  mHeight = bottom_y;
}

void ImagePacker::draw()
{
  for( ImageData &sprite : mImages )
  {
    sprite.draw();
  }
}



