#include "include/vapp.h"
#include "include/vutils.h"
#include "include/vbm.h"
#include "include/LoadShaders.h"

using namespace vmath;

BEGIN_APP_DECLARATION(transformFeedbackExample)
    //override functions from base class
    virtual void initialize(const char* title);
    virtual void display(bool autoRedRaw);
    virtual void finalize(void);
    virtual void resize(int width, int height);

    //memeber variables
    float aspect;
    GLuint updateProg;
    GLuint VAO[2];
    GLuint VBO[2];
    GLuint xfb;

    GLuint renderProg;
    GLuint geometryVBO;
    GLuint renderVAO;
    GLint renderModelMatrixLoc;
    GLint renderProjectionMatrixLoc;

    GLuint geometryTex;
    GLuint geometryXfb;
    GLuint particleXfb;

    GLint modelMatrixLoc;
    GLint projectionMatrixLoc;
    GLint triangleCountLoc;
    GLint timeStepLoc;

    VBObject object;
END_APP_DECLARATION()

DEFINE_APP(transformFeedbackExample, "TransformFeedback Example")

const int pointCount = 5000;
static unsigned int seed = 0x13371337;

static inline float randomFloat()
{
    float res;
    unsigned int tmp;

    seed *= 16807;
    tmp = seed ^ (seed >> 4) ^ (seed << 15);
    *((unsigned int *) &res) = (tmp >> 9) | 0x3f800000;
    return (res - 1.0f);
}

static vec3 randomVector(float minmag = 0.0f, float maxmag = 1.0f)
{
    vec3 randomVec(randomFloat()* 2.0f - 1.0f, randomFloat()* 2.0f - 1.0f, randomFloat()* 2.0f - 1.0f);
    randomVec = normalize(randomVec);
    randomVec *= (randomFloat() * (maxmag - minmag) + minmag);

    return randomVec;
}

void transformFeedbackExample::initialize(const char* title)
{
    base::initialize(title);
    static ShaderInfo updateShaders[] = {
        {GL_VERTEX_SHADER, "shaders/03-xfb/xfbUpdate.vs.glsl", 0},
        {GL_FRAGMENT_SHADER, "shaders/03-xfb/xfbUpdate.fs.glsl", 0},
        {GL_NONE, NULL, 0}
    };

    static ShaderInfo renderShaders[] = {
        {GL_VERTEX_SHADER, "shaders/03-xfb/xfb.vs.glsl", 0},
        {GL_FRAGMENT_SHADER, "shaders/03-xfb/xfb.fs.glsl", 0},
        {GL_NONE, NULL, 0}
    };

    static const char* varyings[] = {
        "positionOut",
        "velocityOut"
    };

    static const char* varyings2[] = {
        "worldPosition"
    };

    updateProg = loadShader(updateShaders);
    renderProg = loadShader(renderShaders);

    glTransformFeedbackVaryings(updateProg, 2, varyings, GL_INTERLEAVED_ATTRIBS);
    glUseProgram(updateProg);

    modelMatrixLoc = glGetUniformLocation(updateProg, "modelMatrix");
    projectionMatrixLoc = glGetUniformLocation(updateProg, "projectionMatrix");
    triangleCountLoc = glGetUniformLocation(updateProg, "triangleCount");
    timeStepLoc = glGetUniformLocation(updateProg, "timeStep");

    glTransformFeedbackVaryings(renderProg, 1, varyings2, GL_INTERLEAVED_ATTRIBS);
    glUseProgram(renderProg);
    renderModelMatrixLoc = glGetUniformLocation(renderProg, "modelMatrix");
    renderProjectionMatrixLoc = glGetUniformLocation(renderProg, "projectionMatrix");

    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    for (int i = 0; i < 2; i++) {
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, VBO[i]);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, pointCount * (sizeof(vec4) + sizeof(vec3)), NULL, GL_DYNAMIC_COPY);
        if ( i == 0) {
            struct buffer_t {
                vec4 position;
                vec3 velocity;
            } * buffer = (buffer_t*) glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_WRITE_ONLY);

            for (int j = 0 ; j < pointCount; j++) {
                buffer[j].velocity = randomVector();
                buffer[j].position = vec4(buffer[j].velocity + vec3(-0.5f, 40.0f, 0.0f), 1.0f);
                buffer[j].velocity = vec3(buffer[j].velocity[0], buffer[j].velocity[1] * 0.3f, buffer[j].velocity[2] * 0.3f);
            }
            glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
        }

        glBindVertexArray(VAO[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4) + sizeof(vec3), NULL);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec4) + sizeof(vec3), (GLvoid*)sizeof(vec4));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    glGenBuffers(1, &geometryVBO);
    glGenTextures(1, &geometryTex);
    glBindBuffer(GL_TEXTURE_BUFFER, geometryVBO);
    glBufferData(GL_TEXTURE_BUFFER, 1024 * 1024 * sizeof(vec4), NULL, GL_DYNAMIC_COPY);
    glBindTexture(GL_TEXTURE_BUFFER, geometryTex);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, geometryVBO);

    glGenVertexArrays(1, &renderVAO);
    glBindVertexArray(renderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, geometryVBO);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);

    object.loadFromVBM("./../../media/armadillo_low.vbm", 0, 1, 2);
}

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

void transformFeedbackExample::display(bool autoRedRaw)
{
    static int frameCount = 0;
    float t = float(appTime() & 0x1fff) / float(0x1fff);
    static float q = 0.0f;
    //we already import the user namespace vmath
    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

    mat4 projectionMatrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f) * translate(0.0f, 0.0f, -100.0f));
    mat4 modelMatrix(scale(0.3f) * rotate(t * 360.0f, 0.0f, 1.0f, 0.0f) * rotate(t * 360.0f * 3.0f, 0.0f, 0.0f, 1.0f));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(renderProg);
    glUniformMatrix4fv(renderModelMatrixLoc, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(renderProjectionMatrixLoc, 1, GL_FALSE, projectionMatrix);

    glBindVertexArray(renderVAO);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, geometryVBO);

    glBeginTransformFeedback(GL_TRIANGLES);
    object.render();
    glEndTransformFeedback();

    glUseProgram(updateProg);
    modelMatrix = mat4::identity();
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, projectionMatrix);
    glUniform1i(triangleCountLoc, object.getVertexCount() / 3);

    if (t > q) {
        glUniform1f(timeStepLoc, (t - q) * 2000.0f);
    }
    q = t;

    if ((frameCount & 1) != 0) {
        glBindVertexArray(VAO[1]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[0]);
    } else {
        glBindVertexArray(VAO[0]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, VBO[1]);
    }

    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, min(pointCount, (frameCount >> 3)));
    glEndTransformFeedback();

    glBindVertexArray(0);
    frameCount++;

    base::display();
}

void transformFeedbackExample::finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(updateProg);
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(2, VBO);
}

void transformFeedbackExample::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = float(height) / float(width);
}