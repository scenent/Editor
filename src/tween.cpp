#include "tween.h"
#include "camera.h"
#include "math_utils.h"

static double linear(double t) {
	return t;
}

static double quad(double t) {
	return t * t;
}

static double cubic(double t) {
	return t * t * t;
}

static double quart(double t) {
	return t * t * t * t;
}

static double quint(double t) {
	return t * t * t * t * t;
}

static double expo(double t) {
	return pow(2, 10 * (t - 1));
}

static double sine(double t) {
	return sin(t * 3.141592f / 2);
}

static float sineWhat(float ratio) {
	return -0.5f * (cos(3.141592f * ratio) - 1.0f);
}

static double easeIn(double t, std::function<double(double)> func) {
	return func(t);
}

static double easeOut(double t, std::function<double(double)> func) {
	return func(1 - t);
}

static double easeInOut(double t, std::function<double(double)> func) {
	if (t < 0.5) {
		return 0.5 * func(2 * t);
	}
	else {
		return 0.5 * func(2 * t - 1) + 0.5;
	}
}

static double easeOutIn(double t, std::function<double(double)> func) {
	if (t < 0.5) {
		return 0.5 * func(2 * t - 1) + 0.5;
	}
	else {
		return 0.5 * func(2 * t);
	}
}


Tween::Tween() {
	
}

Tween::~Tween() {

}

static double truncate(double num) {
	double error = std::abs(num - std::round(num));
	if (error < 0.01) {
		num = std::round(num * 10) / 10;
	}
	return num;
}

void Tween::update() {
	if (!m_isPlaying) {
		return;
	}
	auto iter = m_currentList.begin();
	for (size_t index = 0; index < m_currentList.size(); index++) {
		TweenInfo& info = m_currentList[index];
		if (info.Duration <= info.PlaybackPosition) {
			if (info.PropertyType == TweenablePropertyType::CameraPosition) {
				info.Object->setPosition(info.EndValue);
			}
			else if (info.PropertyType == TweenablePropertyType::CameraZoom) {
				info.Object->setZoom(info.EndValue);
			}
			else if (info.PropertyType == TweenablePropertyType::CameraZoomOffset) {
				info.Object->setZoomOffset(info.EndValue);
				info.Object->setZoomOffset(vec2(truncate(info.Object->getZoomOffset().x), truncate(info.Object->getZoomOffset().y)));
			}
			iter = m_currentList.erase(iter);
			continue;
		}

		const double& targetFPS = 60.0;

		double t = info.PlaybackPosition / info.Duration;
		if (t > 1.0) t = 1.0;

		std::function<double(double)> func; {
			switch (info.TransitionType) {
			case TweenTransitionType::TRANS_LINEAR:
				func = linear;
				break;
			case TweenTransitionType::TRANS_QUAD:
				func = quad;
				break;
			case TweenTransitionType::TRANS_CUBIC:
				func = cubic;
				break;
			case TweenTransitionType::TRANS_QUART:
				func = quart;
				break;
			case TweenTransitionType::TRANS_QUINT:
				func = quint;
				break;
			case TweenTransitionType::TRANS_EXPO:
				func = expo;
				break;
			case TweenTransitionType::TRANS_SINE:
				func = sine;
				break;
			default:
				func = linear;
				break;
			}
		}

		double ratio; {
			switch (info.EaseType) {
			case TweenEaseType::EASE_IN:
				ratio = easeIn(t, func);
				break;
			case TweenEaseType::EASE_OUT:
				ratio = easeOut(t, func);
				break;
			case TweenEaseType::EASE_IN_OUT:
				ratio = easeInOut(t, func);
				break;
			case TweenEaseType::EASE_OUT_IN:
				ratio = easeOutIn(t, func);
				break;
			default:
				ratio = t;
				break;
			}
		}

		switch (info.PropertyType) {
		case (TweenablePropertyType::CameraPosition): {
			vec2 start = info.StartValue;
			vec2 end = info.EndValue;
			vec2 value = lerp(start, end, (float)ratio);
			info.Object->setPosition(value);
			break;
		}
		case (TweenablePropertyType::CameraZoom): {
			vec2 start = info.StartValue;
			vec2 end = info.EndValue;
			vec2 value = lerp(start, end, (float)ratio);
			info.Object->setZoom(value);
			break;
		}
		case (TweenablePropertyType::CameraZoomOffset): {
			vec2 start = info.StartValue;
			vec2 end = info.EndValue;
			vec2 value = lerp(start, end, (float)ratio);
			info.Object->setZoomOffset(value);
			break;
		}
		}
		info.PlaybackPosition += 1.0 / targetFPS;
		iter++;
	}

}

void Tween::interpolateProperty(const TweenInfo& _info) {
	m_currentList.push_back(_info);
}
void Tween::start() {
	m_isPlaying = true;
}
void Tween::stop() {
	m_isPlaying = false;
}
void Tween::clear() {
	m_currentList.clear();
}

const bool& Tween::getIsPlaying() {
	return m_isPlaying;
}

bool Tween::getIsEmpty() {
	return m_currentList.empty();
}