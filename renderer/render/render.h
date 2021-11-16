#pragma once

#include <unordered_map>

#define SIZE_TO_POS( pos, size ) { pos[ X ] + size[ X ] - 1, pos[ Y ] + size[ Y ] - 1 }

#include "drawlist/drawlist.h"
#include "threadsafe/threadsafe.h"
#include "types/geometry/geometry.h"

namespace render {

	enum font_idx {

		DEFAULT,
		MAX

	};

	enum flags {

		LEFT = ( 1 << 0 ),
		RIGHT = ( 1 << 1 ),
		X_CENTER = ( 1 << 2 ),
		TOP = ( 1 << 3 ),
		BOTTOM = ( 1 << 4 ),
		Y_CENTER = ( 1 << 5 ),
		CENTERED = ( Y_CENTER | X_CENTER )

	};

	struct render {
	//private:

		drawlist m_drawlist;

		d3d9::object m_d3d9_obj;

		std::array< font, font_idx::MAX + 1 > m_fonts;

		font m_font;

	public:

		threadsafe m_safe;

		bool init(IDirect3DDevice9*);

		void unload( );

		void start( );

		void end( );

		void pre_reset( );

		void post_reset( );

		const math::v2i get_screen_size( );

		point handle_flags( const point& pos, const size& size, int flags );

		geometry::rect rectangle( const point& point, const size& size, const color& col, int flags = 0 );

		geometry::rect_filled rectangle_filled( const point& point, const size& size, const color& col, int flags = 0 );

		geometry::line line(const point& point1, const point& point2, const color& col, int flags = 0 );

		void text( std::string_view text, point pos, const color& col );

		d3d9::tex_id create_texture( const std::uint8_t* data, const int width, const int height );


	};

}

inline render::render g_render;