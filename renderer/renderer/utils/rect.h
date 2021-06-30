#pragma once

namespace renderer {
	struct s_rect {
	public:
		float x, y, w, h;
	public:
		constexpr s_rect(float x = 0.f, float y = 0.f, float w = 0.f, float h = 0.f) : x(x), y(y), w(w), h(h) {}
	};
}