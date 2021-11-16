#pragma once

#include <cstdint>
#include <vector>

#include "../d3d9/d3d9.h"
#include "../font/font.h"

namespace render {

	enum {

		INITIAL_POINT_BUFFER_SIZE = 32,
		INITIAL_DRAW_CMD_BUFFER_SIZE = 32

	};

	struct draw_cmd {

		std::uint16_t m_num_vertices;
		d3d9::index m_num_indices;
		d3d9::tex_id m_tex_id;

	};

	struct drawlist {

		buffer< d3d9::vertex > m_vertices;
		buffer< d3d9::index > m_indices;

		buffer< point > m_points;
		buffer< draw_cmd > m_draw_cmds;

	private:

		// the basic pathing functions, forcefully inlined because they will be called a lot
		__forceinline void reset_path( ) {

			m_points.m_size = 0;

		}

		__forceinline void add_point( const point& point ) {

			m_points.m_buffer[ m_points.m_size ] = point;
			m_points.m_size++;

		}

		__forceinline void draw_path( const d3d9::color color ) {

			add_polyline( &m_points, color );
			reset_path( );

		}

		__forceinline void draw_path_filled( const d3d9::color color ) {

			add_polyline_filled( &m_points, color );
			reset_path( );

		}

		inline void add_quad( const point& point1, const point& point2, const point& point3, const point& point4, const d3d9::color color ) {

			if ( !m_vertices.reserve( 4 ) || !m_indices.reserve( 6 ) )
				return;

			const std::size_t offset = m_vertices.m_size;

			d3d9::index* indices = m_indices.end( );
			indices[ 0 ] = offset + 0; indices[ 1 ] = offset + 1; indices[ 2 ] = offset + 2;
			indices[ 3 ] = offset + 0; indices[ 4 ] = offset + 2; indices[ 5 ] = offset + 3;
			m_indices.m_size += 6;

			d3d9::vertex* vertices = m_vertices.end( );
			vertices[ 0 ].m_pos[ X ] = point1[ X ]; vertices[ 0 ].m_pos[ Y ] = point1[ Y ]; vertices[ 0 ].m_uv[ X ] = 0.f; vertices[ 0 ].m_uv[ Y ] = 0.f; vertices[ 0 ].m_color = color;
			vertices[ 1 ].m_pos[ X ] = point2[ X ]; vertices[ 1 ].m_pos[ Y ] = point2[ Y ]; vertices[ 1 ].m_uv[ X ] = 1.f; vertices[ 1 ].m_uv[ Y ] = 0.f; vertices[ 1 ].m_color = color;
			vertices[ 2 ].m_pos[ X ] = point3[ X ]; vertices[ 2 ].m_pos[ Y ] = point3[ Y ]; vertices[ 2 ].m_uv[ X ] = 1.f; vertices[ 2 ].m_uv[ Y ] = 1.f; vertices[ 2 ].m_color = color;
			vertices[ 3 ].m_pos[ X ] = point4[ X ]; vertices[ 3 ].m_pos[ Y ] = point4[ Y ]; vertices[ 3 ].m_uv[ X ] = 0.f; vertices[ 3 ].m_uv[ Y ] = 1.f; vertices[ 3 ].m_color = color;
			m_vertices.m_size += 4;

		}

	public:

		bool init( );

		void start( );

		void invalidate( );

	public:

		void add_polyline( const buffer< point >* points, const d3d9::color color );

		void add_polyline_filled( const buffer< point >* points, const d3d9::color color );

		void add_line( const point& point1, const point& point2, const d3d9::color color );

		void add_rect( const point& point1, const point& point2, const d3d9::color color );

		void add_rect_filled( const point& point1, const point& point2, const d3d9::color color );

		void add_texture( const d3d9::tex_id id, point pos, const math::v2i& size, d3d9::color color );

		void add_text( font* font, std::string_view text, point& pos, d3d9::color color );

		void push_cmd( );

		void push_tex_cmd( d3d9::tex_id id );

		void draw( d3d9::object* d3d9_object );

	};

}