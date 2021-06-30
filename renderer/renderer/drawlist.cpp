#include "drawlist.h"

#include <algorithm>
#include <cmath>
#include <iostream>

// managing memory manually can use more memory but is faster
// if u guys want i can use vectors and let them manage it for me but it'll perform "worse"
void renderer::drawlist::init() {
    m_vertices.capacity = INITIAL_VERTEX_BUFFER_SIZE * sizeof(d3d9::vertex);
    m_vertices.buffer = reinterpret_cast<d3d9::vertex*>(std::malloc(m_vertices.capacity));

#ifdef LOG_MEM_CONSUMPTION
    std::printf("created the drawlist vertex buffer with capacity of %ub.\n", m_vertices.capacity);
#endif

    m_indices.capacity = INITIAL_INDEX_BUFFER_SIZE * sizeof(d3d9::index);
    m_indices.buffer = reinterpret_cast<d3d9::index*>(std::malloc(m_indices.capacity));

#ifdef LOG_MEM_CONSUMPTION
    std::printf("created the drawlist index buffer with capacity of %ub.\n", m_indices.capacity);
#endif

    m_points.capacity = INITIAL_POINT_BUFFER_SIZE * sizeof(s_point);
    m_points.buffer = reinterpret_cast<s_point*>(std::malloc(m_points.capacity));

#ifdef LOG_MEM_CONSUMPTION
    std::printf("created the drawlist points buffer with capacity of %ub.\n", m_points.capacity);
#endif
    
    m_draw_cmds.capacity = INITIAL_DRAW_CMD_BUFFER_SIZE * sizeof(drawcmd);
    m_draw_cmds.buffer = reinterpret_cast<drawcmd*>(std::malloc(m_draw_cmds.capacity));

#ifdef LOG_MEM_CONSUMPTION
    std::printf("created the drawlist draw_cmds buffer with capacity of %ub.\n", m_draw_cmds.capacity);
#endif
}

void renderer::drawlist::clear() {
    m_vertices.size = 0;
    m_indices.size = 0;
    m_draw_cmds.size = 0;
}

bool renderer::drawlist::reserve(const size_t& num_vertices, const size_t& num_indices) {
#ifdef LOG_MEM_CONSUMPTION
    const size_t old_vtx_capacity = m_vertices.capacity;
    const size_t old_idx_capacity = m_indices.capacity;
#endif
    size_t desired_capacity = (m_vertices.size + num_vertices) * sizeof(d3d9::vertex);
    if (desired_capacity > m_vertices.capacity) {

        // allocate the new memory
        void* mem = std::malloc(desired_capacity);
        if (!mem)
            return false;

        // copy old to new
        std::memcpy(mem, m_vertices.buffer, m_vertices.capacity);
        // free old
        //std::free(m_vertices.buffer);

        // swap pointers and set size
        m_vertices.buffer = static_cast<d3d9::vertex*>(mem);
        m_vertices.capacity = desired_capacity;

    #ifdef LOG_MEM_CONSUMPTION
        std::printf("allocated %ub for the vertex buffer.\n", desired_capacity - old_vtx_capacity);
        std::printf("total capacity is now %ub.\n", m_vertices.capacity);
    #endif
    }

    desired_capacity = (m_indices.size + num_indices) * sizeof(d3d9::index);
    if (desired_capacity > m_indices.capacity) {

        // allocate the new memory
        void* mem = std::malloc(desired_capacity);
        if (!mem)
            return false;

        // copy old to new
        std::memcpy(mem, m_indices.buffer, m_indices.capacity);
        // free old
        std::free(m_indices.buffer);

        // swap pointers and set size
        m_indices.buffer = static_cast<d3d9::index*>(mem);
        m_indices.capacity = desired_capacity;

    #ifdef LOG_MEM_CONSUMPTION
        std::printf("allocated %ub for the index buffer.\n", desired_capacity - old_idx_capacity);
        std::printf("total capacity is now %ub.\n", m_indices.capacity);
    #endif
    }

    return true;
}

bool renderer::drawlist::reserve_points(const size_t& num_points) {
#ifdef LOG_MEM_CONSUMPTION
    const size_t old_points_capacity = m_points.capacity;
#endif
    const size_t desired_capacity = (m_points.size + num_points) * sizeof(s_point);
    if (desired_capacity > m_points.capacity) {

        // allocate the new memory
        void* mem = std::malloc(desired_capacity);
        if (!mem)
            return false;

        // copy old to new
        std::memcpy(mem, m_points.buffer, m_points.capacity);
        // free old
        std::free(m_points.buffer);

        // swap pointers and set size
        m_points.buffer = static_cast<s_point*>(mem);
        m_points.capacity = desired_capacity;
        
    #ifdef LOG_MEM_CONSUMPTION
        std::printf("allocated %ub for the points buffer.\n", desired_capacity - old_points_capacity);
        std::printf("total capacity is now %ub.\n", m_points.capacity);
    #endif
    }

    return true;
}

void renderer::drawlist::add_polyline(const s_point* points, const size_t num_points, const d3d9::color color, const float thickness) {
    if (num_points < 2)
        return;

    const size_t num_indices = num_points * 6;
    const size_t num_vertices = num_points * 4;

    if (!reserve(num_vertices, num_indices))
        return;

    for (size_t i = 0; i < num_points; i++) {
        const size_t j = (i + 1) == num_points ? 0 : i + 1;
        const s_point& p0 = points[i];
        const s_point& p1 = points[j];

        s_point diff = (p1 - p0).normalize();

        diff *= thickness * 0.5f;

        const size_t offset = m_vertices.size;

        // get a ptr to the last element of the buffer
        d3d9::index* indices = m_indices.buffer + m_indices.size;

        // set the new 6 indices
        indices[0] = offset + 0; indices[1] = offset + 1; indices[2] = offset + 2;
        indices[3] = offset + 0; indices[4] = offset + 2; indices[5] = offset + 3;

        // add offset for the next iteration
        m_indices.size += 6;

        // get a ptr to the last element of the buffer
        d3d9::vertex* vertices = m_vertices.buffer + m_vertices.size;

        // set the new 4 new vertices
        vertices[0].pos.x = p0.x + diff.y; vertices[0].pos.y = p0.y - diff.x; vertices[0].color = color;
        vertices[1].pos.x = p1.x + diff.y; vertices[1].pos.y = p1.y - diff.x; vertices[1].color = color;
        vertices[2].pos.x = p1.x - diff.y; vertices[2].pos.y = p1.y + diff.x; vertices[2].color = color;
        vertices[3].pos.x = p0.x - diff.y; vertices[3].pos.y = p0.y + diff.x; vertices[3].color = color;

        // add offset for the next iteration
        m_vertices.size += 4;
    }
}

void renderer::drawlist::add_line(const s_point& point1, const s_point& point2, const d3d9::color color, const float thickness) {
    add_point(point1);
    add_point(point2);

    draw_path(color, thickness);
}

void renderer::drawlist::add_triangle(const s_point& point1, const s_point& point2, const s_point& point3, const d3d9::color color, const float thickness) {
    add_point(point1);
    add_point(point2);
    add_point(point3);

    draw_path(color, thickness);
}

void renderer::drawlist::add_rect(const s_rect& rect, const d3d9::color color, const float thickness) {
    add_point({rect.x, rect.y});
    add_point({rect.x + rect.w, rect.y});
    add_point({rect.x + rect.w + 0.5f, rect.y + rect.h});
    add_point({rect.x, rect.y + rect.h});

    draw_path(color, thickness);
}

void renderer::drawlist::push_draw_cmd() {
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
        // if the number of unaccounted indices is less than the maximum number of indices that can be hold by d3d9::index
        if (num_unaccounted_indices < UINT16_MAX) {
            // add draw command
            m_draw_cmds.buffer[m_draw_cmds.size].m_num_vertices = m_vertices.size - num_accounted_vertices;
            m_draw_cmds.buffer[m_draw_cmds.size].m_num_indices = m_indices.size - num_accounted_indices;

            m_draw_cmds.size++;

            return;
        } else {
            size_t num_indices = UINT16_MAX;
            d3d9::index last_index = m_indices.buffer[num_indices - 1];

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

void renderer::drawlist::render(d3d9::object* obj) {
    // if we haven't created our vertex buffer yet or it's not big enough
    if (!obj->m_vb || obj->m_vb_size < m_vertices.size) {
        // Delete it
        if (obj->m_vb) {
            obj->m_vb->Release();
            obj->m_vb = 0;
        }

    #ifdef LOG_MEM_CONSUMPTION
        std::printf("creating a new vertex buffer. old capacity: %ub, new capacity: %ub.\n", obj->m_vb_size * sizeof(d3d9::vertex), (m_vertices.size + INITIAL_VERTEX_BUFFER_SIZE) * sizeof(d3d9::vertex));
    #endif

        // create the new buffer
        obj->m_vb_size = m_vertices.size + INITIAL_VERTEX_BUFFER_SIZE;
        if (obj->m_device->CreateVertexBuffer(obj->m_vb_size * sizeof(d3d9::vertex), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &obj->m_vb, NULL) < 0)
            return;
    }

    // if we haven't created our index buffer yet or it's not big enough
    if (!obj->m_ib || obj->m_ib_size < m_indices.size) {
        // Delete it
        if (obj->m_ib) {
            obj->m_ib->Release();
            obj->m_ib = 0;
        }

    #ifdef LOG_MEM_CONSUMPTION
        std::printf("creating a new index buffer. old capacity: %ub, new capacity: %ub.\n", obj->m_ib_size * sizeof(d3d9::index), (m_indices.size + INITIAL_INDEX_BUFFER_SIZE) * sizeof(d3d9::index));
    #endif

        // create the new buffer
        obj->m_ib_size = m_indices.size + INITIAL_INDEX_BUFFER_SIZE;
        if (obj->m_device->CreateIndexBuffer(obj->m_ib_size * sizeof(d3d9::index), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &obj->m_ib, NULL) < 0)
            return;
    }

    {
        // copy vertices to gpu
        d3d9::vertex* new_vertices = nullptr;
        if (obj->m_vb->Lock(0, m_vertices.size * sizeof(d3d9::vertex), reinterpret_cast<void**>(&new_vertices), D3DLOCK_DISCARD) < 0)
            return;

        std::memcpy(new_vertices, m_vertices.buffer, m_vertices.size * sizeof(d3d9::vertex));

        obj->m_vb->Unlock();
    }

    {
        // copy indices to the gpu
        d3d9::index* new_indices = nullptr;
        if (obj->m_ib->Lock(0, m_indices.size * sizeof(d3d9::index), reinterpret_cast<void**>(&new_indices), D3DLOCK_DISCARD) < 0)
            return;

        std::memcpy(new_indices, m_indices.buffer, m_indices.size * sizeof(d3d9::index));

        obj->m_ib->Unlock();
    }

    obj->set_render_states();

    size_t index_buffer_offset = 0;
    for (size_t i = 0; i < m_draw_cmds.size; i++) {
        const auto& cmd = m_draw_cmds.buffer[i];

        // render
        obj->m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, cmd.m_num_indices, index_buffer_offset, cmd.m_num_indices / 3);

        // increase index for the next primitive
        index_buffer_offset += cmd.m_num_indices;
    }
}