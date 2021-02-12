#pragma once
#include <string>
class Texture
{
public:
    Texture();
    ~Texture();

    unsigned int CreateGLTexture(const std::string& sPath);
};