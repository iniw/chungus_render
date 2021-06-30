#pragma once
#include "drawlist.h"

namespace renderer {

	inline s_draw_list draw_list;
	inline bool initialized = false;

	void init(LPDIRECT3DDEVICE9 d3d9_device, s_vec2&& display_size);
	
	void start();
	void end();
	void render();

	void rect(s_rect&& rect, s_color&& col);

	void triangle(s_point&& point1, s_point&& point2, s_point&& point3, s_color&& col);
}