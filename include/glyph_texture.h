#ifndef GLYPH_TEXTURE_H
#define GLYPH_TEXTURE_H


#include "texture.h"
#include "math_utils.h"


class GlyphTexture final : public Texture{
private:
	unsigned int m_characterData;
	vec2         m_bearing;
	unsigned int m_advanceX;
public:
	GlyphTexture(unsigned int _charData, unsigned char* _data, int _width, int _height,
		int _channels, const vec2& _bearing, unsigned int _advanceX, bool pixelFilter = true);
	~GlyphTexture() override;
	const unsigned int& getCharData() const { return m_characterData; }
	const vec2& getBearing() const { return m_bearing;  }
	const unsigned int& getAdvanceX() const { return m_advanceX;  }
};







#endif