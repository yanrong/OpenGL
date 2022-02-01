#include "include/vapp.h"
#include "include/vutils.h"
#include "include/vbm.h"
#include "include/LoadShaders.h"

using namespace vmath;

BEGIN_APP_DECLARATION(instancingExample)
    // override functions from base class
    virtual void initialize(const char* title);
    virtual void display(bool autoRedRaw);
    virtual void finalize(void);
    virtual void resize(int width, int height);

    //member variables
    float aspect;

    GLuint colorBuffer;
    GLuint modelMatrixBuffer;
    GLuint renderProg;
    GLint modelMatrixLoc;
    GLint viewMatrixLoc;
    GLint projectionMatrixLoc;

    VBObject object;
END_APP_DECLARATION()

DEFINE_APP(instancingExample, "instanceing example")

#define INSTANCE_COUNT 100

void instancingExample::initialize(const char* title)
{
    base::initialize(title);

    //create the program for rendering the model
    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "shaders/03-instancing2/03-instancing2.vert"},
        {GL_FRAGMENT_SHADER, "shaders/03-instancing2/03-instancing2.frag"},
        {GL_NONE, NULL}
    };

    renderProg = loadShader(shaders);
    glUseProgram(renderProg);

    viewMatrixLoc = glGetUniformLocation(renderProg, "viewMatrix");
    projectionMatrixLoc = glGetUniformLocation(renderProg, "projectionMatrix");

    //load the object
    object.loadFromVBM("./../../media/armadillo_low.vbm", 0, 1, 2);

    //bind its vertex array object so that we can append the instanced attributes
    object.bindVertexArray();

    //get the locations of the vertex attributes in prog, which is the (linked)program
    //object that we're going to rendering with.note that this isn't really necessary
    //because we specified location for all the attributes in our vertex shader.this code
    //could be made more concise by assuming the vertex attributes are where we asked
    //the compiler to put them
    int positionLoc = glGetAttribLocation(renderProg, "vPosition");
    int normalLoc = glGetAttribLocation(renderProg, "vNormal");
    int colorLoc = glGetAttribLocation(renderProg, "vColor");
    int matrixLoc = glGetAttribLocation(renderProg, "modelMatrix");

    //configure the regular vertex attribute arrays-position and color
    /*
    // This is commented out here because the VBM object takes care
    // of it for us.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glVertexAttribPointer(positionLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(positionLoc);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(normalLoc);
    */

    //generate the colors of the objects
    vec4 colors[INSTANCE_COUNT];

    for (int n = 0; n < INSTANCE_COUNT; n++)
    {
        float a = float(n) / 4.0f;
        float b = float(n) / 5.0f;
        float c = float(n) / 6.0f;

        colors[n][0] = 0.5f + 0.25f * (sinf(a + 1.0f) + 1.0f);
        colors[n][1] = 0.5f + 0.25f * (sinf(b + 2.0f) + 1.0f);
        colors[n][2] = 0.5f + 0.25f * (sinf(c + 3.0f) + 1.0f);
        colors[n][3] = 1.0f;
    }

    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_DYNAMIC_DRAW);

    //now we set up the color array. we want each instance of our geometry to assume
    //a different color, so we'll just pack colors into a buffer
    //object and make an instanced vertex attribute out of it.
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(colorLoc);
    //this is the important bit... set the divisor for the color array to 1 to get
    //OpenGL to give us a new alue of 'color' per-instance ranther than pre-vertex
    glVertexAttribDivisor(colorLoc, 1);

    // Likewise, we can do the same with the model matrix. Note that a
    // matrix input to the vertex shader consumes N consecutive input
    // locations, where N is the number of columns in the matrix. So...
    glGenBuffers(1, &modelMatrixBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBuffer);
    glBufferData(GL_ARRAY_BUFFER, INSTANCE_COUNT * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);

    //Loop over each column of the matrix...
    for (int i = 0; i < 4; i++)
    {
        //set up the vertex attribute
        glVertexAttribPointer(matrixLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
        //enable it
        glEnableVertexAttribArray(matrixLoc + i);
        //make it instanced
        glVertexAttribDivisor(matrixLoc + i, 1);
    }

    //done (unbind the object's VAO)
    glBindVertexArray(0);
}

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

void instancingExample::display(bool autoRedRaw)
{
    float t = float(appTime() & 0x3fff) / float(0x3fff);
    static float q = 0.0f;
    static const vec3 X(1.0f, 0.0f, 0.0f);
    static const vec3 Y(0.0f, 1.0f, 0.0f);
    static const vec3 Z(0.0f, 0.0f, 1.0f);

    //clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //setup
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    //bind the weight VBO and change its data
    glBindBuffer(GL_ARRAY_BUFFER, modelMatrixBuffer);
    //set model matrices for each instance
    mat4* matrices = (mat4 *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int n = 0; n < INSTANCE_COUNT; n++) {
        float a = 50.0f * float(n) / 4.0f;
        float b = 50.0f * float(n) / 5.0f;
        float c = 50.0f * float(n) / 6.0f;

        matrices[n] = rotate(a + t * 360.0f, 1.0f, 0.0f, 0.0f) *
                      rotate(b + t * 360.0f, 0.0f, 1.0f, 0.0f) *
                      rotate(c + t * 360.0f, 0.0f, 0.0f, 1.0f) *
                      translate(10.0f + a, 40.0f + b, 50.0f + c);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    //active insancing program
    glUseProgram(renderProg);
    //setup the view and projection matrices
    mat4 viewMatrix(translate(0.0f, 0.0f, -1500.0f) * rotate(t * 360.0f * 2.0f, 0.0f, 1.0f, 0.0f));
    mat4 projectionMatrix(frustum(-1.0f, 1.0f, -aspect, aspect, 1.0f, 5000.0f));

    glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, viewMatrix);
    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, projectionMatrix);

    //render INSTANCE_COUNT objects
    object.render(0, INSTANCE_COUNT);
    lookat(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    base::display();
}

void instancingExample::finalize(void)
{
    glUseProgram(0);
    glDeleteProgram(renderProg);
    glDeleteBuffers(1, &colorBuffer);
    glDeleteBuffers(1, &modelMatrixBuffer);
}

void instancingExample::resize(int width, int height)
{
    glViewport(0, 0, width, height);
    aspect = float(height) / float(width);
}