#include "include/vapp.h"
#include "include/vutils.h"
#include "include/vmath.h"
#include "include/LoadShaders.h"

using namespace vmath;

#define POINT_COUNT 4

BEGIN_APP_DECLARATION(pointSpriteExample)
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
    GLuint spriteTexture;
    GLint renderModelMatLoc;
    GLint renderProjectionMatLoc;
END_APP_DECLARATION()

DEFINE_APP(pointSpriteExample, "Point Sprite Example")

namespace vtarga {
    unsigned char * load_targa(const char *filename, GLenum &format, int & width, int &height);
}

static inline float randomFloat()
{
    float res;
    unsigned int tmp;
    static unsigned int seed = 0x13371337;

    seed *= 16807;
    tmp = seed ^ (seed >> 4) ^ (seed << 15);
    *((unsigned int *) &res) = (tmp >> 9) | 0x3f800000;
    return (res - 1.0f);
}

void pointSpriteExample::initialize(const char* title)
{
    base::initialize(title);

    GLenum format;
    int width, height;
    unsigned char* data;

    glGenTextures(1, &spriteTexture);
    glBindTexture(GL_TEXTURE_2D, spriteTexture);

    data = vtarga::load_targa("media/sprite2.tga", format, width, height);

    glTexImage2D(GL_TEXTURE_2D, 0 , GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    delete [] data;

    glGenerateMipmap(GL_TEXTURE_2D);

    static ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "shaders/pointsprites/pointsprites.vs.glsl", 0},
        {GL_FRAGMENT_SHADER, "shaders/pointsprites/pointsprites2.fs.glsl", 0},
        {GL_NONE, NULL, 0}
    };

    renderProg = loadShader(shaders);
    glUseProgram(renderProg);

    //model matrix is actually an array of 4 matrices
    renderModelMatLoc = glGetUniformLocation(renderProg, "modelMatrix");
    renderProjectionMatLoc = glGetUniformLocation(renderProg, "projectionMatrix");

    //a single triangle
    static vec4* vertexPositions;
    //set up the vertex attributes
    glGenVertexArrays(1, VAO);
    glBindVertexArray(VAO[0]);

    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, POINT_COUNT * sizeof(vec4), NULL, GL_STATIC_DRAW);

    vertexPositions = (vec4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int n = 0; n < POINT_COUNT; n++) {
        vertexPositions[n] = vec4(randomFloat() * 2.0f - 1.0f, randomFloat() * 2.0f - 1.0f, randomFloat() * 2.0f - 1.0f, 1.0f);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)sizeof(vertexPositions));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void pointSpriteExample::display(bool autoRedRaw)
{
    float t = float(appTime() & 0x1fff) / float(0x1fff);
    static float q = 0.0f;
    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

    mat4 modelMatrix;

    //set up
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //activate simple shading program
    glUseProgram(renderProg);
    //set up the model and projection matrix
    mat4 projectionMatrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 8.0f));
    glUniformMatrix4fv(renderProjectionMatLoc, 1, GL_FALSE, projectionMatrix);

    glBindTexture(GL_TEXTURE_2D, spriteTexture);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(32.0f);

    //draw arrays
    modelMatrix = translate(0.0f, 0.0f, -2.0f) * rotate(t * 360.0f, Y) * rotate(t * 720.0f, Z);
    glUniformMatrix4fv(renderModelMatLoc, 1, GL_FALSE, modelMatrix);
    glDrawArrays(GL_POINTS, 0, POINT_COUNT);

    base::display();
}

void pointSpriteExample::finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(renderProg);
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
}

void pointSpriteExample::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = float(height) / float(width);
}