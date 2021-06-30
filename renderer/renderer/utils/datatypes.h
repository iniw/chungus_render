#pragma once
#include "vec_2.h"
#include "rect.h"
#include "color.h"

namespace renderer {
	using d3d_color = D3DCOLOR;	// stupid ass microsoft

	using d3d9_index = unsigned short;
	using tex_id = void*;

	using s_point = s_vec2;

	struct d3d9_vertex {
		s_point pos;
		d3d_color color;
		//s_vec2 uv; add back when i add textures
	};
}