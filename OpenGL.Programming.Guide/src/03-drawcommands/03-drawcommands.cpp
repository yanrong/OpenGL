#include <iostream>

#include "include/vmath.h"
#include "include/vapp.h"
#include "include/vutils.h"
#include "include/LoadShaders.h"

using namespace vmath;

BEGIN_APP_DECLARATION(drawCommandExample)
    //override function from base class
    virtual void initialize(const char* title);
    virtual void display(bool autoRedRaw);
    virtual void finalize(void);
    virtual void resize(int width, int height);

    //Memebe variables
    float aspcet;
    GLuint renderProgram;
    GLuint VAO[1];
    GLuint VBO[1];
    GLuint EBO[1];

    GLuint modelMatLoc;
    GLuint projectionMatLoc;
END_APP_DECLARATION()

DEFINE_APP(drawCommandExample, "Drawing Commands Example");

void drawCommandExample::initialize(const char* title)
{
    base::initialize(title);

    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "shaders/drawCommands/drawCommands.vert"},
        {GL_FRAGMENT_SHADER, "shaders/drawCommands/drawCommands.frag"},
        {GL_NONE, NULL}
    };

    renderProgram = loadShader(shaders);
    glUseProgram(renderProgram);

    //model matrix is actually an array of 4 matrices
    modelMatLoc = glGetUniformLocation(renderProgram, "model_matrix");
    projectionMatLoc = glGetUniformLocation(renderProgram, "projection_matrix");

    //A single triangle
    static const GLfloat vertexPositions[] = {
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
    };

    //color for each vertex
    static const GLfloat vertexColors[] = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 1.0f
    };

    //indices for the triangle strips
    static const GLfloat vertexIndices[] = {
        0, 1, 2
    };

    // setup the element array buffer
    glGenBuffer(1, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);

    //set up the vertex attribute
    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);

    glGenBuffer(1, VBO);
    glBindBUffer(GL_ARRAY_BUFFER, VBO[0]);
    // at first allocat the buffer memory, the buffer data is consist of two part, we will fill them respectively
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions) + sizeof(vertexColors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexPositions), vertexPositions); //part 1
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexPositions), sizeof(vertexColors), vertexColors); //part 2


    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0 , NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)sizeof(vertexPositions));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void drawCommandExample::display(bool autoRedRaw)
{
    float t = float(appTime() & 0x1fff) / (float) (0x1fff);
    static float q = 0.0f;
    static const vmath::vec3 x(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 z(0.0f, 0.0f, 1.0f);
    static const vmath::vec4 black = vmath::vec4(0.0f, 0.0f, 0.0f, 0.0f);

    mat4 modelMatrix;

    //setup
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glClearBufferfv(GL_COLOR, 0 , black);

    //active shading program
    glUseProgram(renderProgram);

    //set upt the model and projection matrix
    vmath::mat4 projectionMatrix(vmath::frustum(-0.1f, 1.0f, -aspcet, aspcet, 1.0f, 500.0f));
    glUniformMatrix4x2fv(projectionMatLoc, 1, GL_FALSE, projectionMatrix);

    //set up for glDrawElements
    glBindVertexArraySGIX(VAO[0]);
    glBindBuffers(GL_ELEMENT_ARRAY_BUFFER, VBO[0]);

    //draw arrays
    modelMatrix = vmath::translate(-3.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, modelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // draw elements
    modelMatrix = vmath::translate(-1.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, modelMatrix);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL);

    //draw elements base vertex
    modelMatrix = vmath::translate(1.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, modelMatrix);
    glDrawElementsBaseVertex(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, NULL, 1);

    //draw arrays instanced
    modelMatrix = vmath::translate(3.0f, 0.0f, -5.0f);
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, modelMatrix);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 1);

    base::display();
}

void drawCommandExample::finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(renderProgram);
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO)
}

void drawCommandExample::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    aspcet = float(height) / float(width);
}