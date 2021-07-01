#pragma once
#include <cstdint>
#include <vector>
#include "d3d9/d3d9.h"

namespace renderer {
	// arbitrary, can be more or less, dunno
	#define INITIAL_POINT_BUFFER_SIZE 32
	#define INITIAL_DRAW_CMD_BUFFER_SIZE 32

	struct drawcmd {
		uint16_t m_num_vertices;
		d3d9::index m_num_indices;
		d3d9::tex_id m_tex_id;
	};

	struct drawlist {
	public:
		s_buffer<d3d9::vertex> m_vertices;
		s_buffer<d3d9::index> m_indices;

		s_buffer<point> m_points;
		s_buffer<drawcmd> m_draw_cmds;
	public:
		void init();
		void clear();
	private:
		bool reserve(const size_t& num_vertices, const size_t& num_indices);
		bool reserve_points(const size_t& num_points);

		// the basic pathing functions, forcefully inlined because they will be called a LOT
		__forceinline void reset_path() { 
			m_points.size = 0; 
		}
		__forceinline void add_point(const point& point) { 
			m_points.buffer[m_points.size] = point;
			m_points.size++;
		}
		__forceinline void draw_path(const d3d9::color& color, const float& thickness) {
			add_polyline(m_points.buffer, m_points.size, color, thickness);
			reset_path();
		}
		__forceinline void add_quad(const point& point1, const point& point2, const point& point3, const point& point4, const d3d9::color color) {
			const size_t offset = m_vertices.size;

			d3d9::index* indices = m_indices.buffer + m_indices.size;
			indices[0] = offset + 0; indices[1] = offset + 1; indices[2] = offset + 2;
			indices[3] = offset + 0; indices[4] = offset + 2; indices[5] = offset + 3;
			m_indices.size += 6;

			d3d9::vertex* vertices = m_vertices.buffer + m_vertices.size;
			vertices[0].pos.x = point1.x; vertices[0].pos.y = point1.y; vertices[0].uv.x = 0.f; vertices[0].uv.y = 0.f; vertices[0].color = color;
			vertices[1].pos.x = point2.x; vertices[1].pos.y = point2.y; vertices[1].uv.x = 1.f; vertices[1].uv.y = 0.f; vertices[1].color = color;
			vertices[2].pos.x = point3.x; vertices[2].pos.y = point3.y; vertices[2].uv.x = 1.f; vertices[2].uv.y = 1.f; vertices[2].color = color;
			vertices[3].pos.x = point4.x; vertices[3].pos.y = point4.y; vertices[3].uv.x = 0.f; vertices[3].uv.y = 1.f; vertices[3].color = color;
			m_vertices.size += 4;
		}
	public:
		void add_polyline(const point* points, const size_t num_points, const d3d9::color color, const float thickness);
		void add_line(const point& point1, const point& point2, const d3d9::color color, const float thickness);
		void add_triangle(const point& point1, const point& point2, const point& point3, const d3d9::color color, const float thickness);
		void add_rect(const rect& rect, const d3d9::color color, const float thickness);
		void add_texture(const d3d9::tex_id id, const point& pos, const vec_2& size, d3d9::color color);

		void push_draw_cmd();
		void push_tex_draw_cmd(d3d9::tex_id id);
		void render(d3d9::object* d3d9_object);
	};

}