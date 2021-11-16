#pragma once
#include <d3d9.h>
#include <cstdint>
namespace render {

	struct color {

		std::uint8_t r, g, b, a;

		constexpr color( std::uint8_t r = 255, std::uint8_t g = 255, std::uint8_t b = 255, std::uint8_t a = 255 ) : r{ r }, g{ g }, b{ b }, a{ a } { };

		constexpr inline D3DCOLOR to_d3d( ) const {

			return D3DCOLOR_RGBA( r, g, b, a );

		}

		constexpr inline color mod_alpha( std::uint8_t new_alpha ) {

			return color( r, g, b, new_alpha );

		}

		constexpr static inline color white( const std::uint8_t alpha = 255 ) {

			return color( 255, 255, 255, alpha );

		}

		constexpr static inline color black( const std::uint8_t alpha = 255 ) {

			return color( 0, 0, 0, alpha );

		}

		constexpr static inline color red( const std::uint8_t alpha = 255 ) {

			return color( 255, 0, 0, alpha );

		}

	};

}