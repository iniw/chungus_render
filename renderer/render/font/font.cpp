#include "../lib/stb_truetype.h"
#include "../render.h"

render::font render::font::from_memory( const std::uint8_t* data, const float size ) {

    font obj;

    obj.m_width = 512;
    obj.m_height = 512;

    obj.m_pixels8 = static_cast< std::uint8_t* >( std::malloc( obj.m_width * obj.m_height ) );
    obj.m_pixels32 = static_cast< color* >( std::malloc( obj.m_width * obj.m_height * 4 ) );
    obj.m_baked_chars = std::malloc( sizeof( stbtt_bakedchar ) * 96 );

    stbtt_BakeFontBitmap( data, 0, size, obj.m_pixels8, obj.m_width, obj.m_height, 0x20, 96, ( stbtt_bakedchar* )obj.m_baked_chars );

    for ( int i = 0; i < obj.m_width * obj.m_height; i++ )
        obj.m_pixels32[ i ] = color( 255, 255, 255, obj.m_pixels8[ i ] );

    obj.m_tex_id = g_render.create_texture( reinterpret_cast< std::uint8_t* >( obj.m_pixels32 ), obj.m_width, obj.m_height );

    return obj;

}