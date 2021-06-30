#include "renderer.h"

void renderer::init(LPDIRECT3DDEVICE9 d3d9_device, const s_vec2& display_size) {
	draw_list.init();
	d3d_obj.init(d3d9_device, display_size);

	initialized = true;
}

void renderer::start() {
	draw_list.clear();
}

void renderer::end() {
	draw_list.push_draw_cmd();
}

void renderer::render() {
	draw_list.render(&d3d_obj);
}

void renderer::on_size_change(const s_vec2& display_size) {
	d3d_obj.set_size(display_size);
}

void renderer::rect(const s_rect& rect, const s_color& col, const float& thickness) {
	draw_list.add_rect(rect, col.to_d3d(), thickness);
}

void renderer::line(const s_point& point1, const s_point& point2, const s_color& col, const float& thickness) {
	draw_list.add_line(point1, point2, col.to_d3d(), thickness);
}

void renderer::triangle(const s_point& point1, const s_point& point2, const s_point& point3, const s_color& col, const float& thickness) {
	draw_list.add_triangle(point1, point2, point3, col.to_d3d(), thickness);
}
