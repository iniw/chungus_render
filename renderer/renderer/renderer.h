#pragma once
#include "drawlist.h"
#include <unordered_map>

namespace renderer {
	inline drawlist draw_list;
	inline d3d9::object d3d_obj;
	inline std::unordered_map<int, d3d9::texture> textures;
	inline bool initialized = false;

	enum tex_identifiers {
		tex_biohazard = 0
	};
	

	void init(LPDIRECT3DDEVICE9 d3d9_device, const vec_2& display_size);
	
	void start();
	void end();
	void render();

	void on_size_change(const vec_2& display_size);

	void rectangle(const rect& rect, const color& col, const float& thickness = 1.f);
	void line(const point& point1, const point& point2, const color& col, const float& thickness = 1.f);
	void triangle(const point& point1, const point& point2, const point& point3, const color& col, const float& thickness = 1.f);

	void create_texture(int identifier, const uint8_t* data, const int width, const int height);
	void texture(int identifier, const point& pos, const color& col);
}