#include <iostream>
#include "ApplicationWindow.h"

ApplicationWindow* pAppWindow = nullptr;

void FrameBufferSizeCallback(GLFWwindow* pWindow, int iWidth, int iHeight)
{
    pAppWindow->UpdateSize(iWidth, iHeight);
}

void mbtncb(GLFWwindow*, int x1, int x2, int x3)
{
    pAppWindow->SetMouseClickStatus(x1, x2);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    pAppWindow->MouseMove(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    pAppWindow->Scroll(yoffset);
}

int main(int argc, char* argv[])
{
    argc = 2;
    if (argc < 2)
    {
        std::cout << "send .obj file path in the argument list\n";
        return 0;
    }
    pAppWindow = new ApplicationWindow(argv[1]);
    pAppWindow->RegisterFrameBufferResize(FrameBufferSizeCallback);
    pAppWindow->RegisterMouseBtnCB(mbtncb);
    pAppWindow->RegisterMouseCallback(mouse_callback);
    pAppWindow->RegisterScrollCallback(scroll_callback);
    pAppWindow->RenderLoop();
    return 1;
}