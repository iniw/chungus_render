#pragma once
#include <cstdint>

namespace renderer {
	struct s_color {
	public:
		uint8_t r, g, b, a;
	public:
		constexpr s_color(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255, uint8_t a = 255) : r(r), g(g), b(b), a(a) {};
	public:
		__forceinline D3DCOLOR to_d3d() const { 
			return D3DCOLOR_RGBA(r, g, b, a); 
		}
		
		inline s_color modify_alpha(uint8_t new_alpha) {
			return {r, g, b, new_alpha};
		}

		static inline s_color white() {
			return {255, 255, 255, 255};
		}
	};
}