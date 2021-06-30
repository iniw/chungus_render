#include <Windows.h>
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

    renderer::s_vec2 display_size = {640.f, 480.f};

    if (!RegisterClassExW(&wcx))
        return -1;

    HWND hWnd;
    if (!(hWnd = CreateWindowExW(0, TEXT("Chungus Renderer"), TEXT("Chungus Renderer"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, display_size.x, display_size.y, NULL, NULL, NULL, NULL)))
        return -1;

    ShowWindow(hWnd, SW_SHOW);

    LPDIRECT3D9 d3d9;
    LPDIRECT3DDEVICE9 d3dDevice;
    if ((d3d9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return -1;

    D3DPRESENT_PARAMETERS d3dpp;
    memset(&d3dpp, 0, sizeof(D3DPRESENT_PARAMETERS));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3dDevice) < 0)
        return false;

    renderer::init(d3dDevice, std::move(display_size));

    MSG msg;
    while (true) {
        while (PeekMessageW(&msg, NULL, NULL, NULL, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
                return 0;
        }

        renderer::start();

        renderer::rect({100.f, 100.f, 200.f, 200.f}, {255, 0, 0, 255});

        renderer::triangle({200.f, 200.f}, {250.f, 150.f}, {300.f, 200.f}, {255, 255, 0, 255});

        renderer::end();


        d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 255), 0, 0);
        d3dDevice->BeginScene();
        renderer::render();
        d3dDevice->EndScene();
        HRESULT result = d3dDevice->Present(NULL, NULL, NULL, NULL);
    }
}