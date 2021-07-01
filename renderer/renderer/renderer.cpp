#include "renderer.h"

void renderer::init(LPDIRECT3DDEVICE9 d3d9_device, const vec_2& display_size) {
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

void renderer::on_size_change(const vec_2& display_size) {
	d3d_obj.set_size(display_size);
}

void renderer::rectangle(const rect& rect, const color& col, const float& thickness) {
	draw_list.add_rect(rect, col.to_d3d(), thickness);
}

void renderer::line(const point& point1, const point& point2, const color& col, const float& thickness) {
	draw_list.add_line(point1, point2, col.to_d3d(), thickness);
}

void renderer::triangle(const point& point1, const point& point2, const point& point3, const color& col, const float& thickness) {
	draw_list.add_triangle(point1, point2, point3, col.to_d3d(), thickness);
}

void renderer::create_texture(int identifier, const uint8_t* data, const int width, const int height) {
	auto id = d3d_obj.create_texture(data, width, height);
	if (!id)
		return;

	textures[identifier] = {data, id, {static_cast<float>(width), static_cast<float>(height)}};
}

void renderer::texture(int identifier, const point& pos, const color& col) {
	if (!textures.contains(identifier))
		return;

	draw_list.add_texture(textures[identifier].id, pos, textures[identifier].size, col.to_d3d());
}
