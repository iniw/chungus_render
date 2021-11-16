#pragma once
#include <cstdint>
#include <memory>
namespace render {

	template< typename t >
	struct buffer {

	size_t m_size;
		size_t m_capacity;
		t* m_buffer;

	private:

		size_t m_initial_capacity;

	public:

		constexpr buffer( ) : m_size{ 0u }, m_capacity{ 0u }, m_buffer{ nullptr }, m_initial_capacity{ 0u } { }

		// no error handling, u better not pass an invalid index
		inline t& operator [] ( const size_t idx ) {

			return m_buffer[ idx ];

		}

		inline bool init( const size_t size ) {

			m_initial_capacity = size * sizeof( t );
			m_capacity = m_initial_capacity;
			m_buffer = static_cast< t* >( malloc( m_capacity ) );

			// I NEED TO MAKE A WRAPPER FOR COMPILE TIME STRINGS. URGENT.
			//g_io.log( "initialized {} with {}b ( {}mb )", utils::cx::data_holder< utils::cx::type_name< t >( ) >::value.data( ), m_capacity, m_capacity / 1024 );

			return m_buffer;

		}

		inline void clear( ) {

			if ( m_buffer && m_capacity )
				free( m_buffer );

			m_capacity = 0;
			m_size = 0;

		}

		inline bool grow( const size_t new_capacity ) {

			void* mem = malloc( new_capacity );
			if ( !mem )
				return false;

			if ( m_buffer && m_capacity ) {

				memcpy( mem, m_buffer, m_capacity );
				free( m_buffer );

			}

			m_capacity = new_capacity;
			m_buffer = static_cast< t* >( mem );

			//g_io.log( "grew {} by {}b ( {}mb ), total capacity is now {}b ( {}mb )", utils::cx::data_holder< utils::cx::type_name< t >( ) >::value.data( ), new_capacity, new_capacity / 1024, m_capacity, m_capacity / 1024 );

			return m_buffer;

		}

		inline bool reserve( const size_t amt ) {

			const size_t desired_capacity = ( m_size + amt ) * sizeof( t );
			if ( desired_capacity > m_capacity )
				return grow( desired_capacity + m_initial_capacity );

			return true;

		}

		inline t* begin( ) {

			return m_buffer;

		}

		inline t* end( ) {

			return m_buffer + m_size;

		}

	};

}