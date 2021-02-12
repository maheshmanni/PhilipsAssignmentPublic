#include "GLVbo.h"
#include <GL/glew.h>

GLVbo::GLVbo()
{
    m_iVBO = 0;
}

GLVbo::~GLVbo()
{
    glDeleteBuffers(1, &m_iVBO);
}

void GLVbo::CreateVBO(GeomteryInfo* pGeometryInfo)
{
    if (!pGeometryInfo)
        return;
    glGenBuffers(1, &m_iVBO);
    Bind();
    unsigned int iVertexSize = (unsigned int)(pGeometryInfo->m_aVertices.size()) * sizeof(float);
    unsigned int iNormalSize = (unsigned int)(pGeometryInfo->m_aNormals.size()) * sizeof(float);
    unsigned int iTexSize = (unsigned int)(pGeometryInfo->m_aTexCoords.size()) * sizeof(float);
    unsigned int iTotalBufferSize = iVertexSize + iNormalSize + iTexSize;
    glBufferData(GL_ARRAY_BUFFER, iTotalBufferSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, iVertexSize, pGeometryInfo->m_aVertices.data());
    glBufferSubData(GL_ARRAY_BUFFER, iVertexSize, iNormalSize, pGeometryInfo->m_aNormals.data());
    glBufferSubData(GL_ARRAY_BUFFER, iVertexSize + iNormalSize, iTexSize, pGeometryInfo->m_aTexCoords.data());

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(iVertexSize));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(iVertexSize + iNormalSize));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
}

void GLVbo::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_iVBO);
}

void GLVbo::UnBind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
