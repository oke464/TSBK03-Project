#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_


#include <glad/glad.h>
#include <string>
#include "learnOpenGL/shader.h"



class Framebuffer
{
public:
    Framebuffer(int width, int height);

    void bindFBO();
    void bindScreenFB();
    void bindTex(Shader* shader, std::string texName, int texUnit);

    GLuint texid;
	GLuint fb;
	GLuint rb;
	GLuint depth;
	int width;
    int height;
private:
};


#endif