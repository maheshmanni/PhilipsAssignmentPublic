#pragma once
#include <vector>
struct GeomteryInfo
{
    std::vector<float> m_aVertices;
    std::vector<float> m_aNormals;
    std::vector<float> m_aTexCoords;
};
class GLVbo
{
public:
    GLVbo();
    ~GLVbo();

    void CreateVBO(GeomteryInfo* pGeometryInfo);

    void Bind();
    void UnBind();
private:
    unsigned int m_iVBO;
};