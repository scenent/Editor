#include "glyph_texture.h"

GlyphTexture::GlyphTexture(unsigned int _charData, unsigned char* _data, int _width, int _height,
	int _channels, const vec2& _bearing, unsigned int _advanceX, bool _pixelFilter)
	: Texture(_data, _width, _height, _channels, _pixelFilter, false), m_characterData(_charData),
	m_bearing(_bearing), m_advanceX(_advanceX)
{

}

GlyphTexture::~GlyphTexture() {
	Texture::~Texture();
}