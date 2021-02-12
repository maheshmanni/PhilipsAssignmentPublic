#include "RenderCamera.h"


#define MAX(a,b) ((a) > (b) ? (a) : (b))

RenderCamera::RenderCamera(unsigned int iWidth, unsigned int iHeight)
{
    m_fModelLength = .0f;
    m_aNavigationRotXY[0] = m_aNavigationRotXY[1] = .0f;
    m_aPanTrans[0] = m_aPanTrans[1] = .0f;
    m_fZoom = .0f;
    m_bUpdateView = true;
    UpdateViewSize(iWidth, iHeight);
    mAnchorPt[0] = mAnchorPt[1] = mAnchorPt[2] = 0.0f;
}

RenderCamera::~RenderCamera()
{

}

void RenderCamera::SetExtents(const GeometryExtents& extents)
{
    mGeomExtents = extents;
    float diffX = mGeomExtents.aMax[0] - mGeomExtents.aMin[0];
    float diffY = mGeomExtents.aMax[1] - mGeomExtents.aMin[1];
    float diffZ = mGeomExtents.aMax[2] - mGeomExtents.aMin[2];
    m_fModelLength = sqrtf(pow(diffX, 2.0f) + pow(diffY, 2.0f) + pow(diffZ, 2.0f));

    for (int i = 0; i < 3; ++i)
        mAnchorPt[i] = (mGeomExtents.aMax[i] + mGeomExtents.aMin[2]) / 2;

    FitView();
}

float* RenderCamera::GetViewMatrix()
{
    if (m_bUpdateView)
        UpdateViewMatrix();
    return m_aViewMatrix;
}

float* RenderCamera::GetProjectionMatrix()
{
    return m_aProjectionMatrix;
}

void RenderCamera::UpdateViewSize(unsigned int iWidth, unsigned int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)m_iWidth / m_iHeight, 0.1f, 5000.0f);
    for (int i = 0; i < 16; ++i)
        m_aProjectionMatrix[i] = glm::value_ptr(Projection)[i];
}


void RenderCamera::StartNavigation(int mouseX, int mouseY)
{
    m_iPrevMouseX = mouseX;
    m_iPrevMouseY = mouseY;
}

void RenderCamera::Rotate(int mouseX, int mouseY)
{
    int diffx = mouseX - m_iPrevMouseX;
    int diffy = mouseY - m_iPrevMouseY;
    m_iPrevMouseX = mouseX;
    m_iPrevMouseY = mouseY;

    m_aNavigationRotXY[0] += (float) 0.5f * diffy;
    m_aNavigationRotXY[1] += (float) 0.5f * diffx;
    m_bUpdateView = true;
}

void RenderCamera::Pan(int mouseX, int mouseY)
{
    int diffx = mouseX - m_iPrevMouseX;
    int diffy = mouseY - m_iPrevMouseY;
    m_iPrevMouseX = mouseX;
    m_iPrevMouseY = mouseY;
    float fSensitivity = (m_fModelLength < .01f) ? .05f : (m_fModelLength / 100.0f);
    m_aPanTrans[0] += (float)fSensitivity * diffx;
    m_aPanTrans[1] -= (float)fSensitivity * diffy;
    m_bUpdateView = true;
}

void RenderCamera::Zoom(float fDelta)
{
    float fSensitivity = (m_fModelLength < .01f) ? .002f : (m_fModelLength / 1000.0f);
    m_fZoom += fDelta * fSensitivity;
    m_bUpdateView = true;
}


void RenderCamera::UpdateViewMatrix()
{
    m_bUpdateView = false;

    glm::mat4 viewMat(1.0f);
    viewMat = glm::translate(viewMat, glm::vec3(0.0f, 0.0f, -m_fZoom));
    viewMat = glm::translate(viewMat, glm::vec3(m_aPanTrans[0], m_aPanTrans[1], .0f));


  //  viewMat = glm::translate(viewMat, glm::vec3(-mAnchorPt[0], -mAnchorPt[1], -mAnchorPt[2]));
    viewMat = glm::rotate(viewMat, glm::radians(m_aNavigationRotXY[0]), glm::vec3(1.0f, .0f, .0f));
    viewMat = glm::rotate(viewMat, glm::radians(m_aNavigationRotXY[1]), glm::vec3(.0f, 1.0f, .0f));
  //  viewMat = glm::translate(viewMat, glm::vec3(mAnchorPt[0], mAnchorPt[1], mAnchorPt[2]));

    viewMat = mViewMat * viewMat;
    for (int i = 0; i < 16; ++i)
        m_aViewMatrix[i] = glm::value_ptr(viewMat)[i];
}

void RenderCamera::FitView()
{
    float center[] = { (mGeomExtents.aMax[0] + mGeomExtents.aMin[0]) / 2.0f, (mGeomExtents.aMin[1] + mGeomExtents.aMax[1]) / 2.0f, (mGeomExtents.aMax[2] + mGeomExtents.aMin[2]) / 2.0f };

    float zdistW = ((mGeomExtents.aMax[0] - mGeomExtents.aMin[0]) / 2.f) / tan(1.57 / 2.f);
    float zdistH = ((mGeomExtents.aMax[1] - mGeomExtents.aMin[1]) / 2.f) / tan(1.57 / 2.f);
    float zdist = MAX(zdistW, zdistH);


    mPositon[0] = center[0];
    mPositon[1] = center[1];
    mPositon[2] = mGeomExtents.aMax[2] - zdist;

    mViewMat = glm::lookAt(glm::vec3(mPositon[0], mPositon[1], mPositon[2]), glm::vec3(center[0], center[1], center[2]),
        glm::vec3(.0f, 1.0f, .0f));

    for (int i = 0; i < 16; ++i)
        m_aViewMatrix[i] = glm::value_ptr(mViewMat)[i];
}