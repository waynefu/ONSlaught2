/*
* Copyright (c) 2013-2014, Helios (helios.vmg@gmail.com)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, 
*       this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * The name of the author may not be used to endorse or promote products
*       derived from this software without specific prior written permission.
*     * Products derived from this software may not be called "ONSlaught" nor
*       may "ONSlaught" appear in their names without specific prior written
*       permission from the author. 
*
* THIS SOFTWARE IS PROVIDED BY HELIOS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL HELIOS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef _MSC_VER
#pragma once
#endif

#ifndef FONT_H
#define FONT_H

#include "Math.h"
#include "IO.h"
#include "SimpleTypes.h"

namespace ONSlaught{

class GlyphAtlas;

class BitmappedGlyph{
	typedef Math::IntegerSizeVector2 isv;
	typedef Math::IntegerPositionVector2 ipv;
	wchar_t codepoint;
	ipv bitmap_position;
	isv size;
	GlyphAtlas *holder;
public:
	BitmappedGlyph(){}
	BitmappedGlyph(wchar_t codepoint, const ipv &bitmap_position, const isv &size, GlyphAtlas *holder):
		codepoint(codepoint),
		bitmap_position(bitmap_position),
		size(size),
		holder(holder){}
	bool operator<(const BitmappedGlyph &b) const{
		return this->codepoint < b.codepoint;
	}
};

class GlyphAtlas{
	static const unsigned default_size = 1024;
	Math::IntegerSizeVector2 size;
	std::vector<byte_t> bitmap;
public:
	GlyphAtlas();
};

class FontStyle{
	unsigned size;
	bool italic;
	bool bold;
	unsigned outline_size;
};

class GlyphCollection{
	std::vector<BitmappedGlyph> glyphs;
	std::vector<GlyphAtlas *> atlases;
public:
	GlyphCollection(const path_t &path, const FontStyle &);
	~GlyphCollection();
	BitmappedGlyph *find(wchar_t c);
};

}

#endif
