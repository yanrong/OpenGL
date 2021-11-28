#include "include/vgl.h"
#include "include/LoadShaders.h"

enum VAO_ID {Triangles, NumVAOs};
enum BUFFER_ID {ArrayBuffer, NumBuffers};
enum ATTRIB_ID {VPosition = 0};

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 6;

//init
//----------------------------------------------------------------------------
void init (void)
{
    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);

    GLfloat vertices[NumVertices][2] = {
        { -0.90f, -0.90f }, {  0.85f, -0.90f }, { -0.90f,  0.85f },  // Triangle 1
        {  0.90f, -0.85f }, {  0.90f,  0.90f }, { -0.85f,  0.90f }   // Triangle 2
    };

    glCreateBuffers(NumBuffers, Buffers);
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(vertices), vertices, 0);

    ShaderInfo shaders[] = {
        {GL_VERTEX_SHADER, "media/shaders/triangles/triangles.vert"},
        {GL_FRAGMENT_SHADER, "media/shaders/triangles/triangles.frag"},
        {GL_NONE, NULL}
    };

    GLuint program = loadShader(shaders);
    glUseProgram(program);

    glVertexAttribPointer(VPosition, 2, GL_Float, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(VPosition);
}

void display(void)
{
    static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};

    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray(VAOS[Triangles]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

int main(int argc, char *argv)
{
    if (!glfwInit()) {
        std::err << "GLFW init error" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //we are not consider the Mac OS platform, so skip compatibility set

    GLFWwindow* window = glfwCreateWindow(800, 600, "Triangles", NULL, NULL);
    if (!window) {
        std::err << "GLFW create window error" << std::endl;
        glfwTerminate();
        return -1;
    }
    //the original textbook demo use gl3w load function pointer.
    //now we chose GLEW instead
    glewExperimental;
    if (glewInit() != GLEW_OK) {
        std::err << "GLEW initialzation error" << std::endl;
        return -1;
    }
    //call init function, user defined
    init();

    while (!glfwWindowShouldClose(window)) {
        display;

        glfwSwapBuffers();
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}