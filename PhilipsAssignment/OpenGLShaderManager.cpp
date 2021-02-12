#include "OpenGLShaderManager.h"
#include <fstream>
#include <GL/glew.h>
OpenGLShaderManager::OpenGLShaderManager(const char* vsSourceData, const char* fsSourceData)
    :m_iProgramId(0)
{
    m_VSData = vsSourceData;
    m_FSData = fsSourceData;
}

OpenGLShaderManager::OpenGLShaderManager(const std::string& vsPath, const std::string& fsPath)
    :m_iProgramId(0)
{
    ReadFile(vsPath, m_VSData);
    ReadFile(fsPath, m_FSData);
}

bool OpenGLShaderManager::CompileAndLinkShader(std::string& error)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vSource = m_VSData.c_str();
    glShaderSource(vertexShader, 1, &vSource, 0);
    glCompileShader(vertexShader);
    GLint isCompiled;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
        char* errorStr = new char[maxLength];
        glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &errorStr[0]);

        delete[] errorStr;
        //We don't need the shader anymore.
        glDeleteShader(vertexShader);
        return false;
    }
    m_VSData.clear();
    //Create an empty fragment shader handle
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //Send the fragment shader source code to GL
    //Note that std::string's .c_str is NULL character terminated.
    const char* fSource = m_FSData.c_str();
    glShaderSource(fragmentShader, 1, &fSource, 0);

    //Compile the fragment shader
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
        char* errorStr = new char[maxLength];

        //The maxLength includes the NULL character
        glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &errorStr[0]);
        delete[] errorStr;
        //We don't need the shader anymore.
        glDeleteShader(fragmentShader);
        //Either of them. Don't leak shaders.
        glDeleteShader(vertexShader);

        //Use the infoLog as you see fit.

        //In this simple program, we'll just leave
        return false;
    }
    m_iProgramId = glCreateProgram();
    glAttachShader(m_iProgramId, vertexShader);
    glAttachShader(m_iProgramId, fragmentShader);
    //Link our program
    glLinkProgram(m_iProgramId);

    //Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(m_iProgramId, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(m_iProgramId, GL_INFO_LOG_LENGTH, &maxLength);
        char* errorStr = new char[maxLength];

        glGetProgramInfoLog(m_iProgramId, maxLength, &maxLength, &errorStr[0]);

        delete[] errorStr;
        //We don't need the program anymore.
        glDeleteProgram(m_iProgramId);
        //Don't leak shaders either.
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    //Always detach shaders after a successful link.
    glDetachShader(m_iProgramId, vertexShader);
    glDetachShader(m_iProgramId, fragmentShader);
    return true;
}


void OpenGLShaderManager::ReadFile(const std::string& filepath, std::string& fileData)
{

    std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
    if (in)
    {
        in.seekg(0, std::ios::end);
        size_t size = in.tellg();
        if (size != -1)
        {
            fileData.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&fileData[0], size);
        }
    }
}

void OpenGLShaderManager::Bind()
{
    glUseProgram(m_iProgramId);
}

void OpenGLShaderManager::UnBind()
{
    glUseProgram(0);
}

void OpenGLShaderManager::SetInt(const std::string& name, int value)
{
    GLint location = glGetUniformLocation(m_iProgramId, name.c_str());
    glUniform1i(location, value);
}

void OpenGLShaderManager::SetFloat3(const std::string& name, const float* values)
{
    GLint location = glGetUniformLocation(m_iProgramId, name.c_str());
    glUniform3f(location, values[0], values[1], values[2]);
}

void OpenGLShaderManager::SetMat4(const std::string& name, const float* matrix)
{
    GLint location = glGetUniformLocation(m_iProgramId, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}
