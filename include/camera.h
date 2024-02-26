#ifndef CAMERA_H
#define CAMERA_H

#include "math_utils.h"
#include <vector>

class Camera final{
private:
	vec2 m_position = vec2();
	vec2 m_zoom = vec2(1.0f);
	vec2 m_zoomOffset = vec2(1.0f);
	std::vector<float> m_viewMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	std::vector<float> m_projectionMatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};
public:
	Camera(int _screenWidth, int _screenHeight);
	~Camera();
	void setWindowSize(int _width, int _height);
	void setZoom(const vec2& _zoom);
	void addZoom(const vec2& _zoom);
	const vec2& getZoom();
	void setZoomOffset(const vec2& _zoom);
	void addZoomOffset(const vec2& _zoom);
	const vec2& getZoomOffset();
	void setPosition(const vec2& _pos);
	void addPosition(const vec2& _pos);
	const vec2& getPosition();
	const std::vector<float>& getViewMatrix() const ;
	const std::vector<float>& getProjectionMatrix()const;
};








#endif