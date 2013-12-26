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

#include "stdafx.h"
#ifndef PRECOMPILED_HEADERS_ARE_AVAILABLE
#endif
#include "Font.h"
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftoutln.h>
#include <freetype/ftstroke.h>

namespace ONSlaught{

class FreeType_Lib{
	FT_Library library;
	FreeType_Lib();
	FreeType_Lib(const FreeType_Lib &){}
	~FreeType_Lib();
public:
	static FreeType_Lib instance;
	FT_Library get_lib() const{ return this->library; }
};

class Font{
	FT_Face ft_font;
	FT_Stream stream;
	FT_Error error;
	unsigned size;

	Font(const Font &){}
	void operator=(const Font &){}
public:
	unsigned ascent,
		line_skip;

	Font(const std::wstring &filename);
	~Font();
	bool good() const{ return !this->error; }
	FT_Error get_error() const{ return this->error; }
	bool check_flag(unsigned flag) const{
		return (this->ft_font->face_flags&flag)==flag;
	}
	bool is_monospace() const;
	void set_size(unsigned size);
	bool get_glyph(FT_GlyphSlot &output, wchar_t codepoint,bool italic,bool bold) const;
	FT_GlyphSlot render_glyph(wchar_t codepoint,bool italic,bool bold) const;
	FT_GlyphSlot render_glyph(FT_GlyphSlot) const;
	FT_Face get_font() const{ return this->ft_font; }
};

union pixel{
	byte_t array[4];
	uint32_t i32;
};

struct Color{
	byte_t rgba[4];
	Color(byte_t r, byte_t g, byte_t b, byte_t a = 0xFF){
		this->rgba[0] = r;
		this->rgba[1] = g;
		this->rgba[2] = b;
		this->rgba[3] = a;
	}
	Color(uint32_t color = 0){
		*this = color;
	}
	const Color &operator=(uint32_t color){
		this->rgba[0] = (color&0xFF0000)>>16;
		this->rgba[1] = (color&0xFF00)>>8;
		this->rgba[2] = color&0xFF;
		this->rgba[3] = 0xFF;
		return *this;
	}
	uint32_t to_rgb(){
		uint32_t r=
			(this->rgba[0]<<16)|
			(this->rgba[1]<<8)|
			this->rgba[2];
		return r;
	}
	uint32_t to_rgba() const{
		uint32_t r=
			(this->rgba[0]<<24)|
			(this->rgba[1]<<16)|
			(this->rgba[2]<<8)|
			this->rgba[3];
		return r;
	}
	void from_rgba(uint32_t color){
		this->rgba[0]=(color&0xFF000000)>>24;
		this->rgba[1]=(color&0xFF0000)>>16;
		this->rgba[2]=(color&0xFF00)>>8;
		this->rgba[3]=color&0xFF;
	}
	uint32_t to_native(byte_t *format) const{
		pixel r;
		r.array[0] = this->rgba[format[0]];
		r.array[1] = this->rgba[format[1]];
		r.array[2] = this->rgba[format[2]];
		r.array[3] = this->rgba[format[3]];
		return r.i32;
	}
	bool operator==(const Color &b) const{
		byte_t f[] = { 0, 1, 2, 3 };
		return this->to_native(f) == b.to_native(f);
	}
	bool operator!=(const Color &b) const{
		return !(*this==b);
	}
	Color operator*(double factor) const{
		Color r = *this;
		r.rgba[0] = byte_t(double(r.rgba[0])*factor);
		r.rgba[1] = byte_t(double(r.rgba[1])*factor);
		r.rgba[2] = byte_t(double(r.rgba[2])*factor);
		return r;
	}
	static Color white,
		black,
		black_transparent,
		red,
		green,
		blue;
};

template <typename T>
struct BasicRect{
	T x,y,w,h;
	BasicRect<T>(T x=0,T y=0,T w=0,T h=0):x(x),y(y),w(w),h(h){}
	BasicRect<T>(const BasicRect<T> &s):x(s.x),y(s.y),w(s.w),h(s.h){}
	template <typename T2>
	explicit BasicRect<T>(const T2 &s):x((T)s.x),y((T)s.y),w((T)s.w),h((T)s.h){}
	template <typename T2>
	BasicRect<T> &operator=(const T2 &s){
		this->x=(T)s.x;
		this->y=(T)s.y;
		this->w=(T)s.w;
		this->h=(T)s.h;
		return *this;
	}
	BasicRect<T> &operator=(const BasicRect<T> &s){
		return this->operator=<BasicRect<T> >(s);
	}
	BasicRect<T> intersect(const BasicRect<T> &b){
		BasicRect<T> r(
			std::max(this->x,b.x),
			std::max(this->y,b.y),
			std::min(this->x+this->w,b.x+b.w),
			std::min(this->y+this->h,b.y+b.h)
		);
		r.w-=r.x;
		r.h-=r.y;
		return r;
	}
	bool point_is_inside(T x,T y){
		return (x>=this->x && y>=this->y && x<this->x+this->w && y<this->y+this->h);
	}
	bool point_is_inside(const BasicRect<T> &b){
		return (b.x>=this->x && b.y>=this->y && b.x<this->x+this->w && b.y<this->y+this->h);
	}
};
typedef BasicRect<float> Rect;
typedef BasicRect<long> LongRect;

class FontCache;

class Glyph{
public:
	FontCache &fc;
	wchar_t codepoint;
	//style properties:
	unsigned size,
		outline_size;
	Color color,
		outline_color;
	bool italic,
		bold;
	//~style properties
	Uint8 *base_bitmap,
		*outline_base_bitmap;
	LongRect bounding_box,
		outline_bounding_box;
	unsigned advance;
public:
	unsigned refCount,
		real_outline_size;
	Glyph(
		FontCache &fc,
		wchar_t codepoint,
		unsigned size,
		const Color &color,
		bool italic,
		bool bold,
		unsigned outline_size,
		const Color &outline_color
	);
	~Glyph();
	bool operator<(const Glyph &b) const{ return this->codepoint<b.codepoint; }
	void setColor(const Color &color){ this->color=color; }
	void setOutlineColor(const Color &color){ this->outline_color=color; }
	unsigned get_advance_fixed() const{ return this->advance; }
	const LongRect &get_bounding_box() const{ return this->bounding_box; }
	LongRect get_put_bounding_box(long x,long y) const{
		LongRect ret=(!this->outline_base_bitmap)?this->bounding_box:this->outline_bounding_box;
		ret.x+=x;
		ret.y+=y;
		return ret;
	}
	wchar_t get_codepoint() const{ return this->codepoint; }
	bool needs_redraw(unsigned size,bool italic,bool bold,unsigned outline_size) const;
	long get_advance() const;
	const FontCache &get_cache() const{ return this->fc; }
	FontCache &get_cache(){ return this->fc; }
	void done();
	unsigned type(){ return 0; }
};

class FontCache{
	std::map<wchar_t, Glyph *> glyphs;
	Font &font;
	unsigned size,
		outline_size;
	Color color,
		outline_color;
	bool italic,
		bold;
	std::set<Glyph *> garbage;
public:
	long spacing;
	unsigned line_skip,
		font_line_skip,
		ascent;
	FontCache(Font &font,unsigned size,const Color &color,bool italic,bool bold,unsigned outline_size,const Color &outline_color);
	FontCache(const FontCache &fc);
	~FontCache();
	void reset_style(unsigned size,bool italic,bool bold,unsigned outline_size);
	void set_outline_size(unsigned size){ this->outline_size=size; }
	void set_size(unsigned size);
	void set_to_normal(){
		this->italic=0;
		this->bold=0;
	}
	void set_italic(bool i){ this->italic=i; }
	void set_bold(bool b){ this->bold=b; }
	void set_color(const Color &color){ this->color=color; }
	void set_outline_color(const Color &color){ this->outline_color=color; }
	Glyph *get_glyph(wchar_t c);
	void done(Glyph *g);
	const Font &get_font() const{ return this->font; }
	Font &get_font(){ return this->font; }
	unsigned get_size() const{ return this->size; }
	const Color &get_color() const{ return this->color; }
	bool get_italic() const{ return this->italic; }
	bool get_bold() const{ return this->bold; }
};

FreeType_Lib FreeType_Lib::instance;

FreeType_Lib::FreeType_Lib(){
	if (FT_Init_FreeType(&this->library)){
		//throw std::string("FT_Init_FreeType() has failed!");
		exit(1);
	}
}

FreeType_Lib::~FreeType_Lib(){
	FT_Done_FreeType(this->library);
}

unsigned long FT_Stream_IoFunc(FT_Stream s, unsigned long offset, unsigned char *buffer, unsigned long count){
	auto stream = (std::ifstream *)s->descriptor.pointer;
	if (!count)
		return 0;
	stream->seekg(offset);
	stream->read((char *)buffer, count);
	return count;
}

void FT_Stream_CloseFunc(FT_Stream s){
	delete (std::ifstream *)s->descriptor.pointer;
}

Font::Font(const std::wstring &filename){
	this->error = 1;
	auto stream = new std::ifstream(filename.c_str(), std::ios::binary|std::ios::ate);
	if (!*stream){
		delete stream;
		return;
	}
	auto size = stream->tellg();
	stream->seekg(0);
	this->stream = new FT_StreamRec;
	memset(this->stream, 0, sizeof(*this->stream));
	this->stream->descriptor.pointer = stream;
	this->stream->read = FT_Stream_IoFunc;
	this->stream->close = FT_Stream_CloseFunc;
	this->stream->size = size;
	FT_Open_Args args;
	args.flags = FT_OPEN_STREAM;
	args.stream = this->stream;
	this->error = FT_Open_Face(FreeType_Lib::instance.get_lib(), &args, 0, &this->ft_font);
	this->size = 0;
}

Font::~Font(){
	if (this->good())
		FT_Done_Face(this->ft_font);
	delete (std::ifstream *)this->stream->descriptor.pointer;
	delete this->stream;
}

template <typename T>
inline T FT_CEIL(T x){
	return (x + 63) / 64;
}

void Font::set_size(unsigned size){
	if (this->size == size)
		return;
	this->size = size;
	FT_Set_Pixel_Sizes(this->ft_font, 0, size);
	FT_Fixed scale = this->ft_font->size->metrics.y_scale;
	this->ascent = FT_CEIL(FT_MulFix(this->ft_font->ascender, scale));
	this->line_skip = FT_CEIL(FT_MulFix(this->ft_font->height, scale));
}

bool Font::is_monospace() const{
	return this->check_flag(FT_FACE_FLAG_FIXED_WIDTH);
}

bool Font::get_glyph(FT_GlyphSlot &output, wchar_t codepoint, bool italic, bool bold) const{
	auto ci = FT_Get_Char_Index(this->ft_font, codepoint);
	if (!ci)
		return 0;
	FT_Load_Glyph(this->ft_font, ci, FT_LOAD_FORCE_AUTOHINT);
	if (italic){
		FT_Matrix shear;
		shear.xx = 0x10000;
		shear.xy = 0x34FE; //~0.207
		shear.yx = 0;
		shear.yy = 0x10000;
		FT_Outline_Transform(&this->ft_font->glyph->outline, &shear);
	}
	if (bold)
		FT_Outline_Embolden(&this->ft_font->glyph->outline, FT_Pos(this->size * 16 / 9));
	output = this->ft_font->glyph;
	return 1;
}

FT_GlyphSlot Font::render_glyph(wchar_t codepoint,bool italic,bool bold) const{
	FT_GlyphSlot gs;
	this->get_glyph(gs, codepoint, italic, bold);
	return this->render_glyph(gs);
}

FT_GlyphSlot Font::render_glyph(FT_GlyphSlot slot) const{
	FT_Render_Glyph(slot, FT_RENDER_MODE_LIGHT);
	return this->ft_font->glyph;
}

struct Span{
	int x, y, w, alpha;
	Span(){}
	Span(int x, int y, int w, int coverage): x(x), y(y), w(w), alpha(coverage){}
};

typedef std::vector<Span> Spans;

void RasterCallback(int y,int count,const FT_Span *spans,void *user) {
	for (int i = 0; i < count; i++){
		Span s(spans[i].x, y, spans[i].len, spans[i].coverage);
		((Spans *)user)->push_back(s);
	}
}

void RenderSpans(FT_Library library,FT_Outline *outline,Spans *spans){
	FT_Raster_Params params;
	memset(&params,0,sizeof(params));
	params.flags = FT_RASTER_FLAG_AA|FT_RASTER_FLAG_DIRECT;
	params.gray_spans = RasterCallback;
	params.user = spans;
	FT_Outline_Render(library, outline, &params);
}

Uint8 *render_glyph(LongRect &box, FT_Glyph &glyph, int ascent, float outline_size){
	FT_Stroker stroker;
	FT_Stroker_New(FreeType_Lib::instance.get_lib(), &stroker);
	FT_Stroker_Set(stroker, FT_Fixed(outline_size)*64, FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_BEVEL, 0);
	FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);

	Spans outlineSpans;
	RenderSpans(FreeType_Lib::instance.get_lib(), &FT_OutlineGlyph(glyph)->outline, &outlineSpans);
	FT_Stroker_Done(stroker);

	if (!outlineSpans.size())
		return 0;
	int minx = outlineSpans.front().x,
		miny = outlineSpans.front().y,
		maxx = outlineSpans.front().x + outlineSpans.front().w,
		maxy = outlineSpans.front().y;
	for (size_t a=1;a<outlineSpans.size();a++){
		minx = std::min(minx, outlineSpans[a].x);
		miny = std::min(miny, outlineSpans[a].y);
		maxx = std::max(maxx, outlineSpans[a].x+outlineSpans[a].w);
		maxy = std::max(maxy, outlineSpans[a].y);
	}
	box.x = minx;
	box.y = ascent - maxy - 1;
	box.w = maxx - minx + 1;
	box.h = maxy - miny + 1;
	size_t bmsize = box.w * box.h;
	if (!bmsize)
		return 0;
	Uint8 *bitmap = new Uint8[bmsize];
	memset(bitmap, 0, bmsize);
	for (size_t a = 0; a < outlineSpans.size(); a++){
		Span &span = outlineSpans[a];
		unsigned element = span.x - minx + (box.h - (span.y - miny) - 1) * box.w;
		Uint8 *dst = bitmap + element;
		for (int b = 0; b < span.w; b++)
			*dst++ = (Uint8)span.alpha;
	}
	return bitmap;
}

Glyph::Glyph(
		FontCache &fc,
		wchar_t codepoint,
		unsigned size,
		const Color &color,
		bool italic,
		bool bold,
		unsigned outline_size,
		const Color &outline_color)
			:fc(fc){
	this->codepoint=codepoint;
	this->size=size;
	this->outline_size=outline_size;
	this->color=color;
	this->outline_color=outline_color;
	this->refCount=0;
	this->italic=italic;
	this->bold=bold;
	Font &font = fc.get_font();
	font.set_size(size);

	FT_GlyphSlot glyph_slot;
	if (!font.get_glyph(glyph_slot, codepoint,italic,bold)){
		this->base_bitmap = 0;
		this->outline_base_bitmap = 0;
		return;
	}
	this->real_outline_size = 0;
	if (outline_size){
		FT_Glyph glyph;
		FT_Get_Glyph(glyph_slot,&glyph);
		float temp_os=float(outline_size)/18.f*float(size);
		this->real_outline_size=(unsigned)ceil(temp_os);
		this->outline_base_bitmap=render_glyph(this->outline_bounding_box,glyph,fc.ascent,temp_os);
		FT_Done_Glyph(glyph);
	}else
		this->outline_base_bitmap=0;
	
	glyph_slot=font.render_glyph(glyph_slot);
	FT_Bitmap &bitmap=glyph_slot->bitmap;
	unsigned width=bitmap.width,
		height=bitmap.rows;
	Uint8 *dst=this->base_bitmap=new Uint8[1+width*height];
	for (unsigned y=0;y<(unsigned)bitmap.rows;y++){
		Uint8 *src=bitmap.buffer+y*bitmap.pitch;
		for (unsigned x=0;x<(unsigned)bitmap.width;x++)
			*dst++=*src++;
	}

	this->bounding_box.x=glyph_slot->bitmap_left;
	this->bounding_box.y=Sint16(font.ascent-glyph_slot->bitmap_top);
	this->bounding_box.w=(Uint16)width;
	this->bounding_box.h=(Uint16)height;
	this->advance=FT_CEIL(glyph_slot->metrics.horiAdvance);

	if (outline_size && !this->outline_base_bitmap){
		this->outline_bounding_box=this->bounding_box;
		size_t buffer_size=this->outline_bounding_box.w*this->outline_bounding_box.h+1;
		this->outline_base_bitmap=new Uint8[buffer_size];
		memset(this->outline_base_bitmap,0,buffer_size);
	}
}

Glyph::~Glyph(){
	delete[] this->base_bitmap;
	delete[] this->outline_base_bitmap;
}

bool Glyph::needs_redraw(unsigned size,bool italic,bool bold,unsigned outline_size) const{
	return (this->size != size || this->italic != italic || this->bold != bold || this->outline_size != outline_size);
}

long Glyph::get_advance() const{
	return long(this->advance) + this->fc.spacing;
}

void Glyph::done(){
	this->fc.done(this);
}

FontCache::FontCache(Font &f, unsigned size, const Color &color, bool italic, bool bold, unsigned outline_size, const Color &outline_color): font(f){
	this->set_color(color);
	this->set_outline_color(outline_color);
	this->reset_style(size, italic, bold, outline_size);
	this->spacing = 0;
}

FontCache::FontCache(const FontCache &fc): font(fc.font){
	this->set_color(fc.color);
	this->set_outline_color(fc.outline_color);
	this->reset_style(fc.size, fc.italic, fc.bold, fc.outline_size);
	this->spacing = fc.spacing;
	this->line_skip = fc.line_skip;
	this->font_line_skip = fc.font_line_skip;
	this->ascent = fc.ascent;
}

FontCache::~FontCache(){
	unsigned count = 0;
	for (auto &p : this->glyphs){
		count += p.second->refCount;
		delete p.second;
	}
	for (auto p : this->garbage){
		count += p->refCount;
		delete p;
	}
	if (count){
		std::cerr <<"FontCache::~FontCache(): Warning: "<<count<<" possible dangling references.\n";
	}
}

void FontCache::set_size(unsigned size){
	this->size = size;
	this->font.set_size(size);
	this->font_line_skip = this->line_skip = this->font.line_skip;
	this->ascent = this->font.ascent;
}

void FontCache::reset_style(unsigned size,bool italic,bool bold,unsigned outline_size){
	this->set_size(size);
	this->set_italic(italic);
	this->set_bold(bold);
	this->set_outline_size(outline_size);
}

#define INDENTATION_CHARACTER 0x2003

Glyph *FontCache::get_glyph(wchar_t c){
	if (c == '\t')
		c = INDENTATION_CHARACTER;
	if (c < 32)
		return 0;
	bool must_render = (this->glyphs.find(c) == this->glyphs.end());
	Glyph *&g = this->glyphs[c];
	if (!must_render && g->needs_redraw(this->size, this->italic, this->bold, this->outline_size)){
		if (!g->refCount)
			delete g;
		else
			this->garbage.insert(g);
		must_render = 1;
	}
	if (must_render)
		g = new Glyph(*this,c, this->size, this->color, this->italic, this->bold, this->outline_size, this->outline_color);
	g->setColor(this->color);
	g->setOutlineColor(this->outline_color);
	g->refCount++;
	return g;
}

void FontCache::done(Glyph *g){
	if (!g)
		return;
	std::map<wchar_t, Glyph *>::iterator i = this->glyphs.find(g->get_codepoint());
	if (i != this->glyphs.end()){
		if (i->second != g){
			std::set<Glyph *>::iterator i2 = this->garbage.find(g);
			if (i2 != this->garbage.end()){
				(*i2)->refCount--;
				if (!(*i2)->refCount){
					delete *i2;
					this->garbage.erase(i2);
				}
			}
		}else
			i->second->refCount--;
	}
	//otherwise, the glyph doesn't belong to this cache
}

}

using ONSlaught::GlyphAtlas;
using ONSlaught::GlyphCollection;

GlyphAtlas::GlyphAtlas(): size(default_size, default_size), bitmap(default_size * default_size, 0){
}

struct glyph_sorter{
	bool operator()(ONSlaught::Glyph *a, ONSlaught::Glyph *b) const{
		return a->bounding_box.h >  b->bounding_box.h || 
		       a->bounding_box.h == b->bounding_box.h && a->bounding_box.w < b->bounding_box.w;
	}
};

GlyphCollection::GlyphCollection(const path_t &path, const FontStyle &){
	Font f(path);
	FontCache cache(f, 45, Color(0xFFFFFFF), 1, 0, 0, Color(0xFFFFFFF));
	const unsigned total_w = 1024;
	const unsigned total_h = 1024;
	std::vector<Glyph *> glyphs;
	for (int i = 0; i < 0x10000; i++){
		//std::cout <<i<<std::endl;
		auto glyph = cache.get_glyph(i);
		if (!glyph)
			continue;
		if (!glyph->bounding_box.w || !glyph->bounding_box.h){
			cache.done(glyph);
			continue;
		}
		glyphs.push_back(glyph);
	}
	std::sort(glyphs.begin(), glyphs.end(), glyph_sorter());
	Uint8 *buffer = new Uint8[total_w * total_h * 4];
	for (int i = 0; i < total_w * total_h * 4; i++)
		buffer[i] = i % 4 == 1 ? 0x00 : 0xFF;
	unsigned padding = 1;
	unsigned remaining_w = total_w - padding;
	unsigned remaining_h = total_h - padding;
	unsigned x = padding,
	         y = padding;
	unsigned max = 0;
	for (size_t i = 0; i < glyphs.size(); i++){
		auto glyph = glyphs[i];
loop:
		if (remaining_h < glyph->bounding_box.h + 2){
			for (; i < glyphs.size(); i++)
				cache.done(glyphs[i]);
			break;
		}
		if (remaining_w < glyph->bounding_box.w + 2){
			y += max + padding*2;
			remaining_h -= max + padding*2;
			x = padding;
			remaining_w = total_w - padding;
			max = 0;
			goto loop;
		}
		for (int y0 = 0; y0 < glyph->bounding_box.h; y0++){
			Uint8 *row = buffer + (x + (y + y0) * total_w) * 4;
			for (int x0 = 0; x0 < glyph->bounding_box.w; x0++){
				Uint8 *pixel = row + x0 * 4;
				Uint8 color = glyph->base_bitmap[x0 + y0 * glyph->bounding_box.w];
				pixel[0] = color;
				pixel[1] = color;
				pixel[2] = color;
				pixel[3] = 0xff;
			}
		}
		x += glyph->bounding_box.w + padding*2;
		remaining_w -= glyph->bounding_box.w + padding*2;
		max = std::max(max, (unsigned)glyph->bounding_box.h);
		cache.done(glyph);
	}
}

GlyphCollection::~GlyphCollection(){
	for (auto p : this->atlases)
		delete p;
}
