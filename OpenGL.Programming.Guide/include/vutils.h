#ifndef __VUTILS_H__
#define __VUTILS_H__

#include <GL/gl.h>
#include "vgl.h"

void vglAttachShaderSource(GLuint prog, GLenum type, const char* source)
{
    GLuint shader;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, source, NULL);
    glCompileShader(shader);

    char buf[4096];
    glGetShaderInfoLog(shader, sizeof(buf), NULL, buf);
    glAttachShader(prog, shader);
    glDeleteShader(shader);
}

#endif /* end of __VUTILS_H__ */