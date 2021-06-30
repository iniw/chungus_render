#include "drawlist.h"

#include <algorithm>
#include <cmath>
#include <iostream>

// managing memory manually can use more memory but is faster
// if u guys want i can use vectors and let them manage it for me but it'll perform "worse"
void renderer::s_draw_list::init() {
   m_points.size = 0;
   m_points.capacity = INITIAL_POINT_BUFFER_SIZE * sizeof(s_point);
   m_points.buffer = reinterpret_cast<s_point*>(std::malloc(m_points.capacity));
   
   m_vertices.size = 0;
   m_vertices.capacity = INITIAL_VERTEX_BUFFER_SIZE * sizeof(d3d9_vertex);
   m_vertices.buffer = reinterpret_cast<d3d9_vertex*>(std::malloc(m_vertices.capacity));
   
   m_indices.size = 0;
   m_indices.capacity = INITIAL_INDEX_BUFFER_SIZE * sizeof(d3d9_index);
   m_indices.buffer = reinterpret_cast<d3d9_index*>(std::malloc(m_indices.capacity));
   
   m_draw_cmds.size = 0;
   m_draw_cmds.capacity = INITIAL_DRAW_CMD_BUFFER_SIZE * sizeof(s_draw_cmd);
   m_draw_cmds.buffer = reinterpret_cast<s_draw_cmd*>(std::malloc(m_draw_cmds.capacity));
}

bool renderer::s_draw_list::reserve(size_t num_vertices, size_t num_indices) {
    size_t desired_capacity = (m_vertices.size + num_vertices) * sizeof(d3d9_vertex);
    if (desired_capacity > m_vertices.capacity) {
        const size_t old_size = m_vertices.capacity;

        // allocate the new memory
        void* mem = std::malloc(desired_capacity);
        if (!mem)
            return false;

        // copy old to new
        std::memcpy(mem, m_vertices.buffer, m_vertices.capacity);
        // free old
        std::free(m_vertices.buffer);

        // swap pointers and set size
        m_vertices.buffer = static_cast<d3d9_vertex*>(mem);
        m_vertices.capacity = desired_capacity;
        /*
        std::cout << "allocated " << desired_capacity - old_size << "b for the vertex buffer\n";
        std::cout << "total capacity is now: " << m_vertices.capacity << "b\n";
        */
    }

    desired_capacity = (m_indices.size + num_indices) * sizeof(d3d9_index);
    if (desired_capacity > m_indices.capacity) {
        const size_t old_size = m_indices.capacity;

        // allocate the new memory
        void* mem = std::malloc(desired_capacity);
        if (!mem)
            return false;

        // copy old to new
        std::memcpy(mem, m_indices.buffer, m_indices.capacity);
        // free old
        std::free(m_indices.buffer);

        // swap pointers and set size
        m_indices.buffer = static_cast<d3d9_index*>(mem);
        m_indices.capacity = desired_capacity;
        /*
        std::cout << "allocated " << desired_capacity - old_size << "b for the index buffer\n";
        std::cout << "total capacity is now: " << m_indices.capacity << "b\n";
        */
    }

    return true;
}

bool renderer::s_draw_list::reserve_points(size_t num_points) {
    const size_t desired_size = (m_points.size + num_points) * sizeof(s_point);
    if (desired_size > m_points.capacity) {
        const size_t old_size = m_points.capacity;

        // allocate the new memory
        void* mem = std::malloc(desired_size);
        if (!mem)
            return false;

        // copy old to new
        std::memcpy(mem, m_points.buffer, m_points.capacity);
        // free old
        std::free(m_points.buffer);

        // swap pointers and set size
        m_points.buffer = static_cast<s_point*>(mem);
        m_points.capacity = desired_size;
        /*
        std::cout << "allocated: " << desired_size - old_size << "b for the points buffer\n";
        std::cout << "total capacity is now: " << m_points.capacity << "b\n";
        */
    }

    return true;
}

void renderer::s_draw_list::push_draw_cmd() {
    // calculate the number of vertices and indices present in draw commands
    size_t num_accounted_vertices = 0, num_accounted_indices = 0;
    for (size_t i = 0; i < m_draw_cmds.size; i++) {
        num_accounted_vertices += m_draw_cmds.buffer[i].m_num_vertices;
        num_accounted_indices += m_draw_cmds.buffer[i].m_num_indices;
    }

    // calculate the number of vertices and indices NOT present in draw commands
    size_t num_unaccounted_indices = m_indices.size - num_accounted_indices;

    // create draw commands until all vertices and indices are present in draw commands
    while (num_unaccounted_indices > 0) {
        // if the number of unaccounted indices is less than the maximum number of indices that can be hold by d3d9_index
        if (num_unaccounted_indices < UINT16_MAX) {
            // add draw command
            m_draw_cmds.buffer[m_draw_cmds.size].m_num_vertices = m_vertices.size - num_accounted_vertices;
            m_draw_cmds.buffer[m_draw_cmds.size].m_num_indices = m_indices.size - num_accounted_indices;

            m_draw_cmds.size++;
            return;
        } else {
            size_t num_indices = UINT16_MAX;
            d3d9_index last_index = m_indices.buffer[num_indices - 1];

            bool is_last_index_referenced = false;
            do {
                for (size_t i = num_indices; i < num_unaccounted_indices; i++) {
                    if (m_indices.buffer[i] == last_index) {
                        is_last_index_referenced = true;
                        num_indices -= 3;
                        last_index = m_indices.buffer[num_indices - 1];
                        break;
                    }
                }
            } while (is_last_index_referenced);

            m_draw_cmds.buffer[m_draw_cmds.size].m_num_vertices = last_index + 1;
            m_draw_cmds.buffer[m_draw_cmds.size].m_num_indices = num_indices;

            m_draw_cmds.size++;

            num_unaccounted_indices -= num_indices;
        }
    }
}

void renderer::s_draw_list::path_stroke(const s_color& color) {
    add_polyline(m_points.buffer, m_points.size, color, 1.f);
    path_clear();
}

void renderer::s_draw_list::add_polyline(const s_point* points, const size_t num_points, const s_color& color, float thickness) {
    if (num_points < 2)
        return;

    const d3d_color d3d_col = color.to_d3d();

    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/rendering-from-vertex-and-index-buffers
    const size_t num_indices = num_points * 6;
    const size_t num_vertices = num_points * 4;

    if (!reserve(num_vertices, num_indices))
        return;
    for (size_t i = 0; i < num_points; i++) {
        const size_t j = (i + 1) == num_points ? 0 : i + 1;
        const s_point& p0 = points[i];
        const s_point& p1 = points[j];

        s_point diff = (p1 - p0).normalize();

        diff *= 0.5f;

        const size_t offset = m_vertices.size;

        // get a ptr to the last element of the buffer
        d3d9_index* indices = m_indices.buffer + m_indices.size;

        // set the new 6 indices
        indices[0] = offset + 0; indices[1] = offset + 1; indices[2] = offset + 2;
        indices[3] = offset + 0; indices[4] = offset + 2; indices[5] = offset + 3;

        // add offset for the next iteration
        m_indices.size += 6;

        // get a ptr to the last element of the buffer
        d3d9_vertex* vertices = m_vertices.buffer + m_vertices.size;

        // set the new 4 new vertices
        vertices[0].pos.x = p0.x + diff.y; vertices[0].pos.y = p0.y - diff.x; vertices[0].color = d3d_col;
        vertices[1].pos.x = p1.x + diff.y; vertices[1].pos.y = p1.y - diff.x; vertices[1].color = d3d_col;
        vertices[2].pos.x = p1.x - diff.y; vertices[2].pos.y = p1.y + diff.x; vertices[2].color = d3d_col;
        vertices[3].pos.x = p0.x - diff.y; vertices[3].pos.y = p0.y + diff.x; vertices[3].color = d3d_col;

        // add offset for the next iteration
        m_vertices.size += 4;
    }
}

void renderer::s_draw_list::add_rect(const s_rect& rect, const s_color& color) {
    path_to({rect.x, rect.y});
    path_to({rect.x + rect.w, rect.y});
    path_to({rect.x + rect.w, rect.y + rect.h});
    path_to({rect.x, rect.y + rect.h});

    path_stroke(color);
}

void renderer::s_draw_list::add_triangle(const s_point& point1, const s_point& point2, const s_point& point3, const s_color& color) {
    path_to(point1);
    path_to(point2);
    path_to(point3);

    path_stroke(color);
}

#pragma region d3d9
void renderer::d3d9::init(LPDIRECT3DDEVICE9 d3d9_device, const s_vec2& display_size) {
    set_device(d3d9_device);
    set_size(display_size);
    create_vertex_declaration();
}

void renderer::d3d9::set_device(LPDIRECT3DDEVICE9 d3d9_device) {
    info.device = d3d9_device;

    info.viewport.X;
    info.viewport.Y = 0;
    info.viewport.MinZ = 0.0f;
    info.viewport.MaxZ = 1.0f;
}

void renderer::d3d9::set_size(const s_vec2& size) {
    const float L = 0.0f;
    const float R = size.x + 0.0f;
    const float T = 0.0f;
    const float B = size.y + 0.0f;
    float matrix[4][4] = {
        { 2.0f / (R - L),     0.0f,             0.0f, 0.0f },
        { 0.0f,               2.0f / (T - B),   0.0f, 0.0f },
        { 0.0f,               0.0f,             0.0f, 0.0f },
        { (R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f },
    };
    std::memcpy(&info.proj, matrix, sizeof(matrix));

    info.viewport.Width = (DWORD)size.x;
    info.viewport.Height = (DWORD)size.y;
}

void renderer::d3d9::set_render_states() {
    info.device->SetStreamSource(0, info.vb, 0, sizeof(d3d9_vertex));
    info.device->SetIndices(info.ib);
    info.device->SetVertexDeclaration(info.vertex_declaration);
    info.device->SetTransform(D3DTS_PROJECTION, &info.proj);
    info.device->SetViewport(&info.viewport);
    info.device->SetPixelShader(NULL);
    info.device->SetVertexShader(NULL);

    // copied from imgui
    info.device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    info.device->SetRenderState(D3DRS_LIGHTING, false);
    info.device->SetRenderState(D3DRS_ZENABLE, false);
    info.device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
    info.device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
    info.device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    info.device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    info.device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    info.device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
    info.device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    info.device->SetRenderState(D3DRS_FOGENABLE, false);
    info.device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
    info.device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, false);
    info.device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
    info.device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
    info.device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    info.device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    info.device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    info.device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    info.device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    info.device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    info.device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
}

void renderer::d3d9::create_vertex_declaration() {
    constexpr D3DVERTEXELEMENT9 decl_elements[] =       
    {
        {0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        //{0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0}, add back when i add textures
        D3DDECL_END()
    };

    info.device->CreateVertexDeclaration(decl_elements, &info.vertex_declaration);
}

void renderer::d3d9::render(s_draw_list* draw_list) {
    // if we haven't created our vertex buffer yet or it's not big enough
    if (!info.vb || info.vb_size < draw_list->m_vertices.size) {
        // Delete it
        if (info.vb) {
            info.vb->Release();
            info.vb = 0;
        }

        std::cout << "creating a new vertex buffer. old size: " << info.vb_size << " new size: " << (draw_list->m_vertices.size + INITIAL_VERTEX_BUFFER_SIZE) * sizeof(d3d9_vertex) << "\n";

        // create the new buffer
        info.vb_size = draw_list->m_vertices.size + INITIAL_VERTEX_BUFFER_SIZE;
        if (info.device->CreateVertexBuffer(info.vb_size * sizeof(d3d9_vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &info.vb, NULL) < 0)
            return;
    }

    // if we haven't created our index buffer yet or it's not big enough
    if (!info.ib || info.ib_size < draw_list->m_indices.size) {
        // Delete it
        if (info.ib) {
            info.ib->Release();
            info.ib = 0;
        }

        std::cout << "creating a new index buffer. old size: " << info.ib_size << " new size: " << (draw_list->m_indices.size + INITIAL_INDEX_BUFFER_SIZE) * sizeof(d3d9_index) << "\n";

        // create the new buffer
        info.ib_size = draw_list->m_indices.size + INITIAL_INDEX_BUFFER_SIZE;
        if (info.device->CreateIndexBuffer(info.ib_size * sizeof(d3d9_index), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(d3d9_index) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &info.ib, NULL) < 0)
            return;
    }
    {
        // copy vertices to gpu
        d3d9_vertex* new_vertices = 0;
        if (info.vb->Lock(0, draw_list->m_vertices.size * sizeof(d3d9_vertex), reinterpret_cast<void**>(&new_vertices), D3DLOCK_DISCARD) < 0)
            return;

        std::memcpy(new_vertices, draw_list->m_vertices.buffer, draw_list->m_vertices.size * sizeof(d3d9_vertex));

        info.vb->Unlock();
    }
    {
        // copy indices to the gpu
        d3d9_index* new_indices = 0;
        if (info.ib->Lock(0, draw_list->m_indices.size * sizeof(d3d9_index), reinterpret_cast<void**>(&new_indices), D3DLOCK_DISCARD) < 0)
            return;

        std::memcpy(new_indices, draw_list->m_indices.buffer, draw_list->m_indices.size * sizeof(d3d9_index));

        info.ib->Unlock();
    }

    set_render_states();

    size_t index_buffer_offset = 0;
    for (size_t i = 0; i < draw_list->m_draw_cmds.size; i++) {
        auto& cmd = draw_list->m_draw_cmds.buffer[i];

        info.device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, cmd.m_num_indices, index_buffer_offset, cmd.m_num_indices / 3);
        index_buffer_offset += cmd.m_num_indices;

    }
}
#pragma endregion