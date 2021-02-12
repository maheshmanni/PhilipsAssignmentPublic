#pragma once
#include <vector>
class GLEbo
{
public:
    GLEbo();
    ~GLEbo();
    void CreateBuffer(const std::vector<unsigned int>& aIndices);
    void Bind();
    void UnBind();
    void DrawElements();
private:
    unsigned int m_iEBO;
    unsigned int m_iIndicesCount;
};