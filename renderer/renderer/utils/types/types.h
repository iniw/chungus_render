#pragma once
#include "vec_2.h"
#include "rect.h"
#include "color.h"
#include "buffer.h"

namespace renderer {
	using point = vec_2;

	namespace d3d9 {
		using tex_id = void*;
		using color = D3DCOLOR;
		using index = unsigned short;

		struct vertex {
			point pos;
			color color;
			vec_2 uv;
		};

		struct texture {
		public:
			const uint8_t* data;
			tex_id id;
			vec_2 size;
		public:
			texture() : data(0), id(0), size(0) {}

			texture(const uint8_t* data, tex_id id, const vec_2& size) : data(data), id(id), size(size) {}
		};
	}
}