#include "include/LoadShaders.h"
#include "include/vgl.h"
#include <cstdlib>
#include <cstring>

size_t typeSize(GLenum type)
{
    ssize_t size;
#define CASE(Enum, Count, Type) \
    case Enum: size = Count * sizeof(Type); break

    switch(type) {
        CASE(GL_FLOAT,      1, GLfloat);
        CASE(GL_FLOAT_VEC2, 2, GLfloat);
        CASE(GL_FLOAT_VEC3, 3, GLfloat);
        CASE(GL_FLOAT_VEC4, 4, GLfloat);
        CASE(GL_INT,        1, GLint);
        CASE(GL_INT_VEC2,   2, GLint);
        CASE(GL_INT_VEC3,   3, GLint);
        CASE(GL_INT_VEC4,   4, GLint);
        CASE(GL_UNSIGNED_INT,      1, GLuint);
        CASE(GL_UNSIGNED_INT_VEC2, 2, GLuint);
        CASE(GL_UNSIGNED_INT_VEC3, 3, GLuint);
        CASE(GL_UNSIGNED_INT_VEC4, 4, GLuint);
        CASE(GL_BOOL,        1, GLboolean);
        CASE(GL_BOOL_VEC2,   2, GLboolean);
        CASE(GL_BOOL_VEC3,   3, GLboolean);
        CASE(GL_BOOL_VEC4,   4, GLboolean);
        CASE(GL_FLOAT_MAT2,     4, GLfloat);
        CASE(GL_FLOAT_MAT2x3,   6, GLfloat);
        CASE(GL_FLOAT_MAT2x4,   8, GLfloat);
        CASE(GL_FLOAT_MAT3,     9, GLfloat);
        CASE(GL_FLOAT_MAT3x2,   6, GLfloat);
        CASE(GL_FLOAT_MAT3x4,   12, GLfloat);
        CASE(GL_FLOAT_MAT4,     16, GLfloat);
        CASE(GL_FLOAT_MAT4x2,   8, GLfloat);
        CASE(GL_FLOAT_MAT4x3,   12, GLfloat);
#undef CASE
        default:
            std::cerr << "Unkown type 0x" << std::hex << type << std::endl;
            exit(EXIT_FAILURE);
            break;
    }

    return size;
}

void init()
{
    GLuint program;
    GLuint uboIndex, uboSize, ubo;
    GLvoid *buffer = NULL;

    glClearColor(1, 0, 0, 1);
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "shaders/uniform-block/uniform_block.vert"},
        {GL_FRAGMENT_SHADER, "shaders/uniform-block/uniform_block.frag"},
        {GL_NONE, NULL}
    };

    program = loadShader(shaders);
    glUseProgram(program);

    //init the uniform blocks members
    //get uniform block INDEX
    uboIndex = glGetUniformBlockIndex(program, "Uniforms");

    glGetActiveUniformsiv(program, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);
    //allocate new buffer to hold the uniform block value
    buffer = malloc(uboSize);
    if (buffer == NULL) {
        std::cerr << "Unable to allocate buffer" << std::endl;
        exit(EXIT_FAILURE);
    }

    enum
    {
        Translation,
        Scale,
        Rotation,
        Enabled,
        NumUniforms
    };
    GLfloat scale = 0.5;
    GLfloat transloation = {0.1, 0.1, 0.0};
    GLfloat rotation[] = {90, 0.0, 0.0, 1.0};
    GLboolean enabled = GL_TRUE;

    const char8 names[NumUniforms] = {
        "translation",
        "scale",
        "rotation",
        "enabled"
    };

    GLuint indices[NumUniforms];
    GLint size[NumUniforms];
    GLint offset[NumUniforms];
    GLint type[NumUniforms];

    //get uniform block members offset
    glGetUniformIndices(program, NumUniforms, names, indices);
    //glGetActiveUniform
    glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_OFFSET, offset);
    //copy the uniform block member value to the memory buffer
    memcpy(buffer + offset[Scale], &scale, size[Scale] * typeSize(type[Scale]));
    memcpy(buffer + offset[Translation], &transloation, size[Translation] * typeSize(type[Translation]));
    memcpy(buffer + offset[Rotation], &rotation, size[Rotation] * typeSize(type[Rotation]));
    memcpy(buffer + offset[Enabled], &enabled, size[Enabled] * typeSize(type[Enabled]));

    //glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_SIZE, size);
    glGetActiveUniformsiv(program, NumUniforms, indices, GL_UNIFORM_TYPE, type);

    //copy the uniform block member value to the memory buffer
    memcpy(buffer + offset[Scale], &scale, size[Scale] * typeSize(type[Scale]));
    memcpy(buffer + offset[Translation], &transloation, size[Translation] * typeSize(type[Translation]));
    memcpy(buffer + offset[Rotation], &rotation, size[Rotation] * typeSize(type[Rotation]));
    memcpy(buffer + offset[Enabled], &enabled, size[Enabled] * typeSize(type[Enabled]));

    //create uniforms buffer object, initialization, and establish a connect to shader
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, uboSize, buffer, GL_STATIC_DRAW);
    glBindBuffersBase(GL_UNIFORM_BUFFER, uboIndex, ubo);
}