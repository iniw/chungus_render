#pragma once

#include <array>
#include <algorithm>
#include <cmath>
#include <format>
#include <ostream>

// just to make accessing vector elements less annoying
enum {

    X = 0,
    Y,
    Z,

    X2 = 2,
    Y2 = 3,
    WIDTH = 2,
    HEIGHT = 3

};

namespace math {

    template < typename t >
    concept number = std::is_integral< t >::value || std::is_floating_point< t >::value;

    // credits to: https://www.unknowncheats.me/forum/c-and-c/157292-variadic-vector-class.html

    template< number t, std::size_t size >
    struct vector {
    private:

        using arr_t = std::array< t, size >;
        using vec_t = vector< t, size >;

        arr_t m_data;

    public:

        constexpr vector( ) {

            static_assert( size >= 2, "vector requires a min. dimension of 2" );

            fill( static_cast< t >( 0 ) );

        }

        constexpr explicit vector( const arr_t& arr ) : m_data{ arr } {

            static_assert( size >= 2, "vector requires a min. dimension of 2" );

        }

        constexpr vector( const vec_t& other ) : m_data{ other.data( ) } {

            static_assert( size >= 2, "vector requires a min. dimension of 2" );

        }

        // this is extremely unsafe, oh well
        constexpr vector( const t* ptr ) {

            static_assert( size >= 2, "vector requires a min. dimension of 2" );

            std::copy( ptr, ptr + size, m_data.begin( ) );

        }

        // thanks para
        template < number... args_t >
        constexpr vector( args_t... args ) : m_data{ static_cast< t >( std::forward< args_t >( args ) )... } {

            static_assert( size >= 2, "vector requires a min. dimension of 2" );

        }

        constexpr auto operator <=> ( const vec_t& ) const = default;

        constexpr inline t& operator [] ( const std::size_t idx ) {

            return m_data[ idx ];

        }

        constexpr inline const t& operator [] ( const std::size_t idx ) const {

            return m_data[ idx ];

        }

        constexpr inline t& at( const std::size_t idx ) {

            return m_data.at( idx );

        }

        constexpr inline const t& at( const std::size_t idx ) const {

            return m_data.at( idx );

        }

        constexpr inline vector& operator = ( const vec_t& other ) {

            for ( std::size_t i = 0; i < size; i++ )
                at( i ) = other[ i ];

            return *this;

        }

        constexpr inline vector& operator = ( const arr_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) = other.at( i );

             return *this;

        }

        constexpr inline vector& operator = ( const t other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) = other;

             return *this;

        }

        constexpr inline vector operator + ( const vec_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) + other[ i ];

             return buf;

        }

        constexpr inline vector operator + ( const arr_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) + other.at( i );

             return buf;

        }

        constexpr inline vector operator + ( const t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) + other;

             return buf;

        }

        constexpr inline vector& operator += ( const vec_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) += other[ i ];

             return *this;

        }

        constexpr inline vector& operator += ( const arr_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) += other.at( i );

             return *this;

        }

        constexpr inline vector& operator += ( const t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) += other;

             return *this;

        }

        constexpr inline vector operator - ( const vec_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) - other[ i ];

             return buf;

        }

        constexpr inline vector operator - ( const arr_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) - other.at( i );

             return buf;

        }

        constexpr inline vector operator - ( const t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) - other;

             return buf;

        }

        constexpr inline vector& operator -= ( const vec_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) -= other[ i ];

             return *this;

        }

        constexpr inline vector& operator -= ( const arr_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) -= other.at( i );

             return *this;

        }

        constexpr inline vector& operator -= ( const t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) -= other;

             return *this;

        }

        constexpr inline vector operator * ( const vec_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) * other[ i ];

             return buf;

        }

        constexpr inline vector operator * ( const arr_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) * other.at( i );

             return buf;

        }

        constexpr inline vector operator * ( const t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) * other;

             return buf;

        }

        constexpr inline vector& operator *= ( const vec_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) *= other[ i ];;

             return *this;

        }

        constexpr inline vector& operator *= ( const arr_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) *= other.at( i );

             return *this;

        }

        constexpr inline vector& operator *= ( const t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) *= other;

             return *this;

        }

        constexpr inline vector operator / ( const vec_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) / other[ i ];

             return buf;

        }

        constexpr inline vector operator / ( const arr_t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) / other.at( i );

             return buf;

        }

        constexpr inline vector operator / ( const t& other ) const {

             vec_t buf;

             for ( std::size_t i = 0; i < size; i++ )
                 buf[ i ] = at( i ) / other;

             return buf;

        }

        constexpr inline vector& operator /= ( const vec_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) /= other[ i ];

             return *this;

        }

        constexpr inline vector& operator /= ( const arr_t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) /= other.at( i );

             return *this;

        }

        constexpr inline vector& operator /= ( const t& other ) {

             for ( std::size_t i = 0; i < size; i++ )
                 at( i ) /= other;

             return *this;

        }

        constexpr inline const arr_t& data( ) const {

             return m_data;

        }

        constexpr inline bool empty( ) const {

             return std::all_of( m_data.begin( ), m_data.end( ), [ ]( t value ) {

                 return value == static_cast< t >( 0 );

             } );

        }

        constexpr inline t dist_to( const vec_t& other ) const {

             return ( other - ( *this ) ).length( );

        }

        constexpr inline float length_2d( ) const {

            return std::hypotf( at( X ), at( Y ) );

        }

        constexpr inline t length( ) const {

             return std::sqrt( length_sqr( ) );

        }

        constexpr inline t length_sqr( ) const {

             t buf = static_cast< t >( 0 );

             for ( const auto& i : m_data )
                 buf += ( i * i );

             return buf;

        }


        constexpr inline void fill( const t value ) {

             m_data.fill( value );

        }

        constexpr inline void normalize( ) {

             ( *this ) /= length( );

        }

        constexpr inline vec_t cross_product( const vec_t& other, const bool normalize_cross_product = false ) const {

             static_assert( size == 3, "cross product requires a 3 dimensional vector" );

             vec_t vec = {

                 at( 1 ) * other[ 2 ] - at( 2 ) * other[ 1 ],
                 at( 2 ) * other[ 0 ] - at( 0 ) * other[ 2 ],
                 at( 0 ) * other[ 1 ] - at( 1 ) * other[ 0 ]

             };

             if ( normalize_cross_product )
                 vec.normalize( );

             return vec;

        }

        constexpr inline vec_t inversed( ) const {

             vec_t vec = *this;
             const auto one = static_cast< t >( 1 );

             for ( auto& i : vec )
                 i = one / i;

             return vec;

        }

        constexpr inline vec_t normalized( ) const {

             vec_t vec = *this;
             vec.normalize( );

             return vec;

        }

        constexpr inline t dot_product( const vec_t& other ) const {

            auto dot = static_cast< t >( 0 );

            for ( std::size_t i = 0; i < size; i++ )
                dot += at( i ) * other[ i ];

            return dot;

        }

        constexpr inline vec_t floor( ) {

            for ( std::size_t i = 0; i < size; i++ )
                at( i ) = std::floor( at( i ) );

            return *this;

        }

        constexpr inline vec_t ceil( ) {

            for ( std::size_t i = 0; i < size; i++ )
                at( i ) = std::ceil( at( i ) );

            return *this;

        }

        constexpr inline t width( ) {

            static_assert( size == 4, "width requires a 4 dimensional vector" );

            return at( X2 ) - at( X );

        }

        constexpr inline t height( ) {

            static_assert( size == 4, "height requires a 4 dimensional vector" );

            return at( Y2 ) - at( Y );

        }


    };

    using v2 = vector< float, 2u >;
    using v3 = vector< float, 3u >;
    using v4 = vector< float, 4u >;

    using v2i = vector< int, 2u >;
    using v3i = vector< int, 3u >;
    using v4i = vector< int, 4u >;

}

template < math::number t, std::size_t n >
struct std::formatter< math::vector< t, n > > : std::formatter< std::string > {

    auto format( math::vector< t, n > p, format_context& ctx ) {

        std::string buf = "[ ";

        for ( std::size_t i = 0; i < n; i++ ) {

            buf.append( "{}" );

            if ( i != n - 1 )
                buf.append( " - " );

            buf = std::format( buf, p[ i ] );

        }

        buf.append( " ]" );

        return formatter< string >::format( buf, ctx );

    }

};