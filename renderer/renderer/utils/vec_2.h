#pragma once
#include <limits>

namespace renderer {
	struct s_vec2 {
	public:
		float x, y;
	public:
		constexpr s_vec2(float x = 0.f, float y = 0.f) : x(x), y(y) {}
	public:
		inline s_vec2 operator-(const s_vec2& v) const {
			s_vec2 r = *this;
			r.x -= v.x;
			r.y -= v.y;
			return r;
		}

		inline s_vec2 operator*=(const float& v) {
			this->x *= v;
			this->y *= v;
			return *this;
		}

		inline float length() const {
			return hypotf(x, y);
		}

		inline s_vec2& normalize() {
			const float len = length();
			const float radius = 1.f / (len + FLT_EPSILON);

			x *= radius;
			y *= radius;

			return *this;
		}
	};
}