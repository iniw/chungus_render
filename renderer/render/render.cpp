#include "render.h"

#include <d3dx9core.h>

bool render::render::init(IDirect3DDevice9*d) {

	if ( !m_drawlist.init( ) )
		return false;

	if ( !m_d3d9_obj.init( d ) )
		return false;

	m_font = font::from_memory( karla, 20.f );

	geometry::base::m_list = &m_drawlist;

	return true;

}

void render::render::unload( ) {

	m_drawlist.invalidate( );
	m_d3d9_obj.invalidate( );

}

void render::render::start( ) {

	m_drawlist.start( );

}

void render::render::end( ) {

	m_drawlist.push_cmd( );

	m_drawlist.draw( &m_d3d9_obj );

}

void render::render::pre_reset( ) {

	m_drawlist.invalidate( );
	m_d3d9_obj.invalidate( );

}

void render::render::post_reset( ) {

	D3DVIEWPORT9 viewport;
	m_d3d9_obj.m_device->GetViewport( &viewport );

	m_d3d9_obj.set_size( { viewport.Width, viewport.Height } );

}

const math::v2i render::render::get_screen_size( ) {

	return math::v2i( m_d3d9_obj.m_viewport.Width, m_d3d9_obj.m_viewport.Height );

}

render::point render::render::handle_flags( const point& pos, const size& size, int flags ) {

	if ( !flags )
		return pos;

	point ret = pos;

	if ( flags & LEFT )
		ret[ X ] -= size[ X ];
	else if ( flags & RIGHT )
		ret[ X ] += size[ X ];
	else if ( flags & X_CENTER )
		ret[ X ] -= size[ X ] / 2;

	if ( flags & TOP )
		ret[ Y ] -= size[ Y ];
	else if ( flags & BOTTOM )
		ret[ Y ] += size[ Y ];
	else if ( flags & Y_CENTER )
		ret[ Y ] -= size[ Y ] / 2;

	return ret;

}

render::geometry::rect render::render::rectangle( const point& pos, const size& size, const color& col, int flags ) {

	const auto new_pos = handle_flags( pos, size, flags );

	geometry::rect obj = { new_pos, SIZE_TO_POS( new_pos, size ), col };

	obj.draw( );

	return obj;

}

render::geometry::rect_filled render::render::rectangle_filled( const point& pos, const size& size, const color& col, int flags ) {

	const auto new_pos = handle_flags( pos, size, flags );

	geometry::rect_filled obj = { new_pos, SIZE_TO_POS( new_pos, size ), col };

	obj.draw( );

	return obj;

}

render::geometry::line render::render::line( const point& point1, const point& point2, const color& col, int flags ) {

	geometry::line obj = { point1, point2, col };

	obj.draw( );

	return obj;

}

void render::render::text( std::string_view text, point pos, const color& col ) {

	m_drawlist.add_text( &m_font, text, pos, col.to_d3d( ) );

}


render::d3d9::tex_id render::render::create_texture( const std::uint8_t* data, const int width, const int height ) {

	return m_d3d9_obj.create_texture( data, width, height );

}