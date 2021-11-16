#pragma once
#include <vector>
#include <memory>
#include "../color.h"
#include "base.h"
#include "../../variadic_vector.h"
namespace render::geometry {

	struct rect : public base {
	protected:

		point m_point1;
		point m_point2;

	public:

		constexpr rect( ) = default;

		rect( const point& point1, const point& point2, const color& color ) : m_point1{ point1 }, m_point2{ point2 } {

			m_color = color;

		}

		inline void draw( ) override {

			m_list->add_rect( m_point1, m_point2 + 0.5f, m_color.to_d3d( ) );

		};

		inline virtual void outline( const color& color = color::black( ), const bool innerline = false ) {

			m_list->add_rect( m_point1 - 1.f, m_point2 + 1.5f, color.to_d3d( ) );

			if ( innerline )
				m_list->add_rect( m_point1 + 1.f, m_point2 - 0.5f, color.to_d3d( ) );

		}

	};

	struct rect_filled : public rect {

		constexpr rect_filled( ) = default;

		rect_filled( const point& point1, const point& point2, const color& col ) {

			m_point1 = point1;
			m_point2 = point2;
			m_color = col;

		}

		inline void draw( ) override {

			m_list->add_rect_filled( m_point1, m_point2 + 0.5f, m_color.to_d3d( ) );

		};

	};

	struct safe_rect : public rect, public safe_base {

		constexpr safe_rect( ) = default;

		safe_rect( const point& point1, const point& point2, const color& col ) {

			m_point1 = point1;
			m_point2 = point2;
			m_color = col;

			m_drawlist.emplace_back( std::make_unique< rect >( m_point1, m_point2, m_color ) );

		}

		inline void draw( ) override {

			for ( auto& draw_cmd : m_drawlist )
				draw_cmd->draw( );

		};

		inline void outline( const color& color = color::black( ), const bool innerline = false ) override {

			m_drawlist.emplace_back( std::make_unique< rect >( m_point1 - 1.f, m_point2 + 1.f, color ) );

			if ( innerline )
				m_drawlist.emplace_back( std::make_unique< rect >( m_point1 + 1.f, m_point2 - 1.f, color ) );

		}

	};

	struct safe_rect_filled : public safe_rect {

		constexpr safe_rect_filled( ) = default;

		safe_rect_filled( const point& point1, const point& point2, const color& col ) {

			m_point1 = point1;
			m_point2 = point2;
			m_color = col;

			m_drawlist.emplace_back( std::make_unique< rect_filled >( m_point1, m_point2, m_color ) );

		}

	};

}