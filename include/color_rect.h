#ifndef COLOR_RECT_H
#define COLOR_RECT_H

#include "math_utils.h"
#include <glad/glad.h>

class ColorRect final {
private:
	class Camera* m_camera = nullptr;
	class Shader* m_shader = nullptr;
	vec2 m_position = vec2(0, 0);
	vec2i m_size = vec2i(40, 40);
	vec4 m_color = vec4(1, 1, 1, 1);
	bool m_visible = true;
	bool m_ignoreViewMatrix = false;
public:
	ColorRect(class Camera* _cam);
	~ColorRect();
	void draw() const;
public:
	void setPosition(const vec2& _pos);
	const vec2& getPosition();
	void setSize(const vec2i& _size);
	const vec2i& getSize();
	void setColor(const vec4& _color);
	const vec4& getColor();
	void setVisible(const bool& flag);
	void setIgnoreViewMatrix(const bool& flag);
};

class Singleton4ColorRect{
private:
	Singleton4ColorRect() {

	}
public:
	static Singleton4ColorRect& get() {
		static Singleton4ColorRect instance;
		return instance;
	}
	unsigned int m_VAO, m_VBO, m_EBO;
	~Singleton4ColorRect() {
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
		glDeleteVertexArrays(1, &m_VAO);
	}
};










#endif