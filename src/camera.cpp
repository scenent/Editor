#include "camera.h"
#include <algorithm>

Camera::Camera(int _screenWidth, int _screenHeight) {
	m_projectionMatrix[0] = 2.0f / _screenWidth;
	m_projectionMatrix[5] = 2.0f / _screenHeight;
}
Camera::~Camera() {

}
void Camera::setWindowSize(int _width, int _height) {
	m_projectionMatrix[0] = 2.0f / _width;
	m_projectionMatrix[5] = 2.0f / _height;
}

const std::vector<float>& Camera::getViewMatrix() const {
	return m_viewMatrix;
}
const std::vector<float>& Camera::getProjectionMatrix() const{
	return m_projectionMatrix;
}

void Camera::setZoom(const vec2& _zoom) {
	m_zoom = _zoom;
	m_zoom.x = std::clamp(m_zoom.x, 0.65f, 1.0f);
	m_zoom.y = std::clamp(m_zoom.y, 0.65f, 1.0f);
	m_viewMatrix[0] = m_zoom.x * m_zoomOffset.x;
	m_viewMatrix[5] = m_zoom.y * m_zoomOffset.y;
	m_viewMatrix[3] = m_position.x * m_zoom.x;
	m_viewMatrix[7] = m_position.y * m_zoom.y;
}
void Camera::addZoom(const vec2& _zoom) {
	m_zoom += _zoom;
	m_zoom.x = std::clamp(m_zoom.x, 0.65f, 1.0f);
	m_zoom.y = std::clamp(m_zoom.y, 0.65f, 1.0f);
	m_viewMatrix[0] = m_zoom.x * m_zoomOffset.x;
	m_viewMatrix[5] = m_zoom.y * m_zoomOffset.y;
	m_viewMatrix[3] = m_position.x * m_zoom.x;
	m_viewMatrix[7] = m_position.y * m_zoom.y;
}

void Camera::setZoomOffset(const vec2& _zoom) {
	m_zoomOffset = _zoom;
	m_zoomOffset.print();
	m_viewMatrix[0] = m_zoom.x * m_zoomOffset.x;
	m_viewMatrix[5] = m_zoom.y * m_zoomOffset.y;
}

void Camera::addZoomOffset(const vec2& _zoom) {
	m_zoomOffset += _zoom;
	m_viewMatrix[0] = m_zoom.x * m_zoomOffset.x;
	m_viewMatrix[5] = m_zoom.y * m_zoomOffset.y;
}

void Camera::setPosition(const vec2& _pos) {
	m_position = _pos;
	m_viewMatrix[3] = m_position.x * m_zoom.x;
	m_viewMatrix[7] = m_position.y * m_zoom.y;
	if (m_viewMatrix[3] > 0) { m_viewMatrix[3] = 0; }
}
void Camera::addPosition(const vec2& _pos) {
	m_position += _pos;
	m_viewMatrix[3] = m_position.x * m_zoom.x;
	m_viewMatrix[7] = m_position.y * m_zoom.y;
	if (m_viewMatrix[3] > 0) { m_viewMatrix[3] = 0; }
}

const vec2& Camera::getZoom() {
	return m_zoom;
}
const vec2& Camera::getPosition() {
	return m_position;
}

const vec2& Camera::getZoomOffset() {
	return m_zoomOffset;
}