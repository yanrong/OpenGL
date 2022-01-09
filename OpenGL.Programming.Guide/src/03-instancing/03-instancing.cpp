#include <iostream>

#include "include/LoadShaders.h"
#include "include/vapp.h"
#include "include/vmath.h"
#include "include/vutils.h"
#include "include/vbm.h"

BEGIN_APP_DECLARATION(instancingExample)
    //override function from base class
    virtual void initialize(const char* title);
    virtual void display(bool autoRedRaw);
    virtual void finialize(void);
    virtual void resize(int width, int height);

    //memeber variables
    float aspect;
    GLuint updateProg;
    GLuint vao[2];
    GLuint vbo[2];
    GLuint xfb;

    GLuint weightVbo;
    GLuint colorVbo;
    GLuint renderProg;
    GLint renderModelMatLoc;
    GLint renderProjectionMatLoc;

    GLuint geometryTex;
    GLuint geometryXfb;
    GLuint particleXfb;

    GLuint modelMatLoc;
    GLuint projectionMatLoc;
    GLint triangleCountLoc;
    GLint timeStepLoc;

    VBObject object;
END_APP_DECLARATION()

DEFINE_APP(instancingExample, "instancing Example")

#define INSTANCE_COUNT 200
static unsigned int seed = 0x13371337;

static inline float randomFloat()
{
    float res;
    unsigned int tmp;

    seed *= 16807;

    tmp = seed ^ (seed >> 4) ^ (seed << 15);
    *((unsigned int *)&res) = (tmp >> 9) | 0x3f800000;

    return (res - 1.0f);
}

static vmath::vec3 randomVector(float minmag = 0.0f, float maxmag = 1.0f)
{
    vmath::vec3 randomVec(randomFloat() * 2.0f - 1.0f, randomFloat() * 2.0f - 1.0f, randomFloat() * 2.0f - 1.0f);
    randomVec = normalize(randomVec);
    randomVec *= (randomFloat() * (maxmag - minmag) + minmag);
	
	return randomVec;
}

void instancingExample::initialize(const char* title)
{
    base::initialize(title);

    //create the program for rendering the model
    renderProg = glCreateProgram();
    //this is the rendering vertex shader
    static const char vertexShader[] =
        "#version 410\n"
        "\n"
        // Uniforms
        "uniform mat4 modelMatrix[4];\n"
        "uniform mat4 projectionMatrix;\n"
        // Regular vertex attributes
        "layout (location = 0) in vec4 position;\n"
        "layout (location = 1) in vec3 normal;\n"
        // Instanced vertex attributes
        "layout (location = 3) in vec4 instanceWeights;\n"
        "layout (location = 4) in vec4 instanceColor;\n"
        "\n"
        // Outputs to the fragment shader
        "out vec3 vNormal;\n"
        "out vec4 vColor;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    mat4 m = mat4(0.0);\n"
        "    vec4 pos = position;\n"
        // Normalize the weights so that their sum total is 1.0
        "    vec4 weights = normalize(instanceWeights);\n"
        "    for (int n = 0; n < 4; n++)\n"
        "    {\n"
        // Calulate a weighted average of the matrices
        "        m += (modelMatrix[n] * weights[n]);\n"
        "    }\n"
        // Use that calculated matrix to transform the object.
        "    vNormal = normalize((m * vec4(normal, 0.0)).xyz);\n"
        "    vColor = instanceColor;\n"
        "    gl_Position = projectionMatrix * (m * pos);\n"
        "}\n";

    // Simple fragment shader
    static const char fragmentShader[] =
        "#version 410\n"
        "\n"
        "layout (location = 0) out vec4 color;\n"
        "\n"
        "in vec3 vNormal;\n"
        "in vec4 vColor;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    color = vColor * (0.1 + abs(vNormal.z)) + vec4(0.8, 0.9, 0.7, 1.0) * pow(abs(vNormal.z), 40.0);\n"
        "}\n";
    
    vglAttachShaderSource(renderProg, GL_VERTEX_SHADER, vertexShader);
    vglAttachShaderSource(renderProg, GL_FRAGMENT_SHADER, fragmentShader);

    glLinkProgram(renderProg);
    glUseProgram(renderProg);

    // "model matrix" is actually an array of 4 matrices
    renderModelMatLoc = glGetUniformLocation(renderProg, "modelMatrix");
    renderProjectionMatLoc = glGetUniformLocation(renderProg, "projectionMatrix");

    // load the object
    object.loadFromVBM("media/armadillo_low.vbm", 0, 1, 2);
    // bind its vertex array object so that we can append the instanced attributes
    object.bindVertexArray();

    //generate the colors of the objects
    vmath::vec4 colors[INSTANCE_COUNT];

    for (int n = 0; n < INSTANCE_COUNT; n++) {
        float a = float(n) / 4.0f;
        float b = float(n) / 5.0f;
        float c = float(n) / 6.0f;

        colors[n][0] = 0.5f * (sinf(a + 1.0f) + 1.0f);
        colors[n][1] = 0.5f * (sinf(b + 2.0f) + 1.0f);
        colors[n][2] = 0.5f * (sinf(c + 3.0f) + 1.0f);
        colors[n][3] = 1.0f;
    }

    // create and allocate the vbo to hold the weights
    // notice that we use the 'colors' array as the initial data,
    // but only because we know it's the same size.
    glGenBuffers(1, &weightVbo);
    glBindBuffer(GL_ARRAY_BUFFER, weightVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

    // here is the instanced vertex attribute -set the divisor
    glVertexAttribDivisor(3, 1);
    // it's otherwise the same as any other vertex attribute - set the pointer and enable it
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(3);

    // same with the instance color array
    glGenBuffers(1, &colorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, colorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    glVertexAttribDivisor(4, 1);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(4);

    // Done (unbind the object's VAO)
    glBindVertexArray(0);
}

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

void instancingExample::display(bool autoRedRaw)
{
    float t = float(appTime() & 0x3ffff) / float(0x3ffff);
    static float q = 0.0f;
    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);

    vmath::vec4 weights[INSTANCE_COUNT];
    for (int n = 0; n < INSTANCE_COUNT; n++) {
        float a = float(n) / 4.0f;
        float b = float(n) / 5.0f;
        float c = float(n) / 6.0f;

        weights[n][0] = 0.5f * (sinf(t * 6.28318531f * 8.0f + a) + 1.0f);
        weights[n][1] = 0.5f * (sinf(t * 6.28318531f * 26.0f + b) + 1.0f);
        weights[n][2] = 0.5f * (sinf(t * 6.28318531f * 21.0f + c) + 1.0f);
        weights[n][3] = 0.5f * (sinf(t * 6.28318531f * 13.0f + a + b) + 1.0f);
    }

    //bind the weight VBO and change its data
    glBindBuffer(GL_ARRAY_BUFFER, weightVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(weights), weights, GL_DYNAMIC_DRAW);

    //clear 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //setup 
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //activate instancing program
    glUseProgram(renderProg);

    //set four model matrices
    vmath::mat4 modelMatrix[4];
    for (int n = 0; n < 4; n++)
    {
        modelMatrix[n] = (vmath::scale(5.0f) *
                           vmath::rotate(t * 360.0f * 40.0f + float(n + 1) * 29.0f, 0.0f, 1.0f, 0.0f) *
                           vmath::rotate(t * 360.0f * 20.0f + float(n + 1) * 35.0f, 0.0f, 0.0f, 1.0f) *
                           vmath::rotate(t * 360.0f * 30.0f + float(n + 1) * 67.0f, 0.0f, 1.0f, 0.0f) *
                           vmath::translate((float)n * 10.0f - 15.0f, 0.0f, 0.0f) *
                           vmath::scale(0.01f));
    }
    
    glUniformMatrix4fv(renderModelMatLoc, 4, GL_FALSE, modelMatrix[0]);
    //set up the projection matrix
    vmath::mat4 projectionMatrix(vmath::frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f) * vmath::translate(0.0f, 0.0f, -100.0f));
    glUniformMatrix4fv(renderProjectionMatLoc, 1, GL_FALSE, projectionMatrix);

    //render INSTANCE_COUNT objects
    object.render(0, INSTANCE_COUNT);

    base::display();
}

void instancingExample::finialize(void)
{
    glUseProgram(0);
    glDeleteProgram(updateProg);
    glDeleteVertexArrays(2, vao);
    glDeleteBuffers(2, vbo);
}

void instancingExample::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = float(height) / float(width);
}