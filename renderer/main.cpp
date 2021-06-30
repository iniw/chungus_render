#include <Windows.h>
#include <iostream>
#include "renderer/renderer.h"

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

int main() {
    WNDCLASSEXW wcx;
    memset(&wcx, 0, sizeof(WNDCLASSEXW));
    wcx.cbSize = sizeof(WNDCLASSEXW);
    wcx.lpfnWndProc = WindowProc;
    wcx.lpszClassName = TEXT("Chungus Renderer");

    if (!RegisterClassExW(&wcx))
        return -1;

    constexpr renderer::s_vec2 display_size = {640.f, 480.f};

    HWND window;
    if (!(window = CreateWindowExW(0, TEXT("Chungus Renderer"), TEXT("Chungus Renderer"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, display_size.x, display_size.y, NULL, NULL, NULL, NULL)))
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
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3d9_device) < 0)
        return false;

    renderer::init(d3d9_device, display_size);

    MSG msg;
    while (true) {
        while (PeekMessageW(&msg, NULL, NULL, NULL, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                return 0;
        }

        renderer::start();
       
        renderer::rect({100.f, 100.f, 250.f, 250.f}, {255, 0, 0, 255});

        renderer::triangle({200.f, 200.f}, {250.f, 150.f}, {300.f, 200.f}, {255, 255, 0, 255});

        renderer::draw_list.add_line({ 50.f, 50.f }, { 75.f, 50.f }, { 255, 255, 255, 255 });
        renderer::draw_list.add_line({ 60.f, 50.f }, { 100.f, 75.f }, { 255, 255, 255, 255 });

        renderer::end();


        d3d9_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 255), 0, 0);
        d3d9_device->BeginScene();
        renderer::render();
        d3d9_device->EndScene();
        HRESULT result = d3d9_device->Present(NULL, NULL, NULL, NULL);
    }
}