#pragma once
#include "DataStructs.h"

#include "glm/vec3.hpp" // glm::vec3
#include "glm/vec4.hpp" // glm::vec4
#include "glm/mat4x4.hpp" // glm::mat4
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/vector_float2.hpp> // vec2
#include <glm/ext/vector_float3.hpp> // vec3
#include <glm/ext/matrix_float4x4.hpp> 
class RenderCamera
{
public:
    RenderCamera(unsigned int iWidth, unsigned int iHeight);
    ~RenderCamera();

    void SetExtents(const GeometryExtents& extents);
    float* GetViewMatrix();
    float* GetProjectionMatrix();

    void UpdateViewSize(unsigned int iWidth, unsigned int iHeight);

    void StartNavigation(int mouseX, int mouseY);
    void Rotate(int mouseX, int mouseY);
    void Pan(int mouseX, int mouseY);
    void Zoom(float fDelta);
    void UpdateViewMatrix();

    void FitView();
private:
    GeometryExtents mGeomExtents;
    float m_fModelLength;
    unsigned int m_iWidth;
    unsigned int m_iHeight;
    float m_aViewMatrix[16];
    float m_aProjectionMatrix[16];
    float                   m_aNavigationRotXY[2];
    float                   m_aPanTrans[2];
    float                   m_fZoom;
    int                     m_iPrevMouseX;
    int                     m_iPrevMouseY;
    bool                    m_bUpdateView;

    float                   mAnchorPt[3];
    float                   mPositon[3];
    glm::mat4               mViewMat;
};