#pragma once
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "DataStructs.h"

class RenderCamera;
class GLVbo;
class GLEbo;
class OpenGLShaderManager;
struct GeomteryInfo;
struct VAO;
struct PartMaterialStruct;
class ModelLoader
{
public:
    ModelLoader(unsigned int iWidth, unsigned int iHeight);
    ~ModelLoader();

    void LoadModel(const std::string& filePath);

    void RenderModel();

    inline GeometryExtents GetExtents() const
    {
        return mExtents;
    }

    void UpdateSize(unsigned int iWidth, unsigned int iHeight);
    void Rotate(double xpos, double ypos);
    void StartNavigation(double xpos, double ypos);
    void Pan(double xpos, double ypos);
    void Zoom(float delta);
protected:
    void CreateRenderObj(GeomteryInfo* pGeometry, const std::vector<unsigned int>& aIndices, PartMaterialStruct* pMat);
    void FetchNodeData(aiNode* pNode, const aiScene* pScene);
    void FetchMeshData(aiMesh* pMesh, const aiScene* pScene);
    void BindTexture(VAO* pvao);
    void ExtractDirectoryFromFilePath(const std::string sFilePath);
private:
    OpenGLShaderManager* m_pGLShaderMngr;
    std::vector<GLVbo*> m_aVBO;
    std::vector<GLEbo*> m_aEBO;
    std::vector<VAO*> m_aVAO;
    GeometryExtents mExtents;

    unsigned int m_iWidth;
    unsigned int m_iHeight;
    RenderCamera* m_pCamera;
    std::string m_sFileDirectory;
};