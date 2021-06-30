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

		// TODO: make a template struct for this

		struct {
			s_point* buffer;
			size_t size;
			size_t capacity;
		} m_points;

		struct {
			d3d9_vertex* buffer;
			size_t size;
			size_t capacity;
		} m_vertices;

		struct {
			d3d9_index* buffer;
			size_t size;
			size_t capacity;
		} m_indices;

		struct {
			s_draw_cmd* buffer;
			size_t size;
			size_t capacity;
		} m_draw_cmds;

		void init();
		
		bool reserve(size_t num_vertices, size_t num_indices);
		bool reserve_points(size_t num_points);

		void push_draw_cmd();

		// the two basic pathing functions, forcefully inlined because they will be called a LOT
		__forceinline void path_clear() { m_points.size = 0; }
		__forceinline void path_to(const s_point& point) { 
			m_points.buffer[m_points.size] = point;
			m_points.size++;
		}

		void path_rect(const s_point& min, const s_point& max);
		void path_stroke(const s_color& col);

		void add_polyline(const s_point* points, const size_t num_points, const s_color& col, float thickness = 1.f);
		void add_rect(const s_rect& rect, const s_color& col);
		void add_triangle(const s_point& point1, const s_point& point2, const s_point& point3, const s_color& col);
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