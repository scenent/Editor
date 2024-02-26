#include "color_rect.h"
#include "utils.h"
#include "macros.h"
#include "shader.h"
#include "camera.h"

ColorRect::ColorRect(Camera* _cam) : m_camera(_cam) {
	if (Singleton4ColorRect::get().m_VAO == 0) {
		float vertexBuffer[] = {
			-0.5f,  0.5f, 0.0f,
			0.5f,  0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
		};
		unsigned int indexBuffer[] = {
			0, 1, 2,
			2, 3, 0
		};
		glGenVertexArrays(1, &Singleton4ColorRect::get().m_VAO);
		glBindVertexArray(Singleton4ColorRect::get().m_VAO);
		glGenBuffers(1, &Singleton4ColorRect::get().m_VBO);
		glBindBuffer(GL_ARRAY_BUFFER, Singleton4ColorRect::get().m_VBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(float), vertexBuffer, GL_STATIC_DRAW);
		glGenBuffers(1, &Singleton4ColorRect::get().m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Singleton4ColorRect::get().m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indexBuffer, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	}
	m_shader = new Shader(readFile(COLOR_RECT_VERTEX_SHADER_PATH).c_str(), readFile(COLOR_RECT_FRAGMENT_SHADER_PATH).c_str());
}
ColorRect::~ColorRect() {

}
void ColorRect::draw() const {
	if (!m_visible) return;
	glSetRenderMode(GLRenderMode::GL2D);
	m_shader->use();

	glBindVertexArray(Singleton4ColorRect::get().m_VAO);

	std::vector<float> worldMatrix = {
		float(m_size.x), 0.0f, 0.0f, m_position.x,
		0.0f, float(m_size.y), 0.0f, -m_position.y,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	m_shader->setMat4("WorldMatrix", worldMatrix.data());
	if (m_ignoreViewMatrix) {
		std::vector<float> temp = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		m_shader->setMat4("ViewMatrix", temp.data());
	}
	else {
		m_shader->setMat4("ViewMatrix", m_camera->getViewMatrix().data());
	}
	m_shader->setMat4("ProjectionMatrix", m_camera->getProjectionMatrix().data());
	m_shader->setVec4("color", m_color);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	m_shader->unuse();
	glBindVertexArray(0);
}

void ColorRect::setPosition(const vec2& _pos){
	m_position = _pos;
}
const vec2& ColorRect::getPosition() {
	return m_position;
}
void ColorRect::setSize(const vec2i& _size) {
	m_size = _size;
}
const vec2i& ColorRect::getSize() {
	return m_size;
}
void ColorRect::setColor(const vec4& _color) {
	m_color = _color;
}
const vec4& ColorRect::getColor() {
	return m_color;
}

void ColorRect::setVisible(const bool& flag) {
	m_visible = flag;
}

void ColorRect::setIgnoreViewMatrix(const bool& flag) {
	m_ignoreViewMatrix = flag;
}