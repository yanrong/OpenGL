#include <iostream>
#include "include/vapp.h"
#include "include/LoadShaders.h"

enum VAO_ID {Triangles, NumVAOs};
enum BUFFER_ID {ArrayBuffer, NumBuffers};
enum ATTRIB_ID {VPosition = 0};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 6;

BEGIN_APP_DECLARATION(keyPressExample)
    virtual void initialize(const char* title);
    virtual void display(bool autoRedRaw);
    virtual void finalize(void);
    virtual void resize(int width, int height);
    void onKey(int key, int code, int action, int mods);
END_APP_DECLARATION()

DEFINE_APP(keyPressExample, "Key Press Example")

//init
void keyPressExample::initialize(const char* title)
{
    base::initialize(title);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);

    GLfloat vertices[NumVertices][2] = {
        { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
        {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
    };

    glGenBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "shaders/keyPress/keyPress.vert"},
        {GL_FRAGMENT_SHADER, "shaders/keyPress/keyPress.frag"},
        {GL_NONE, NULL}
    };

    GLuint program = loadShader(shaders);
    glUseProgram(program);

    glVertexAttribPointer(VPosition, 2 , GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(VPosition);
}

void keyPressExample::onKey(int key, int code, int action, int mods)
{
    if (action == GLFW_PRESS) {
        switch (key)
        {
        case GLFW_KEY_M:
            {
                static GLenum mode = GL_FILL;
                mode = (mode == GL_FILL ? GL_LINE : GL_FILL);
                glPolygonMode(GL_FRONT_AND_BACK, mode);
            }
            return;
        default:
            std::cout << "No keys matched: " << key <<std::endl;
            break;
        }
    }

    base::onKey(key, code, action, mods);
}

//display
void keyPressExample::display(bool autoRewRaw)
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAOs[Triangles]);
    glDrawArrays(GL_TRIANGLES, 0 , NumVertices);

    base::display(autoRewRaw);
}

void keyPressExample::resize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void keyPressExample::finalize(void)
{

}