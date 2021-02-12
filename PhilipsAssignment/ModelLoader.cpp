#include "ModelLoader.h"
#include <iostream>

#include <queue>
#include "OpenGLShaderManager.h"
#include "GL/glew.h"
#include "GLVbo.h"
#include "GLEbo.h"
#include "RenderCamera.h"
#include "Texture.h"

std::string VS = R"(
#version 330 core
#define LAYOUT_LOCATION(x) layout(location = x)

LAYOUT_LOCATION(0) in vec3 vertex;
LAYOUT_LOCATION(1) in vec3 normal;
LAYOUT_LOCATION(2) in vec2 texCoords;
uniform mat4 proj, view;
out vec3 ecPos3;
out vec3 fragNormal;
out vec2 varyingTex;
void main()
{
vec4 ecPos = view * vec4(vertex, 1.0);
ecPos3 = vec3(ecPos);
fragNormal = normal;
varyingTex = texCoords;
gl_Position = proj * view * vec4(vertex, 1.0);
}
)";

std::string FS = R"(
#version 330 core
in vec3 ecPos3;
out vec4 FragmentColor;
in vec3 fragNormal;
in vec2 varyingTex;
uniform int hastexture;
struct Material
{
vec3 diffuse;
vec3 ambience;
vec3 specular;
};
uniform Material pMaterial;

uniform sampler2D diffuseTexture;
void main()
{
    vec4 color = vec4(1.0);
	vec3 N = fragNormal;

  vec4 light_ambdiff = vec4(0.35, 0.35, 0.35, 1.0);
        vec4	light_spec    = vec4(0.45, 0.45, 0.45, 1.0);
        vec4	light_ambient = vec4(0.5, 0.5, 0.5, 1.0);
        
        vec4 diffuse = vec4(0.0);
        vec4 specular = vec4(0.0);
        
        vec4 amb_color = vec4(0.0);
        // Compute vector from surface to light position
        vec3 VP = vec3(0.0, 0.0, -1.0) - ecPos3;
        
        VP = normalize(VP);
        
        float nDotVP = abs(dot(N, VP));

        {
            float pf = pow(max(dot(N, normalize(vec3(0.0, 1.0, 0.0) + VP)), 0.0), 45.0);
            diffuse = light_ambdiff * nDotVP;
            specular = light_spec * pf;
        }
        
        color = vec4(pMaterial.ambience,1.0) + (diffuse * vec4(pMaterial.diffuse,1.0)) + (specular * vec4(pMaterial.specular,1.0));

        N = -N;
        nDotVP = abs(dot(N, VP));
        {
            float pf = pow(max(dot(N, normalize(vec3(0.0, 1.0, 0.0) + VP)), 0.0), 45.0);
            diffuse = light_ambdiff * nDotVP;
            specular = light_spec * pf;
        }
        color += (diffuse * vec4(pMaterial.diffuse,1.0)) + (specular * vec4(pMaterial.specular,1.0));
    if(hastexture == 1)
    {
        color =  color * texture(diffuseTexture, varyingTex);
    }
       FragmentColor = color;
}
)";

struct PartMaterialStruct
{
    PartMaterialStruct()
    {
        mDiffuseTexture = "";
    }
    std::vector<float> aDiffuse;
    std::vector<float> aAmbience;
    std::vector<float> aSpecular;
    std::string mDiffuseTexture;
};
struct VAO
{
    VAO()
    {
        m_iVAO = 0;
        m_iIndexCount = 0;
        m_iTexture = 0;
    }
    ~VAO()
    {
        glDeleteVertexArrays(1, &m_iVAO);
    }
    unsigned int m_iVAO;
    unsigned int m_iIndexCount;
    unsigned int m_iTexture;
    PartMaterialStruct* m_pMaterial;
};


ModelLoader::ModelLoader(unsigned int iWidth, unsigned int iHeight)
    :m_iWidth(iWidth),
    m_iHeight(iHeight)
{
    m_pGLShaderMngr = nullptr;
    m_pCamera = nullptr;
}

ModelLoader::~ModelLoader()
{
    for (VAO* pvao : m_aVAO)
        delete pvao;
}

void ModelLoader::LoadModel(const std::string& filePath)
{
    ExtractDirectoryFromFilePath(filePath);
    mExtents.reset();
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "Model Load Error " << importer.GetErrorString() << std::endl;
        return;
    }

    //doing a DFS to traverse all the nodes
    std::queue<aiNode*> qNodes;
    qNodes.push(scene->mRootNode);
    while (!qNodes.empty())
    {
        aiNode* pNode = qNodes.front();
        FetchNodeData(pNode, scene);
        qNodes.pop();
        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            qNodes.push(pNode->mChildren[i]);
        }
    }
}

void ModelLoader::ExtractDirectoryFromFilePath(const std::string sFilePath)
{
    size_t found;
    found = sFilePath.find_last_of("/\\");
    m_sFileDirectory = sFilePath.substr(0, found);
}

void ModelLoader::RenderModel()
{
    if (!m_pGLShaderMngr)
        return;
    if (!m_pCamera)
    {
        m_pCamera = new RenderCamera(m_iWidth, m_iHeight);
        m_pCamera->SetExtents(mExtents);
    }
    m_pGLShaderMngr->Bind();
    m_pGLShaderMngr->SetMat4("view", m_pCamera->GetViewMatrix());
    m_pGLShaderMngr->SetMat4("proj", m_pCamera->GetProjectionMatrix());
    for (VAO* pvao : m_aVAO)
    {
        BindTexture(pvao);
        m_pGLShaderMngr->SetFloat3("pMaterial.diffuse", pvao->m_pMaterial->aDiffuse.data());
        m_pGLShaderMngr->SetFloat3("pMaterial.ambience", pvao->m_pMaterial->aAmbience.data());
        m_pGLShaderMngr->SetFloat3("pMaterial.specular", pvao->m_pMaterial->aSpecular.data());
        glBindVertexArray(pvao->m_iVAO); 
        glDrawElements(GL_TRIANGLES, pvao->m_iIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    m_pGLShaderMngr->UnBind();
}

void ModelLoader::CreateRenderObj(GeomteryInfo* pGeometry, const std::vector<unsigned int>& aIndices, PartMaterialStruct* pMat)
{
    if (!m_pGLShaderMngr)
    {
        m_pGLShaderMngr = new OpenGLShaderManager(VS.data(), FS.data());
        std::string shaderError;
        m_pGLShaderMngr->CompileAndLinkShader(shaderError);
    }
    VAO* pvao = new VAO();
    glGenVertexArrays(1, &pvao->m_iVAO);
    glBindVertexArray(pvao->m_iVAO);

    
    GLVbo* pVBO = new GLVbo();
    pVBO->CreateVBO(pGeometry);
    delete pGeometry;
    GLEbo* pEBO = new GLEbo();
    pEBO->CreateBuffer(aIndices);

    pvao->m_iIndexCount = (unsigned int)aIndices.size();
    pvao->m_pMaterial = pMat;

    if (pMat->mDiffuseTexture.length() > 0)
    {
        Texture tex;
        pvao->m_iTexture = tex.CreateGLTexture(m_sFileDirectory + "//" + pMat->mDiffuseTexture);
    }
    m_aVAO.push_back(pvao);

    glBindVertexArray(0);
}



void ModelLoader::FetchNodeData(aiNode* pNode, const aiScene* pScene)
{
    for (unsigned int i = 0; i < pNode->mNumMeshes; i++)
    {
        std::cout << "fetching part " << i << "\n";
        aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[i]];
        FetchMeshData(mesh, pScene);
    }
}

void ModelLoader::FetchMeshData(aiMesh* pMesh, const aiScene* pScene)
{
    GeomteryInfo* partObj = new GeomteryInfo();
    for (unsigned int iVertexIndex = 0; iVertexIndex < pMesh->mNumVertices; ++iVertexIndex)
    {

        aiVector3D aCurrentVertex = pMesh->mVertices[iVertexIndex];

        partObj->m_aVertices.push_back(aCurrentVertex.x);
        partObj->m_aVertices.push_back(aCurrentVertex.y);
        partObj->m_aVertices.push_back(aCurrentVertex.z);

        float vertex[3] = { aCurrentVertex.x, aCurrentVertex.y, aCurrentVertex.z };
        mExtents.update(vertex);
        partObj->m_aNormals.push_back(pMesh->mNormals[iVertexIndex].x);
        partObj->m_aNormals.push_back(pMesh->mNormals[iVertexIndex].y);
        partObj->m_aNormals.push_back(pMesh->mNormals[iVertexIndex].z);

        if (pMesh->mTextureCoords[0])//if the mesh has texture coordinates
        {
            partObj->m_aTexCoords.push_back(pMesh->mTextureCoords[0][iVertexIndex].x);
            partObj->m_aTexCoords.push_back(pMesh->mTextureCoords[0][iVertexIndex].y);
        }
    }
    std::vector<unsigned int> aIndices;
    for (unsigned int iFaceIndex = 0; iFaceIndex < pMesh->mNumFaces; ++iFaceIndex)
    {
        aiFace face = pMesh->mFaces[iFaceIndex];
        for (unsigned int i = 0; i < face.mNumIndices; ++i)
        {
            aIndices.push_back(face.mIndices[i]);
        }
    }

    aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];
    aiColor4D color;
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    PartMaterialStruct* pMaterial = new PartMaterialStruct();
    pMaterial->aDiffuse.resize(3);
    pMaterial->aDiffuse[0] = color.r;
    pMaterial->aDiffuse[1] = color.g;
    pMaterial->aDiffuse[2] = color.b;

    pMaterial->aAmbience.resize(3);
    material->Get(AI_MATKEY_COLOR_AMBIENT, color);
    pMaterial->aAmbience[0] = color.r;
    pMaterial->aAmbience[1] = color.g;
    pMaterial->aAmbience[2] = color.b;

    pMaterial->aSpecular.resize(3);
    material->Get(AI_MATKEY_COLOR_SPECULAR, color);
    pMaterial->aSpecular[0] = color.r;
    pMaterial->aSpecular[1] = color.g;
    pMaterial->aSpecular[2] = color.b;


    for (int iTextureIndex = 0; iTextureIndex < material->GetTextureCount(aiTextureType_DIFFUSE); ++iTextureIndex)
    {
        aiString str;
        material->GetTexture(aiTextureType_DIFFUSE, iTextureIndex, &str);
        pMaterial->mDiffuseTexture = str.C_Str();
    }

    CreateRenderObj(partObj, aIndices, pMaterial);
}

void ModelLoader::BindTexture(VAO* pvao)
{
    if (pvao->m_iTexture != 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pvao->m_iTexture);
        m_pGLShaderMngr->SetInt("hastexture", 1);
        return;
    }
    m_pGLShaderMngr->SetInt("hastexture", 0);
}

void ModelLoader::UpdateSize(unsigned int iWidth, unsigned int iHeight)
{
    m_iWidth = iWidth;
    m_iHeight = iHeight;
    m_pCamera->UpdateViewSize(iWidth, iHeight);
}


void ModelLoader::Rotate(double xpos, double ypos)
{
    m_pCamera->Rotate(xpos, ypos);
}

void ModelLoader::StartNavigation(double xpos, double ypos)
{
    m_pCamera->StartNavigation(xpos, ypos);
}

void ModelLoader::Pan(double xpos, double ypos)
{
    m_pCamera->Pan(xpos, ypos);
}

void ModelLoader::Zoom(float delta)
{
    m_pCamera->Zoom(delta);
}
