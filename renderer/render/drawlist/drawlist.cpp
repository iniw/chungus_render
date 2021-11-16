#include "drawlist.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../lib/stb_truetype.h"

#include <algorithm>
#include <cmath>
#include <iostream>

bool render::drawlist::init( ) {

    if ( !m_vertices.init( INITIAL_VERTEX_BUFFER_SIZE ) )
        return false;

    if ( !m_indices.init( INITIAL_INDEX_BUFFER_SIZE ) )
        return false;

    if ( !m_points.init( INITIAL_POINT_BUFFER_SIZE ) )
        return false;

    if ( !m_draw_cmds.init( INITIAL_DRAW_CMD_BUFFER_SIZE ) )
        return false;

    return true;

}

void render::drawlist::start( ) {

    m_vertices.m_size = 0;
    m_indices.m_size = 0;
    m_points.m_size = 0;
    m_draw_cmds.m_size = 0;

}

void render::drawlist::invalidate( ) {

    m_vertices.clear( );
    m_indices.clear( );
    m_points.clear( );
    m_draw_cmds.clear( );

   // g_io.log( "cleared render buffers" );

}

void render::drawlist::add_polyline( const buffer< point >* points, const d3d9::color color ) {

    if ( points->m_size < 2 )
        return;

    const std::size_t num_indices = points->m_size * 6;
    const std::size_t num_vertices = points->m_size * 4;

    if ( !m_vertices.reserve( 4 * points->m_size ) || !m_indices.reserve( 6 * points->m_size ) )
        return;

    for ( std::size_t i = 0; i < points->m_size; i++ ) {

        const std::size_t j = ( i + 1 == points->m_size ) ? 0 : i + 1;

        const point& p1 = points->m_buffer[ i ];
        const point& p2 = points->m_buffer[ j ];

        auto diff = p1 - p2;

        diff.normalize( );
        diff *= 0.5f;

        const std::size_t offset = m_vertices.m_size;

        {

            d3d9::index* indices = m_indices.end( );

            indices[ 0 ] = offset + 0; indices[ 1 ] = offset + 1; indices[ 2 ] = offset + 2;
            indices[ 3 ] = offset + 0; indices[ 4 ] = offset + 2; indices[ 5 ] = offset + 3;

            m_indices.m_size += 6;

        }

        {

            d3d9::vertex* vertices = m_vertices.end( );

            vertices[ 0 ].m_pos[ X ] = p1[ X ] + diff[ Y ]; vertices[ 0 ].m_pos[ Y ] = p1[ Y ] - diff[ X ]; vertices[ 0 ].m_uv = 0.f; vertices[ 0 ].m_color = color;
            vertices[ 1 ].m_pos[ X ] = p2[ X ] + diff[ Y ]; vertices[ 1 ].m_pos[ Y ] = p2[ Y ] - diff[ X ]; vertices[ 1 ].m_uv = 0.f; vertices[ 1 ].m_color = color;
            vertices[ 2 ].m_pos[ X ] = p2[ X ] - diff[ Y ]; vertices[ 2 ].m_pos[ Y ] = p2[ Y ] + diff[ X ]; vertices[ 2 ].m_uv = 0.f; vertices[ 2 ].m_color = color;
            vertices[ 3 ].m_pos[ X ] = p1[ X ] - diff[ Y ]; vertices[ 3 ].m_pos[ Y ] = p1[ Y ] + diff[ X ]; vertices[ 3 ].m_uv = 0.f; vertices[ 3 ].m_color = color;

            m_vertices.m_size += 4;

        }

    }

}

void render::drawlist::add_polyline_filled( const buffer< point >* points, const d3d9::color color ) {

    if ( points->m_size < 3 )
        return;

    if ( !m_vertices.reserve( points->m_size ) || !m_indices.reserve( points->m_size - 2 ) * 3 )
        return;

    {

        const std::size_t offset = m_vertices.m_size;

        d3d9::index* indices = m_indices.end( );

        for ( std::size_t i = 2; i < points->m_size; i++ )
            indices[ ( i - 2 ) * 3 + 0 ] = offset, indices[ ( i - 2 ) * 3 + 1 ] = offset + ( i - 1 ), indices[ ( i - 2 ) * 3 + 2 ] = offset + i;

        m_indices.m_size += ( points->m_size - 2 ) * 3;

    }

    {

        d3d9::vertex* vertices = m_vertices.end( );

        for ( std::size_t i = 0; i < points->m_size; i++ )
            vertices[ i ].m_pos = points->m_buffer[ i ], vertices[ i ].m_uv = 0.f, vertices[ i ].m_color = color;

        m_vertices.m_size += points->m_size;

    }

}

void render::drawlist::add_line( const point& point1, const point& point2, const d3d9::color color ) {

    if ( !m_points.reserve( 2 ) )
        return;

    add_point( point1 );
    add_point( point2 );

    draw_path( color );

}

void render::drawlist::add_rect( const point& point1, const point& point2, const d3d9::color color ) {

    if ( !m_points.reserve( 4 ) )
        return;

    add_point( point1 );
    add_point( point( point2[ X ], point1[ Y ] ) );
    add_point( point2 );
    add_point( point( point1[ X ], point2[ Y ] ) );

    draw_path( color );

}

void render::drawlist::add_rect_filled( const point& point1, const point& point2, const d3d9::color color ) {

    if ( !m_points.reserve( 4 ) )
        return;

    add_point( point1 );
    add_point( point( point2[ X ], point1[ Y ] ) );
    add_point( point2 );
    add_point( point( point1[ X ], point2[ Y ] ) );

    draw_path_filled( color );

}

void render::drawlist::add_texture( const d3d9::tex_id id, point pos, const math::v2i& size, d3d9::color color ) {

    push_cmd( );

    add_quad( pos, point( pos[ X ] + size[ X ], pos[ Y ] ), point( pos[ X ] + size[ X ], pos[ Y ] + size[ Y ] ), point( pos[ X ], pos[ Y ] + size[ Y ] ), color );

    push_tex_cmd( id );

}

void render::drawlist::add_text( font* font, std::string_view text, point& pos, d3d9::color color ) {

    if ( !m_vertices.reserve( text.length( ) * 4 ) || !m_indices.reserve( text.length( ) * 6 ) )
        return;

    push_cmd( );

    stbtt_aligned_quad q;
    for ( auto& letter : text ) {

        stbtt_GetBakedQuad( ( stbtt_bakedchar* )font->m_baked_chars, 512, 512, letter - 32, &pos[ X ], &pos[ Y ], &q, 1 );

        const std::size_t offset = m_vertices.m_size;

        d3d9::index* indices = m_indices.end( );

        indices[ 0 ] = offset + 0; indices[ 1 ] = offset + 1; indices[ 2 ] = offset + 2;
        indices[ 3 ] = offset + 0; indices[ 4 ] = offset + 2; indices[ 5 ] = offset + 3;

        m_indices.m_size += 6;

        d3d9::vertex* vertices = m_vertices.end( );

        vertices[ 0 ].m_pos[ X ] = q.x0; vertices[ 0 ].m_pos[ Y ] = q.y0; vertices[ 0 ].m_uv[ X ] = q.s0; vertices[ 0 ].m_uv[ Y ] = q.t0; vertices[ 0 ].m_color = color;
        vertices[ 1 ].m_pos[ X ] = q.x1; vertices[ 1 ].m_pos[ Y ] = q.y0; vertices[ 1 ].m_uv[ X ] = q.s1; vertices[ 1 ].m_uv[ Y ] = q.t0; vertices[ 1 ].m_color = color;
        vertices[ 2 ].m_pos[ X ] = q.x1; vertices[ 2 ].m_pos[ Y ] = q.y1; vertices[ 2 ].m_uv[ X ] = q.s1; vertices[ 2 ].m_uv[ Y ] = q.t1; vertices[ 2 ].m_color = color;
        vertices[ 3 ].m_pos[ X ] = q.x0; vertices[ 3 ].m_pos[ Y ] = q.y1; vertices[ 3 ].m_uv[ X ] = q.s0; vertices[ 3 ].m_uv[ Y ] = q.t1; vertices[ 3 ].m_color = color;

        m_vertices.m_size += 4;
    }

    push_tex_cmd( font->m_tex_id );

}

void render::drawlist::push_cmd( ) {

    std::size_t num_accounted_vertices = 0, num_accounted_indices = 0;

    for ( std::size_t i = 0; i < m_draw_cmds.m_size; i++ ) {

        num_accounted_vertices += m_draw_cmds[ i ].m_num_vertices;
        num_accounted_indices += m_draw_cmds[ i ].m_num_indices;

    }

    std::size_t num_unaccounted_indices = m_indices.m_size - num_accounted_indices;

    while ( num_unaccounted_indices > 0 ) {

        if ( num_unaccounted_indices < UINT16_MAX ) {

            m_draw_cmds[ m_draw_cmds.m_size ].m_num_vertices = m_vertices.m_size - num_accounted_vertices;
            m_draw_cmds[ m_draw_cmds.m_size ].m_num_indices = m_indices.m_size - num_accounted_indices;
            m_draw_cmds[ m_draw_cmds.m_size ].m_tex_id = nullptr;

            m_draw_cmds.m_size++;

            return;

        } else {

            std::size_t num_indices = UINT16_MAX;
            d3d9::index last_index = m_indices[ num_indices - 1 ];

            bool is_last_index_referenced = false;

            do {

                for ( std::size_t i = num_indices; i < num_unaccounted_indices; i++ ) {

                    if ( m_indices[ i ] == last_index ) {

                        is_last_index_referenced = true;
                        num_indices -= 3;
                        last_index = m_indices[ num_indices - 1 ];
                        break;

                    }

                }

            } while ( is_last_index_referenced );

            m_draw_cmds[ m_draw_cmds.m_size ].m_num_vertices = last_index + 1;
            m_draw_cmds[ m_draw_cmds.m_size ].m_num_indices = num_indices;
            m_draw_cmds[ m_draw_cmds.m_size ].m_tex_id = nullptr;

            m_draw_cmds.m_size++;

            num_unaccounted_indices -= num_indices;

        }

    }

}

void render::drawlist::push_tex_cmd( d3d9::tex_id id ) {

    std::size_t num_accounted_vertices = 0, num_accounted_indices = 0;

    for ( std::size_t i = 0; i < m_draw_cmds.m_size; i++ ) {

        num_accounted_vertices += m_draw_cmds[ i ].m_num_vertices;
        num_accounted_indices += m_draw_cmds[ i ].m_num_indices;

    }

    m_draw_cmds[ m_draw_cmds.m_size ].m_num_vertices = m_vertices.m_size - num_accounted_vertices;
    m_draw_cmds[ m_draw_cmds.m_size ].m_num_indices = m_indices.m_size - num_accounted_indices;
    m_draw_cmds[ m_draw_cmds.m_size ].m_tex_id = id;

    m_draw_cmds.m_size++;

}

void render::drawlist::draw( d3d9::object* obj ) {

    // this avoids rendering and creating the buffers if we don't have anything to render
    if ( !m_vertices.m_size || !m_indices.m_size )
        return;

    if ( m_vertices.m_size >= obj->m_vb_size ) {

        if ( obj->m_vb ) {

            obj->m_vb->Release( );
            obj->m_vb = nullptr;

        }

        obj->m_vb_size = m_vertices.m_size + INITIAL_VERTEX_BUFFER_SIZE;

        if ( obj->m_device->CreateVertexBuffer( obj->m_vb_size * sizeof( d3d9::vertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &obj->m_vb, NULL ) != D3D_OK )
            return;

        const auto capacity = obj->m_vb_size * sizeof( d3d9::vertex );

        //g_io.log( "created new vertex buffer with capacity {}b ( {}mb )", capacity, capacity / 1024 );

    }

    if ( m_indices.m_size >= obj->m_ib_size ) {

        if ( obj->m_ib ) {

            obj->m_ib->Release( );
            obj->m_ib = nullptr;

        }

        obj->m_ib_size = m_indices.m_size + INITIAL_INDEX_BUFFER_SIZE;

        if ( obj->m_device->CreateIndexBuffer( obj->m_ib_size * sizeof( d3d9::index ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &obj->m_ib, NULL ) != D3D_OK )
            return;

        const auto capacity = obj->m_ib_size * sizeof( d3d9::index );

       // g_io.log( "created new index buffer with capacity {}b ( {}mb )", capacity, capacity / 1024 );

    }

    {

        d3d9::vertex* new_vertices = nullptr;

        if ( obj->m_vb->Lock( 0, m_vertices.m_size * sizeof( d3d9::vertex ), reinterpret_cast< void** >( &new_vertices ), D3DLOCK_DISCARD ) != D3D_OK )
            return;

        std::memcpy( new_vertices, m_vertices.m_buffer, m_vertices.m_size * sizeof( d3d9::vertex ) );

        obj->m_vb->Unlock( );

    }

    {

        d3d9::index* new_indices = nullptr;

        if ( obj->m_ib->Lock( 0, m_indices.m_size * sizeof( d3d9::index ), reinterpret_cast< void** >( &new_indices ), D3DLOCK_DISCARD ) != D3D_OK )
            return;

        std::memcpy( new_indices, m_indices.m_buffer, m_indices.m_size * sizeof( d3d9::index ) );

        obj->m_ib->Unlock( );

    }

    obj->set_render_states( );

    std::size_t index_buffer_offset = 0;
    for ( const auto& cmd : m_draw_cmds ) {

        obj->m_device->SetTexture( 0, cmd.m_tex_id );

        obj->m_device->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, cmd.m_num_indices, index_buffer_offset, cmd.m_num_indices / 3 );

        index_buffer_offset += cmd.m_num_indices;

    }

}