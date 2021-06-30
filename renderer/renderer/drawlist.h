#pragma once
#include <cstdint>
#include <vector>
#include <d3d9.h>
#include "utils/datatypes.h"

// imgui
#define INITIAL_VERTEX_BUFFER_SIZE 5000
#define INITIAL_INDEX_BUFFER_SIZE 10000

// arbitrary, can be more or less, dunno
#define INITIAL_POINT_BUFFER_SIZE 32
#define INITIAL_DRAW_CMD_BUFFER_SIZE 32

namespace renderer {

	struct s_draw_cmd {
		uint16_t m_num_vertices;
		d3d9_index m_num_indices;
	};

	struct s_draw_list {
		bool m_aa; // anti-aliasing, not implemented yet

		s_point* m_points; // points buffer
		size_t m_num_points; // number of points in the buffer, this also doubles as the index
		size_t m_size_points; // size of the buffer in bytes

		d3d9::s_vertex* m_vertices; // vertices buffer
		size_t m_num_vertices; // number of vertices in the buffer
		size_t m_size_vertices; // size of the 'vertices' buffer in bytes

		d3d9_index* m_indices; // indices buffer
		size_t m_num_indices; // number of indices in the 'indices' buffer
		size_t m_size_indices; // size of the buffer in bytes

		s_draw_cmd* m_draw_cmds; // draw commands buffer
		size_t m_num_draw_cmds; // number of draw commands in the buffer
		size_t m_size_draw_cmds; // size of the buffer in bytes

		void init();
		
		bool reserve(size_t num_vertices, size_t num_indices);
		bool reserve_points(size_t num_points);

		void push_draw_cmd();

		// the two basic pathing functions, forcefully inlined because they will be called a LOT
		__forceinline void path_clear() { m_num_points = 0; }
		__forceinline void path_to(const s_point& point) { 
			m_points[m_num_points] = point;
			m_num_points++;
		}

		void path_stroke(const s_color& color);

		void add_polyline(const s_point* points, const size_t num_points, const s_color& color, float thickness = 1.f);
		void add_rect(const s_rect& rect, const s_color& color);
		void add_triangle(const s_point& point1, const s_point& point2, const s_point& point3, const s_color& color);
	};

	namespace d3d9 {
		struct {
			LPDIRECT3DDEVICE9 device = 0;
			LPDIRECT3DVERTEXBUFFER9 vb = 0; size_t vb_size; // vertex buffer
			LPDIRECT3DINDEXBUFFER9 ib = 0; size_t ib_size; // index buffer
			D3DMATRIX proj;
			D3DVIEWPORT9 viewport;
			LPDIRECT3DVERTEXDECLARATION9 vertex_declaration; // our custom vertex declaration
		} info;

		void init(LPDIRECT3DDEVICE9 d3d9_device, const s_vec2& display_size);

		void set_device(LPDIRECT3DDEVICE9 d3d9_device);
		void set_size(const s_vec2& size);
		void create_vertex_declaration();

		void set_render_states();
		void render(s_draw_list* draw_list);
	}

}