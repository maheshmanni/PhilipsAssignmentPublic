#include "GLEbo.h"
#include <GL/glew.h>
GLEbo::GLEbo()
{
    m_iEBO = 0;
}

GLEbo::~GLEbo()
{
    glDeleteBuffers(1, &m_iEBO);
}

void GLEbo::CreateBuffer(const std::vector<unsigned int>& aIndices)
{
    m_iIndicesCount = (unsigned int)aIndices.size();
    if (m_iIndicesCount == 0)
        return;
    glGenBuffers(1, &m_iEBO);
    Bind();
    unsigned int iSize = m_iIndicesCount * sizeof(unsigned int);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, iSize, aIndices.data(), GL_STATIC_DRAW);
}

void GLEbo::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iEBO);
}

void GLEbo::UnBind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLEbo::DrawElements()
{
    glDrawElements(GL_TRIANGLES, m_iIndicesCount, GL_UNSIGNED_INT, (void*)0);
}