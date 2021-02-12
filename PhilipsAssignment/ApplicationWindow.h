#pragma once
#include <string>
class ModelLoader;
typedef struct GLFWwindow GLFWwindow;
class ApplicationWindow
{
public:
    ApplicationWindow(const char* sFilePath);
    ~ApplicationWindow();


    void RenderLoop();

    void RegisterFrameBufferResize(void(*FunPtr)(GLFWwindow* window, int iWidth, int iHeight));

    void RegisterMouseCallback(void(*FunPtr)(GLFWwindow* window, double xpos, double ypos));

    void RegisterMouseBtnCB(void(*FunPtr)(GLFWwindow* window, int x1, int x2, int x3));

    void RegisterScrollCallback(void(*FunPtr)(GLFWwindow* window, double xoffset, double yoffset));
  

    void UpdateSize(int iWidth, int iHeight);

    void MouseMove(double xpos, double ypos);
    //1 clicked
    //0 Released
    //btn
    //1 for right
    //0 for left
    void SetMouseClickStatus(int btn, int iClick);

    void Scroll(double offset);
protected:
    void InitWindow();

private:
    void* m_pWindow;
    unsigned int m_iWidth;
    unsigned int m_iHeight;
    ModelLoader* m_pModelLoader;
    int m_iMouseLClickStatus;
    bool m_bMouseMoveFirst;
    int m_iMouseRClickStatus;
};