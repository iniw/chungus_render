#include "renderer.h"

void renderer::init(LPDIRECT3DDEVICE9 d3d9_device, s_vec2&& display_size) {
	draw_list.init();
	d3d9::init(d3d9_device, std::move(display_size));

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

void renderer::rect(s_rect&& rect, s_color&& col) {
	draw_list.add_rect(std::move(rect), std::move(col));
}

void renderer::triangle(s_point&& point1, s_point&& point2, s_point&& point3, s_color&& col) {
	draw_list.add_triangle(std::move(point1), std::move(point2), std::move(point3), std::move(col));
}
