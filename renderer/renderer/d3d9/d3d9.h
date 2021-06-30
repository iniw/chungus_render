#pragma once
#include <d3d9.h>
#include "../utils/datatypes.h"

namespace renderer {
	// imgui
	#define INITIAL_VERTEX_BUFFER_SIZE 1000
	#define INITIAL_INDEX_BUFFER_SIZE 2000

	namespace d3d9 {
		struct object {
		public:
			LPDIRECT3DDEVICE9 m_device = 0;
			LPDIRECT3DVERTEXBUFFER9 m_vb = 0; size_t m_vb_size; // vertex buffer
			LPDIRECT3DINDEXBUFFER9 m_ib = 0; size_t m_ib_size; // index buffer
			D3DMATRIX m_proj;
			D3DVIEWPORT9 m_viewport;
			LPDIRECT3DVERTEXDECLARATION9 m_vtx_decl; // our custom vertex declaration
		public:
			void init(LPDIRECT3DDEVICE9 d3d9_device, const s_vec2& display_size);
			void set_size(const s_vec2& size);
			void create_vertex_declaration();
			void create_buffers();

			void set_render_states();
		};
	}
}