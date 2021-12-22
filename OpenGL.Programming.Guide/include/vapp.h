#ifdef __VAPP_H__
#define __VAPP_H__

#include "include/vgl.h"

class vermilionApplication
{
protected:
    inline vermilionApplication(void){}
    virtual ~vermilionApplication(void){}

    static vermilionApplication *sApp;
    GLFWwindow *mPWindow;

    struct timval mAppStartTime;

    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int keys, in code, int action, int modes);
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
        glfwSwapBuffers(mPWindow);
    }

    virtual finalized(void) {}
    virtual void resize(int wdith, int height)
    {
        glViewPort(0, 0, width, height);
    }

    virtual void onKey(int key, int code, int action, int modes) {}
    virtual void onChar(unsigned int code) {}
};

#define BEGIN_APP_DECLARATIONI(appClass)        \
class appClass : public vermilionApplication    \
{                                               \
public:                                         \
    typedef class vermilionApplication base;    \
    static vermilionApplication * create(void)  \
    {                                           \
        return (sApp = new appClass)            \
    }
#define END_APP_DECLARATION()                   \
};

#ifdef DEBUG
#define DEBUG_OUTPUT_CALLBACK
void APIENTRY debugOutputCallback(GLenum source, GLenum type,
                                            GLenum id, GLenum severtiry,
                                            GLisize length, const GLchar* msg,
                                            GLvoid* params)
{
    outputDebugStringA(msg);
    outputDebugStringA("\n");
}
#else
/*
#define DEBUG_OUTPUT_CALLBACK                                                   \
void APIENTRY VermilionApplication::DebugOutputCallback(GLenum source,         \
                                                        GLenum type,           \
                                                        GLuint id,             \
                                                        GLenum severity,       \
                                                        GLsizei length,        \
                                                        const GLchar* message, \
                                                        GLvoid* userParam)     \
{                                                                               \
    printf("Debug Message: SOURCE(0x%04X), "\
                            "TYPE(0x%04X), "\
                            "ID(0x%08X), "\
                            "SEVERITY(0x%04X), \"%s\"\n",\
            source, type, id, severity, message);\
}
*/
#define DEBUG_OUTPUT_CALLBACK
#endif // endof debug

#define MAIN_DECL int main(int argc, char* argv[])

#define DEFINE_APP(appClass, title)                     \
void vermilionApplication* vermilionApplicationi::sApp; \
                                                        \
void vermilionApplication::mainLoop()                   \
{                                                       \
    do {                                                \
        display();                                      \
        glfwPollEvents();                               \
    } while (!glfwWindowShouldClose(mPWindow));         \
}                                                       \
                                                        \
MAIN_DECL                                               \
{                                                       \
    vermilisonApplication* app = appClass::create();    \
    app->initialze(title);                              \
    app->mainLoop();                                    \
    app->finalize();                                    \
    return 0;                                           \
}                                                       \
DEBUG_OUTPUT_CALLBACK

#endif /* __VAPP_H__ */
