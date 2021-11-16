#pragma once

#include "color.h"
#include "buffer.h"
#include "../variadic_vector.h"

namespace render {

	using point = math::v2;
	using rect = math::v4;
	using size = math::v2i;

	namespace d3d9 {

		using tex_id = IDirect3DTexture9*;
		using color = D3DCOLOR;
		using index = unsigned short;

		struct vertex {

			point m_pos;
			color m_color;
			math::v2 m_uv;

		};

	}

}