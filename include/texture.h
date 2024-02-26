#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GLAD/glad.h>
#include "math_utils.h"

class Texture {
protected:
	std::string m_filePath = "";
	uint32_t m_textureID = 0;
	bool m_pixelFilter = true;
	vec2i m_size = vec2i(0, 0);
public:
	bool loadSuccess = false;
public:
	Texture(unsigned char* _data, int _width, int _height, int _channels, bool _pixelFilter = true, bool _repeatTex = true);
	virtual ~Texture();
	void bind(int _offset = 0) const;
public:
	const uint32_t& getID() const;
	const vec2i& getSize() const;
	const bool& getPixelFilterEnabled() const;
	const std::string& getFilePath() const;
};








#endif