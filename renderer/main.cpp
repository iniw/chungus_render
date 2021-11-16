#include <Windows.h>
#include <iostream>
#include <chrono>
#pragma comment (lib, "d3dx9")
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include "render/render.h"
#include "render/variadic_vector.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

using vec_2 = render::point;

constexpr vec_2 display_size = { 1280.f, 720.f };

class c_engine {
public:
    static constexpr vec_2 axis = { 250.f, 600.f };
    static constexpr auto init_dist = 200.f;
    static constexpr auto block_dist = 100.f;

    c_engine(const vec_2& d1, const vec_2& d2, const double& m1, const double& m2, const double& v1, const double& v2)
        : m1(m1), m2(m2), v1(v1), v2(v2), collisions(0), frametime(0.f)
    {
        big_rect = math::v4{ axis[X] + init_dist + block_dist, axis[Y] - d1[Y], d1[X], d1[Y] };
        small_rect = math::v4{ axis[X] + init_dist, axis[Y] - d2[Y], d2[X], d2[Y] };
    }

    void start() {
        t1 = std::chrono::high_resolution_clock::now();
    }
    
    void end() {
        update();
    }

    void run() {
        g_render.line(axis, { axis[X], 0.f }, { 255, 255, 255, 255 });
        g_render.line(axis, { display_size[X], axis[Y] }, { 255, 255, 255, 255 });

        // hate this
        auto big_rect_pos = big_rect[X];
        if (big_rect_pos < small_rect[X] + small_rect[X2])
            big_rect_pos = small_rect[X] + small_rect[X2];

        g_render.rectangle_filled({ big_rect_pos, big_rect[Y] }, { big_rect[X2], big_rect[Y2] }, { 255, 0, 0, 255 });
        g_render.rectangle_filled({ small_rect[X], small_rect[Y] }, { small_rect[X2], small_rect[Y2] }, { 0, 0, 255, 255 });

        g_render.rectangle({ big_rect_pos, big_rect[Y]  }, { big_rect[X2], big_rect[Y2] }, { 255, 255, 255, 255 });
        g_render.rectangle({ small_rect[X], small_rect[Y] }, { small_rect[X2], small_rect[Y2] }, { 255, 255, 255, 255 });

        g_render.text("Collisions: " + std::to_string(collisions), { 800, 300 }, { 255, 255, 255, 255 });
        g_render.text("C H U N G U S", { 100, 200 }, { 255, 255, 255, 255 });
    }

private:
    void update() {
        t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> ms = t2 - t1;
        frametime = ms.count() * 0.1f;

        big_rect[X] += v1 * frametime;
        small_rect[X] += v2 * frametime;

        if (small_rect[X] + small_rect[X2] > big_rect[X]) {
            on_block_collision();
            small_rect[X] = big_rect[X] - small_rect[X2];
        }

        if (small_rect[X] < axis[X]) {
            on_wall_collision();
            small_rect[X] = axis[X];
        }
    }

    void on_block_collision() {
        auto v1_p = ((m1 - m2) * v1 + (2 * m2) * v2) / (m1 + m2);
        auto v2_p = ((2 * m1) * v1 - (m1 - m2) * v2) / (m1 + m2);

        v1 = v1_p;
        v2 = v2_p;

        collisions++;
    }

    void on_wall_collision() {
        v2 *= -1.0;

        collisions++;
    }

private:
    int collisions;

    render::rect big_rect;
    render::rect small_rect;
    
    // mass, vel
    float m1, v1;
    float m2, v2;

    float frametime;
    std::chrono::steady_clock::time_point t1, t2;
};

int main() {
    WNDCLASSEXW wcx;
    memset(&wcx, 0, sizeof(WNDCLASSEXW));
    wcx.cbSize = sizeof(WNDCLASSEXW);
    wcx.lpfnWndProc = WindowProc;
    wcx.lpszClassName = TEXT("Chungus Renderer");

    if (!RegisterClassExW(&wcx))
        return -1;

    HWND window;
    if (!(window = CreateWindowExW(0, TEXT("Chungus Renderer"), TEXT("Chungus Renderer"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, display_size[X], display_size[Y], NULL, NULL, NULL, NULL)))
        return -1;

    ShowWindow(window, SW_SHOW);

    LPDIRECT3D9 d3d9;
    LPDIRECT3DDEVICE9 d3d9_device;
    if ((d3d9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return -1;

    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3d9_device) < 0)
        return false;

    g_render.init(d3d9_device);

    c_engine engine({ 100.f, 100.f }, { 50.f, 50.f }, pow(100.0, 4), 1, -1.0, 0.0);
    
    MSG msg;
    while (true) {
        while (PeekMessageW(&msg, NULL, NULL, NULL, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                return 0;
        }

        engine.start();
        {
            d3d9_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 255), 0, 0);
            d3d9_device->BeginScene();

            g_render.start();
            {
                engine.run();
            }
            g_render.end();

            d3d9_device->EndScene();
            HRESULT result = d3d9_device->Present(NULL, NULL, NULL, NULL);
        }
        engine.end();
    }
}