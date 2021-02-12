#pragma once
#include <string>
 class OpenGLShaderManager
    {
    public:
        OpenGLShaderManager(const char* vsSourceData, const char* fsSourceData);
        OpenGLShaderManager(const std::string& vsPath, const std::string& fsPath);

        //return true if shader compiles and links
        //return false if shader fails or links to compile
        bool CompileAndLinkShader(std::string& error);
        //Loads shader program
        void Bind();
        void UnBind();

         void SetInt(const std::string& name, int value) ;
         void SetIntArray(const std::string& name, int* values, uint32_t count) ;
         void SetFloat(const std::string& name, float value) ;
         void SetFloat3(const std::string& name, const float* values) ;
         void SetFloat4(const std::string& name, const float* values) ;
         void SetMat4(const std::string& name, const float* matrix) ;


    protected:
        void ReadFile(const std::string& sFilePath, std::string& fileData);
    private:
        std::string m_VSData;
        std::string m_FSData;
        unsigned int m_iProgramId;
    };
