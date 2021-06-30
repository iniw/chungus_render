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

		s_buffer<s_point> m_points;
		s_buffer<drawcmd> m_draw_cmds;
	public:
		void init();
		void clear();
		
		bool reserve(const size_t& num_vertices, const size_t& num_indices);
		bool reserve_points(const size_t& num_points);

		// the basic pathing functions, forcefully inlined because they will be called a LOT
		__forceinline void reset_path() { 
			m_points.size = 0; 
		}
		__forceinline void add_point(const s_point& point) { 
			m_points.buffer[m_points.size] = point;
			m_points.size++;
		}
		__forceinline void draw_path(const d3d9::color& color, const float& thickness) {
			add_polyline(m_points.buffer, m_points.size, color, thickness);
			reset_path();
		}
	public:
		void add_polyline(const s_point* points, const size_t num_points, const d3d9::color color, const float thickness);
		void add_line(const s_point& point1, const s_point& point2, const d3d9::color color, const float thickness);
		void add_triangle(const s_point& point1, const s_point& point2, const s_point& point3, const d3d9::color color, const float thickness);
		void add_rect(const s_rect& rect, const d3d9::color color, const float thickness);
	public:
		void push_draw_cmd();
		void render(d3d9::object* d3d9_object);
	};

}