#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include <iostream>
#include <cassert>

#undef min
#undef max

template<typename GenType>
struct _vec2 {
	GenType x, y;
	_vec2() : x(0.0f), y(0.0f) {

	}
	_vec2(const GenType& _x, const GenType& _y) : x(_x), y(_y) {

	}
	explicit _vec2(const GenType& _f) : x(_f), y(_f) {

	}
	_vec2(const _vec2<GenType>& other) {
		this->x = other.x;
		this->y = other.y;
	}
	_vec2 operator+(const _vec2& v) {
		return _vec2(this->x + v.x, this->y + v.y);
	}
	_vec2& operator+=(const _vec2& v) {
		this->x += v.x;
		this->y += v.y;
		return *this;
	}
	_vec2 operator-(const _vec2& v) {
		return _vec2(this->x - v.x, this->y - v.y);
	}
	_vec2& operator-=(const _vec2& v) {
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}
	template<typename Scala>
	_vec2 operator*(const Scala& v) {
		return _vec2(this->x * v, this->y * v);
	}
	template<typename Scala>
	_vec2& operator*=(const Scala& v) {
		this->x *= v;
		this->y *= v;
		return *this;
	}
	template<typename Scala>
	_vec2 operator/(const Scala& v) {
		return _vec2(this->x / v, this->y / v);
	}
	template<typename Scala>
	_vec2& operator/=(const Scala& v) {
		this->x /= v;
		this->y /= v;
		return *this;
	}
	void print() const {
		std::cout << "vec2(" << x << ", " << y << ")\n";
	}
};

template<typename GenType>
struct _vec4 {
	union {
		struct { GenType x, y, z, w; };
		struct { GenType r, g, b, a; };
	};
	_vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {

	}
	_vec4(const GenType& _x, const GenType& _y, const GenType& _z, const GenType& _w) : x(_x), y(_y), z(_z), w(_w) {

	}
	explicit _vec4(const GenType& _f) : x(_f), y(_f), z(_f), w(_f) {

	}
};

using vec2 = _vec2<float>;
using vec2i = _vec2<int>;

using vec4 = _vec4<float>;
using vec4i = _vec4<int>;


inline float lerp(const float& min, const float& max, const float& value) {
	return min + (max - min) * value;
}

inline vec2 lerp(vec2& min, vec2& max, const float& value) {
	return min + (max - min) * value;
}

inline float clamp(const float& value, const float& min, const float& max) {
	assert(min <= max);
	if (min == max) {
		return min;
	}
	float result = value;
	if (result < min) {
		result = min;
	}
	if (result > max) {
		result = max;
	}
	return result;
}

#endif