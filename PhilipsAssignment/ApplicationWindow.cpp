#include "ApplicationWindow.h"
#include <iostream>
#include "GL/glew.h"

//#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "ModelLoader.h"

ApplicationWindow::ApplicationWindow(const char* sFilePath)
{
    m_pWindow = nullptr;
    m_iWidth = m_iHeight = 512;
    m_iMouseLClickStatus = 0;
    InitWindow();
    m_pModelLoader = new ModelLoader(m_iWidth, m_iHeight);
    m_pModelLoader->LoadModel(sFilePath);
}


ApplicationWindow::~ApplicationWindow()
{
    delete m_pModelLoader;
    glfwTerminate();
}

void ApplicationWindow::InitWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(m_iWidth, m_iHeight, "Assignment", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glewInit();
    m_pWindow = window;
    glEnable(GL_DEPTH_TEST);

}

void ApplicationWindow::RenderLoop()
{
    if (!m_pWindow)
        return;
    while (!glfwWindowShouldClose((GLFWwindow*)m_pWindow))
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_pModelLoader->RenderModel();
        glfwSwapBuffers((GLFWwindow*)m_pWindow);
        glfwPollEvents();
    }
}

void ApplicationWindow::RegisterFrameBufferResize(void(*FunPtr)(GLFWwindow* window, int iWidth, int iHeight))
{
    glfwSetFramebufferSizeCallback((GLFWwindow*)m_pWindow, FunPtr);
}

void ApplicationWindow::RegisterMouseCallback(void(*FunPtr)(GLFWwindow* window, double xpos, double ypos))
{
    glfwSetCursorPosCallback((GLFWwindow*)m_pWindow, FunPtr);
}

void ApplicationWindow::RegisterMouseBtnCB(void(*FunPtr)(GLFWwindow* window, int x1, int x2, int x3))
{
    glfwSetMouseButtonCallback((GLFWwindow*)m_pWindow, FunPtr);
}

void ApplicationWindow::UpdateSize(int iWidth, int iHeight)
{
    glViewport(0, 0, iWidth, iHeight);
    if (!m_pModelLoader)
        return;
    m_pModelLoader->UpdateSize(iWidth, iHeight);
}

void ApplicationWindow::MouseMove(double xpos, double ypos)
{
    if (!m_pModelLoader)
        return;
    if (m_bMouseMoveFirst)
    {
        m_pModelLoader->StartNavigation(xpos, ypos);
    }
    if (m_iMouseLClickStatus == 1)
    {
      
        m_pModelLoader->Rotate(xpos, ypos);
    }
    else if (m_iMouseRClickStatus == 1)
    {
        m_pModelLoader->Pan(xpos, ypos);
    }
    m_bMouseMoveFirst = false;
}

void ApplicationWindow::SetMouseClickStatus(int btn, int iClick)
{
    switch (btn)
    {
    case 0:
    {
        m_iMouseLClickStatus = iClick;
    }
        break;
    case 1:
        m_iMouseRClickStatus = iClick;
        break;
    default:
        break;
    }
    m_bMouseMoveFirst = true;

}

void ApplicationWindow::RegisterScrollCallback(void(*FunPtr)(GLFWwindow* window, double xoffset, double yoffset))
{
    glfwSetScrollCallback((GLFWwindow*)m_pWindow, FunPtr);
}


void ApplicationWindow::Scroll(double offset)
{
    m_pModelLoader->Zoom(offset * 10.0f);
}