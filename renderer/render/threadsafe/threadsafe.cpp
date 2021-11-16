
#include "threadsafe.h"
#include "../render.h"

void render::threadsafe::draw( ) {

	std::unique_lock< std::shared_mutex > lock( m_mutex );

	if ( m_safe_list.empty( ) )
		return;

	for ( auto& obj : m_safe_list )
		obj->draw( );

}

void render::threadsafe::swap( ) {

	std::unique_lock< std::shared_mutex > lock( m_mutex );

	m_list.swap( m_safe_list );

}

void render::threadsafe::clear( ) {

	if ( m_list.size( ) )
		m_list.clear( );

}

std::shared_ptr< render::geometry::safe_rect > render::threadsafe::rectangle( const point& point1, const size& size, const color& col, int flags ) {

	const auto new_pos = g_render.handle_flags( point1, size, flags );

	return add< geometry::safe_rect >( new_pos, point( SIZE_TO_POS( new_pos, size ) ), col );

}

std::shared_ptr< render::geometry::safe_rect_filled > render::threadsafe::rectangle_filled( const point& point1, const size& size, const color& col, int flags ) {

	const auto new_pos = g_render.handle_flags( point1, size, flags );

	return add< geometry::safe_rect_filled >( new_pos, point( SIZE_TO_POS( new_pos, size ) ), col );

}