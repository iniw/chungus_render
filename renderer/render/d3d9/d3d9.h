#pragma once

#include <d3d9.h>
#include <d3dx9core.h>

#include "../types/types.h"

namespace render {

	enum {

		INITIAL_VERTEX_BUFFER_SIZE = 2500,
		INITIAL_INDEX_BUFFER_SIZE = 5000

	};

	namespace d3d9 {

		struct object {

			IDirect3DDevice9* m_device = 0;

			IDirect3DVertexBuffer9* m_vb = 0; std::size_t m_vb_size;
			IDirect3DIndexBuffer9* m_ib = 0; std::size_t m_ib_size;

			D3DMATRIX m_proj;

			D3DMATRIX m_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };

			D3DVIEWPORT9 m_viewport;

			IDirect3DVertexDeclaration9* m_vtx_decl; // custom vertex declaration

		public:

			bool init( IDirect3DDevice9* d3d9_device );

			void invalidate( );

		public:

			void set_size( const math::v2i& size );

			bool setup_vertex_declaration( );

			bool setup_buffers( );

			// s/o daniel krupinski
			tex_id create_texture( const std::uint8_t* data, const int width, const int height );

			void set_render_states( );

		};

	}

}