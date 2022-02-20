#include "include/vapp.h"
#include "include/vutils.h"
#include "include/vmath.h"
#include "include/LoadShaders.h"

using namespace vmath;

//Define USE_PRIMITIVE_RESTART to 0 to use two separate draw commands
#define USE_PRIMITIVE_RESTART 1

BEGIN_APP_DECLARATION(primitiveRestartExample)
    //override functions from base class
    virtual void initialize(const char* title);
    virtual void display(bool autoRedRaw);
    virtual void finalize(void);
    virtual void resize(int width, int height);

    //member variables
    float aspect;
    GLuint renderProg;
    GLuint VAO[1];
    GLuint VBO[1];
    GLuint EBO[1];

    GLint renderModelMatrixLoc;
    GLint renderProjectionMatrixLoc;
END_APP_DECLARATION()

DEFINE_APP(primitiveRestartExample, "Primitive Restart Example")

void primitiveRestartExample::initialize(const char* title)
{
    base::initialize(title);

    static ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "shaders/primitiveRestart/primitiveRestart.vs.glsl"},
        {GL_FRAGMENT_SHADER, "shaders/primitiveRestart/primitiveRestart.fs.glsl"},
        {GL_NONE, NULL}
    };

    renderProg = loadShader(shaders);
    glUseProgram(renderProg);

    //"modelMatrix" is actually an array of 4 matrices
    renderModelMatrixLoc = glGetUniformLocation(renderProg, "modelMatrix");
    renderProjectionMatrixLoc = glGetUniformLocation(renderProg, "projectionMatrix");

    // 8 corners of a cube, side length 2, centered on the origin
    static const GLfloat cubePositions[] =
    {
        -1.0f, -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    // Color for each vertex
    static const GLfloat cubeColors[] =
    {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f
    };

    // Indices for the triangle strips
    static const GLushort cubeIndices[] =
    {
        0, 1, 2, 3, 6, 7, 4, 5,         // First strip
        0xFFFF,                         // <<-- This is the restart index
        2, 6, 0, 4, 1, 5, 3, 7          // Second strip
    };

    //set up the element array buffer
    glGenBuffers(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    //set up the vertex attributes
    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    //just alloc memory
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubePositions) + sizeof(cubeColors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubePositions), cubePositions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubePositions), sizeof(cubeColors), cubeColors);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(cubePositions));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void primitiveRestartExample::display(bool autoRedRaw)
{
    float t = float(appTime() & 0x1fff) / float(0x1fff);
    static float q = 0.0f;
    //we already import the user namespace vmath
    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

    //setup
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //activate simple shading program
    glUseProgram(renderProg);

    mat4 modelMatrix(translate(0.0f, 0.0f, -5.0f) * rotate(t * 360.0f, Y) * rotate(t * 720.0f, Z));
    mat4 projectionMatrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 500.0f));

    glUniformMatrix4fv(renderModelMatrixLoc, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(renderProjectionMatrixLoc, 1, GL_FALSE, projectionMatrix);

    //set upt for glDrawElements call
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);

#if USE_PRIMITIVE_RESTART
    //when primitive restart is on, we can call one draw command
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(0xFFFF);
    glDrawElements(GL_TRIANGLE_STRIP, 17, GL_UNSIGNED_SHORT, NULL);
#else
    //without primitve restart, we need to call two draw commands
    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, NULL);
    glDrawElements(GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_SHORT, (const GLvoid*)(9 * sizeof(GLushort)));
#endif

    base::display();
}

void primitiveRestartExample::finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(renderProg);
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
}

void primitiveRestartExample::resize(int width, int height)
{
    glViewport(0, 0, width, height);

    aspect = float(height) / float(width);
}