#pragma once
#include "drawlist.h"

namespace renderer {

	inline s_draw_list draw_list;
	inline bool initialized = false;

	void init(LPDIRECT3DDEVICE9 d3d9_device, const s_vec2& display_size);
	
	void start();
	void end();
	void render();

	void rect(const s_rect& rect, const s_color& col);

	void triangle(const s_point& point1, const s_point& point2, const s_point& point3, const s_color& col);
}