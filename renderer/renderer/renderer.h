#pragma once
#include "drawlist.h"

namespace renderer {
	inline drawlist draw_list;
	inline d3d9::object d3d_obj;
	inline bool initialized = false;

	void init(LPDIRECT3DDEVICE9 d3d9_device, const s_vec2& display_size);
	
	void start();
	void end();
	void render();

	void on_size_change(const s_vec2& display_size);

	void rect(const s_rect& rect, const s_color& col, const float& thickness = 1.f);
	void line(const s_point& point1, const s_point& point2, const s_color& col, const float& thickness = 1.f);
	void triangle(const s_point& point1, const s_point& point2, const s_point& point3, const s_color& col, const float& thickness = 1.f);
}