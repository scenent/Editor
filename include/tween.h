#ifndef TWEEN_H
#define TWEEN_H

#include <any>
#include <functional>
#include "math_utils.h"

enum class TweenTransitionType {
	TRANS_LINEAR,
	TRANS_SINE,
	TRANS_QUAD,
	TRANS_CUBIC,
	TRANS_QUART, 
	TRANS_QUINT,
	TRANS_EXPO,
};

enum class TweenEaseType {
	EASE_IN,
	EASE_IN_OUT,
	EASE_OUT,
	EASE_OUT_IN
};


enum class TweenablePropertyType {
	Unknown, CameraPosition, CameraZoom, CameraZoomOffset
};

struct TweenInfo {
	class Camera* Object = nullptr;
	double Duration = 1.0;
	vec2 StartValue = vec2();
	vec2 EndValue = vec2();
	TweenablePropertyType PropertyType = TweenablePropertyType::Unknown;
	TweenTransitionType TransitionType = TweenTransitionType::TRANS_LINEAR;
	TweenEaseType EaseType = TweenEaseType::EASE_IN;
	double PlaybackPosition = 0.0;
};

class Tween final{
private:
	std::vector<TweenInfo> m_currentList{};
	bool m_isPlaying = false;
public:
	Tween();
	~Tween();
	void update();
public:
	void interpolateProperty(const TweenInfo& _info);
	void start();
	void stop();
	void clear();
	const bool& getIsPlaying();
	bool getIsEmpty();
};






#endif