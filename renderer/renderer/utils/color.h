#pragma once
#include <cstdint>

namespace renderer {
	struct s_color {
		uint8_t r, g, b, a;

		constexpr s_color(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255) : r(r), g(g), b(b), a(a) {};

		__forceinline D3DCOLOR to_d3d() const { return D3DCOLOR_RGBA(r, g, b, a); }
	};
}