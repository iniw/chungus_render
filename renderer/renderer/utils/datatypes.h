#pragma once
#include "vec_2.h"
#include "rect.h"
#include "color.h"
#include "buffer.h"

namespace renderer {
	using s_point = s_vec2;
	namespace d3d9 {
		using tex_id = void*;
		using color = D3DCOLOR;
		using index = unsigned short;

		struct vertex {
			s_point pos;
			color color;
			s_vec2 uv;
		};
	}
}