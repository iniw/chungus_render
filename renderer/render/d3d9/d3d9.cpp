#include "d3d9.h"

#include <cstring>

bool render::d3d9::object::init( IDirect3DDevice9* d3d9_device ) {

    m_device = d3d9_device;

   //D3DVIEWPORT9 viewport;
   //m_device->GetViewport( &viewport );

    set_size( math::v2i( 1280, 720 ) );

    if ( !setup_vertex_declaration( ) )
        return false;

    if ( !setup_buffers( ) )
        return false;

    return true;

}

void render::d3d9::object::invalidate( ) {

    if ( m_ib ) {

        m_ib->Release( );
        m_ib = nullptr;

        m_ib_size = 0;

    }

    if ( m_vb ) {

        m_vb->Release( );
        m_vb = nullptr;

        m_vb_size = 0;

    }

}

void render::d3d9::object::set_size( const math::v2i& size ) {

    const float L = 0.0f;
    const float R = static_cast< float >( size[ X ] );
    const float T = 0.0f;
    const float B = static_cast< float >( size[ Y ] );

    float matrix[ 4 ][ 4 ] = {

        { 2.f / ( R - L ),     0.f,             0.f, 0.f },
        { 0.f,               2.f / ( T - B ),   0.f, 0.f },
        { 0.f,               0.f,             0.f, 0.f },
        { ( R + L ) / ( L - R ), ( T + B ) / ( B - T ), 0.f, 1.f },

    };

    std::memcpy( &m_proj, matrix, sizeof( matrix ) );

    m_viewport.X = 0;
    m_viewport.Y = 0;
    m_viewport.MinZ = 0.f;
    m_viewport.MaxZ = 1.f;
    m_viewport.Width = size[ X ];
    m_viewport.Height = size[ Y ];

}

bool render::d3d9::object::setup_vertex_declaration( ) {

    constexpr D3DVERTEXELEMENT9 decl_elements[ ] = {

        { 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
        D3DDECL_END( )

    };

    return m_device->CreateVertexDeclaration( decl_elements, &m_vtx_decl ) == D3D_OK;
}

bool render::d3d9::object::setup_buffers( ) {

    m_vb = nullptr;
    m_vb_size = INITIAL_VERTEX_BUFFER_SIZE;
    if ( m_device->CreateVertexBuffer( m_vb_size * sizeof( vertex ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb, NULL ) != D3D_OK )
        return false;

    m_ib = nullptr;
    m_ib_size = INITIAL_INDEX_BUFFER_SIZE;
    if ( m_device->CreateIndexBuffer( m_ib_size * sizeof( index ), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib, NULL ) != D3D_OK )
        return false;

    return true;

}

render::d3d9::tex_id render::d3d9::object::create_texture( const std::uint8_t* data, const int width, const int height ) {

    IDirect3DTexture9* temp_tex;
    if ( m_device->CreateTexture( width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &temp_tex, nullptr ) != D3D_OK )
        return nullptr;

    D3DLOCKED_RECT lock_rect;
    if ( temp_tex->LockRect( 0, &lock_rect, nullptr, D3DLOCK_DISCARD ) != D3D_OK ) {

        temp_tex->Release( );
        return nullptr;

    }

    for ( int y = 0; y < height; ++y ) {

        const auto dest = reinterpret_cast< std::uint32_t* >( ( unsigned char* )lock_rect.pBits + lock_rect.Pitch * y );
        std::memcpy( dest, data + width * 4 * y, width * 4 );

        for ( int i = 0; i < width; ++i ) {

            auto color = dest[ i ];
            dest[ i ] = ( color & 0xFF00FF00 ) | ( ( color & 0xFF0000 ) >> 16 ) | ( ( color & 0xFF ) << 16 ); // rgba to brga

        }

    }

    temp_tex->UnlockRect( 0 );

    IDirect3DTexture9* tex;
    if ( m_device->CreateTexture( width, height, 1, D3DUSAGE_AUTOGENMIPMAP, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &tex, nullptr ) != D3D_OK ) {

        temp_tex->Release( );
        return nullptr;

    }

    m_device->UpdateTexture( temp_tex, tex );
    temp_tex->Release( );

    return tex;

}

void render::d3d9::object::set_render_states( ) {

    m_device->SetStreamSource( 0, m_vb, 0, sizeof( vertex ) );
    m_device->SetIndices( m_ib );
    m_device->SetVertexDeclaration( m_vtx_decl );

    m_device->SetTransform( D3DTS_WORLD, &m_identity );
    m_device->SetTransform( D3DTS_VIEW, &m_identity );
    m_device->SetTransform( D3DTS_PROJECTION, &m_proj );
    m_device->SetViewport( &m_viewport );

    m_device->SetPixelShader( nullptr );
    m_device->SetVertexShader( nullptr );

    // copied from imgui
    m_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    m_device->SetRenderState( D3DRS_LIGHTING, false );
    m_device->SetRenderState( D3DRS_ZENABLE, false );
    m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    m_device->SetRenderState( D3DRS_ALPHATESTENABLE, false );
    m_device->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
    m_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    m_device->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
    m_device->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    m_device->SetRenderState( D3DRS_FOGENABLE, false );
    m_device->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, false );
    m_device->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, false );
    m_device->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
    m_device->SetRenderState( D3DRS_SRGBWRITEENABLE, false );

    m_device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_device->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

    m_device->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_device->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

}