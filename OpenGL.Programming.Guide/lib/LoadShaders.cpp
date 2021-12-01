#include <cstdio>
#include <unistd.h>
#include "include/LoadShaders.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//----------------------------------------------------------------------------
static const GLchar* readShader(const char *filename)
{
    std::string fullPath = getPath(filename);
    std::cout << "Full path: " << fullPath.c_str() << std::endl;
    FILE* infile = fopen(fullPath.c_str(), "rb");
    if (!infile) {
#ifdef _DEBUG
        std::cerr << "Unable to openg file '" <<  filename << "'" << std::endl;
#endif
        return NULL;
    }

    fseek(infile, 0 , SEEK_END);
    int len = ftell(infile);
    fseek(infile, 0 ,SEEK_SET);

    GLchar* source = new GLchar[len + 1];
    fread(source, 1, len, infile);
    fclose(infile);

    source[len] = 0; //add a null char for string terminate char

    return const_cast<const GLchar*>(source);
}

//----------------------------------------------------------------------------
GLuint loadShader(ShaderInfo* shaders)
{
    if (shaders == NULL) {
        return 0;
    }

    GLuint program = glCreateProgram();

    ShaderInfo* entry = shaders;
    while (entry->type != GL_NONE) {
        GLuint shader = glCreateShader(entry->type);
        entry->shader = shader;
        const GLchar * source = readShader(entry->filename);
        if (source == NULL) {
            for ( entry = shaders; entry->type != GL_NONE; ++entry) {
                glDeleteShader(entry->shader);
                entry->shader = 0;
            }

            return 0;
        }

        glShaderSource(shader, 1, &source, NULL);
        delete []source;

        glCompileShader(shader);

        //check compile status if the DEBUG is on
        GLint compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
#ifdef _DEBUG
            GLsizei len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            GLchar* log = new GLchar[len + 1];
            glGetShaderInfoLog(shader, len, &len, log);
            std::cerr << "Shader compliation failed" << log << std::endl;
            delete []log;
#endif
            return 0;
        }
        glAttachShader(program, shader);
        entry++;
    }

    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
#ifdef _DEBUG
        GLsizei len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        GLchar *log = new GLchar[len + 1];
        glGetProgramInfoLog(program, len, &len, log);
        std::cerr << "Shader linking failed: " << log << std::endl;
        delete[] log;
#endif

        for (entry = shaders; entry->type != GL_NONE; ++entry) {
            glDeleteProgram(entry->shader);
            entry->shader = 0;
        }

        return 0;
    }

    return program;
}

std::string getPath(const char* resource)
{
    const char *currentPath = getRelativePath();
    if(currentPath != NULL) {
        std::string cur(currentPath);
        delete []currentPath;
        return cur + "/../../" + resource;
    } else {
        return NULL;
    }
}

const char* getRelativePath()
{
    char* currentPath = new char[512];
    if (getcwd(currentPath, 512) != NULL){
        return currentPath;
    }
    delete []currentPath;
    return NULL;
}

#ifdef __cplusplus
}
#endif // __cplusplus
