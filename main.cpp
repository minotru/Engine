#include <windows.h>
#include <windef.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "Engine.h"


using namespace std;
using namespace Engine;


LRESULT CALLBACK WndProc(HWND, UINT,WPARAM, LPARAM);
void InitInstance(HINSTANCE, TCHAR CLASS_NAME[]);
 
const int UPDATE_TIME = CLOCKS_PER_SEC / 12; 

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
    TCHAR CLASS_NAME[] = "up5";
    HWND hWnd;
    MSG msg;
    Engine::Scene scene;
    
    InitInstance(hInstance, CLASS_NAME);
    hWnd = CreateWindow(CLASS_NAME, "Caption",
                       WS_OVERLAPPEDWINDOW, 
                       CW_USEDEFAULT, CW_USEDEFAULT, 
                       CW_USEDEFAULT, CW_USEDEFAULT,
                       NULL, NULL, hInstance, NULL);
    SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)(&scene));
    
    //Engine::Polygon sword;
    //sword.drawStyle.setPen(PS_SOLID, 3, RGB(255, 0, 0));
    /*sword.addVertex(10, 10).addVertex(20, 0).addVertex(30, 10)
       .addVertex(30, 60).addVertex(40, 60).addVertex(40, 70)
       .addVertex(30, 70).addVertex(30, 90).addVertex(10, 90)
       .addVertex(10, 70).addVertex(0, 70).addVertex(0, 60)
       .addVertex(10, 60);*/
    //sword.addVertex(Point(0, 0)).addVertex(Point(0, 3)).addVertex(Point(3, 3)).addVertex(Point(3, 0));
    //sword.transform.translate(Vector(20, 90), Space::SELF);
    //sword.transform.rotate(180, Space::SELF);
    //sword.transform.translate(Vector(50, 0), Space::)
    Engine::Polygon p1, p2;
    p1.addVertex(Point(0, 0)).addVertex(Point(8, 8)).addVertex(Point(16, 0));
    p1.drawStyle.setBrush(BS_SOLID, RGB(255, 0, 0));
    p2.addVertex(Point(0, 0)).addVertex(Point(8, 8)).addVertex(Point(16, 0));
    p2.drawStyle.setBrush(BS_SOLID, RGB(0, 255, 0));
    ComplexObject complex;
    complex.addObject(&p1);
    complex.addObject(&p2);
    //Engine::Ellipse ellipse(Point(50, 50), 3, 3);
    //ellipse.drawStyle.setBrush(BS_SOLID, RGB(0, 255, 0));
    //complex.addObject(&sword);
    scene.addObject(&complex);
    
    Engine::Line ox(Point(-1000, 0), Point(1000, 0)), 
    oy(Point(0, -1000), Point(0, 1000));
    ox.drawStyle.setPen(PS_SOLID, 3, RGB(0, 0, 0));
    oy.drawStyle.setPen(PS_SOLID, 3, RGB(0, 0, 0));
    scene.addObject(&ox);
    scene.addObject(&oy);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

   
    bool isRunning = true;
    clock_t nowTime, prevTime;
    prevTime = clock();

    RECT size;
    int h, w;
    GetClientRect(hWnd, &size);
    h = size.bottom - size.top;
    w = size.right - size.left;
    bool pause = false;
    //complex.transform.rotate(45);
    //complex.transform.translate(Vector(50, 0));
    p1.transform.translate(Vector(100, 0));
    p2.transform.translate(Vector(-100, 0));
    while (isRunning)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                isRunning = false;
            TranslateMessage(&msg);
            if (msg.message == WM_KEYDOWN)
                pause = !pause;
            DispatchMessage(&msg);
        }
        if (!pause)
        {
            nowTime = clock();
            if (nowTime - prevTime >= UPDATE_TIME)
            { 
                prevTime = nowTime;
                complex.transform.translate(Vector(5, 0), Space::WORLD);
                complex.transform.rotate(5, Space::SELF);
                p1.transform.translate(Vector(-5, 0));
                p2.transform.translate(Vector(5, 0));
                //complex.transform.rotate(5, Space::WORLD);
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static Engine::Scene *scene;
    static int cx, cy;
    HDC hdc;
    PAINTSTRUCT ps;
        
    switch (msg)
    {
    case WM_SHOWWINDOW:
        scene = (Engine::Scene*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        SetMapMode(hdc, MM_ISOTROPIC);
        SetGraphicsMode(hdc, GM_ADVANCED);
        SetViewportOrgEx(hdc, cx / 2, cy / 2, NULL);
        SetViewportExtEx(hdc, cx, cy, NULL);
        SetWindowExtEx(hdc, cx, -cy, NULL);
        scene->draw(hdc);
        EndPaint(hWnd, &ps);
        return 0;
    case WM_SIZE:
        cx = LOWORD(lParam);
        cy = HIWORD(lParam);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void InitInstance(HINSTANCE hInstance, TCHAR CLASS_NAME[])
{
    WNDCLASS wc;
    wc = {0};
    wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    wc.hCursor = (HCURSOR) GetStockObject((int)IDC_ARROW);
    wc.hIcon = (HICON) GetStockObject((int)IDI_APPLICATION);
    wc.hInstance = hInstance;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbWndExtra = sizeof(void*);
    RegisterClass(&wc);
}