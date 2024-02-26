#include "texture.h"
#include "macros.h"
#include <vector>


Texture::Texture(unsigned char* _data, int _width, int _height, int _channels, bool pixelFilter, bool _repeatTex) {
	m_size.x = _width;
	m_size.y = _height;
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	if (_repeatTex) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	}
	if (_repeatTex) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_pixelFilter == true ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_pixelFilter == true ? GL_LINEAR : GL_NEAREST);
	if (_channels == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, _width, _height, 0, GL_RED, GL_UNSIGNED_BYTE, _data);
	else if (_channels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, _data);
	else if (_channels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _data);
	else {
		PUSH_ERROR(("Invalid Channels : " + std::to_string(_channels)).c_str());
		return;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture() {
	glDeleteTextures(1, &m_textureID);
}

void Texture::bind(int _offset) const {
	glActiveTexture(GL_TEXTURE0 + _offset);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

const uint32_t& Texture::getID() const {
	return m_textureID;
}
const vec2i& Texture::getSize() const {
	return m_size;
}
const bool& Texture::getPixelFilterEnabled() const {
	return m_pixelFilter;
}
const std::string& Texture::getFilePath() const {
	return m_filePath;
}