#pragma once

namespace render::geometry {

	struct line : public base {
	private:

		point m_point1;
		point m_point2;

	public:

		constexpr line( ) = default;

		line( const point& point1, const point& point2, const color& col ) : m_point1{ point1 }, m_point2{ point2 } {

			m_color = col;

		}

		inline void draw( ) override {

			m_list->add_line( m_point1, m_point2 + 0.5f, m_color.to_d3d( ) );

		}

	};

}