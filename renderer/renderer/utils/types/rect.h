#pragma once

namespace renderer {
	struct rect {
	public:
		float x, y, w, h;
	public:
		constexpr rect(float x = 0.f, float y = 0.f, float w = 0.f, float h = 0.f) : x(x), y(y), w(w), h(h) {}
	};
}