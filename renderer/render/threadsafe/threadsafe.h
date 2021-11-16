#pragma once

#include <deque>
#include <shared_mutex>
#include <memory>
#include "../types/geometry/geometry.h"
#include "../types/types.h"

namespace render {

	struct threadsafe {
	private:

		std::deque< std::shared_ptr< geometry::safe_base > > m_list;
		std::deque< std::shared_ptr< geometry::safe_base > > m_safe_list;

		std::shared_mutex m_mutex;

	public:

		void draw( );

		void swap( );

		void clear( );

		std::shared_ptr< geometry::safe_rect > rectangle( const point& point1, const size& size, const color& col, int flags = 0 );

		std::shared_ptr< geometry::safe_rect_filled > rectangle_filled( const point& point1, const size& size, const color& col, int flags = 0 );

	private:

		template< geometry::obj t, class... args_t >
		inline std::shared_ptr< t > add( args_t&&... args ) {

			return std::dynamic_pointer_cast< t, geometry::safe_base >( m_list.emplace_back( std::make_shared< t >( args... ) ) );

		}

	};

}