#include "Texture.h"
#include "GL/glew.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
Texture::Texture()
{

}

Texture::~Texture()
{

}

unsigned int Texture::CreateGLTexture(const std::string& sPath)
{
    int iWidth, iHeight, iChannels;
    stbi_set_flip_vertically_on_load(true);
    float* data = stbi_loadf(sPath.c_str(), &iWidth, &iHeight, &iChannels, 0);
    GLenum format = GL_RGB;
    switch (iChannels)
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
    default:
        break;
    }
    GLuint iTexId;
    glGenTextures(1, &iTexId);
    glBindTexture(GL_TEXTURE_2D, iTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, iWidth, iHeight, 0, format, GL_FLOAT, data);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return iTexId;
}
