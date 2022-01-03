#ifndef __VAPP_H__
#define __VAPP_H__

#include <sys/time.h>
#include "include/vgl.h"

class vermilionApplication
{
protected:
    inline vermilionApplication(void){}
    virtual ~vermilionApplication(void){}

    static vermilionApplication *sApp;
    GLFWwindow *mPtrWin;

    struct timeval mAppStartTime;

    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int keys, int code, int action, int modes);
    static void charCallback(GLFWwindow* window, unsigned int codePoint);
    unsigned int appTime();
#ifdef DEBUG
    static void APIENTRY debugOutputCallback(GLenum source, GLenum type,
                                            GLenum id, GLenum severtiry,
                                            GLisize length, const GLchar* msg,
                                            GLvoid* params);
#endif

public:
    void mainLoop(void);
    virtual void initialize(const char* title = 0);
    virtual void display(bool autoReDraw = true)
    {
        glfwSwapBuffers(mPtrWin);
    }

    virtual void finalize(void) {}
    virtual void resize(int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    virtual void onKey(int key, int code, int action, int mods) {}
    virtual void onChar(unsigned int code) {}
};

// define static function
void vermilionApplication::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    vermilionApplication* pThis = (vermilionApplication*) glfwGetWindowUserPointer(window);
    pThis->resize(width, height);
}

void vermilionApplication::keyCallback(GLFWwindow* window, int key, int code, int action, int modes)
{
    vermilionApplication* pThis = (vermilionApplication*) glfwGetWindowUserPointer(window);
    pThis->onKey(key, code, action, modes);
}

void vermilionApplication::charCallback(GLFWwindow* window, unsigned int codePoint)
{
    vermilionApplication* pThis = (vermilionApplication*) glfwGetWindowUserPointer(window);
    pThis->onChar(codePoint);
}

unsigned int vermilionApplication::appTime()
{
    return 0;
}

void vermilionApplication::initialize(const char* title)
{
    gettimeofday(&mAppStartTime, nullptr);

    glfwInit();

#ifdef DBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    mPtrWin = glfwCreateWindow(800, 600, title ? title : "OpenGL Application", nullptr, nullptr);
    glfwSetWindowUserPointer(mPtrWin, this);
    glfwSetWindowSizeCallback(mPtrWin, windowSizeCallback);
    glfwSetKeyCallback(mPtrWin, keyCallback);
    glfwSetCharCallback(mPtrWin, charCallback);

    glfwMakeContextCurrent(mPtrWin);

    glewExperimental = true; // Needed for core profile
	glewInit();

    resize(800, 600);
#ifdef DEBUG
    if(glDebugMessageCallbackARB != NULL)
    {
        glDebugMessageCallbackARB(debugOutputCallback, this);
    }
#endif
}

#define BEGIN_APP_DECLARATION(appClass)        \
class appClass : public vermilionApplication    \
{                                               \
public:                                         \
    typedef class vermilionApplication base;    \
    static vermilionApplication * create(void)  \
    {                                           \
        return (sApp = new appClass);           \
    }
#define END_APP_DECLARATION()                   \
};



#ifdef DEBUG
#define DEBUG_OUTPUT_CALLBACK           \
void APIENTRY vermilionApplication::debugOutputCallback(GLenum source,  \
                                            GLenum type,                  \
                                            GLenum id,  \
                                            GLenum severtiry,       \
                                            GLisize length,\
                                            const GLchar* msg, \
                                            GLvoid* params)                    \
{                                                                              \
    outputDebugStringA(msg);                                                   \
    outputDebugStringA("\n");                                                  \
}
#else
/*
#define DEBUG_OUTPUT_CALLBACK                                                  \
void APIENTRY vermilionApplication::DebugOutputCallback(GLenum source,         \
                                                        GLenum type,           \
                                                        GLuint id,             \
                                                        GLenum severity,       \
                                                        GLsizei length,        \
                                                        const GLchar* message, \
                                                        GLvoid* userParam)     \
{                                                                              \
    printf("Debug Message: SOURCE(0x%04X), "\
                            "TYPE(0x%04X), "\
                            "ID(0x%08X), "\
                            "SEVERITY(0x%04X), \"%s\"\n",\
            source, type, id, severity, message);\
}
*/
#define DEBUG_OUTPUT_CALLBACK
#endif // end of debug

#define MAIN_DECL int main(int argc, char* argv[])

#define DEFINE_APP(appClass,title)                      \
vermilionApplication* vermilionApplication::sApp;       \
                                                        \
void vermilionApplication::mainLoop()                   \
{                                                       \
    do {                                                \
        display();                                      \
        glfwPollEvents();                               \
    } while (!glfwWindowShouldClose(mPtrWin));          \
}                                                       \
                                                        \
MAIN_DECL                                               \
{                                                       \
    vermilionApplication* app = appClass::create();     \
    app->initialize(title);                             \
    app->mainLoop();                                    \
    app->finalize();                                    \
    return 0;                                           \
}                                                       \
DEBUG_OUTPUT_CALLBACK

#endif /* __VAPP_H__ */
