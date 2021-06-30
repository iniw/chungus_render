#include "renderer.h"

void renderer::init(LPDIRECT3DDEVICE9 d3d9_device, const s_vec2& display_size) {
	draw_list.init();
	d3d9::init(d3d9_device, display_size);

	initialized = true;
}

void renderer::start() {
	draw_list.m_num_vertices = 0;
	draw_list.m_num_indices = 0;
	draw_list.m_num_draw_cmds = 0;
}

void renderer::end() {
	draw_list.push_draw_cmd();
}

void renderer::render() {
	d3d9::render(&draw_list);
}

void renderer::rect(const s_rect& rect, const s_color& col) {
	draw_list.add_rect(rect, col);
}

void renderer::triangle(const s_point& point1, const s_point& point2, const s_point& point3, const s_color& col) {
	draw_list.add_triangle(point1, point2, point3, col);
}
