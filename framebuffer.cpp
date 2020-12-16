#include "framebuffer.h"

#include <iostream>

Framebuffer::Framebuffer(int width, int height) : 
    texid{},
    fb{},
    rb{},
    width{width},
    height{height}
{
    // Create framebuffer
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    
    // create a color attachment texture
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Bind color attatchment to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texid, 0);
    
    // Create renderbuffer
    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb); // now actually attach it

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    
    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindFBO()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
}

void Framebuffer::bindScreenFB()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Remember to activate correct tex to corresponing texunit beforehand with glActiveTexture(GL_TEXTURE0);
void Framebuffer::bindTex(Shader* shader, std::string texName, int texUnit)
{
    shader->useProgram();
    shader->uploadInt(texName, texUnit);
    glBindTexture(GL_TEXTURE_2D, texid);
}