#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow *window);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

#define constexpr unsigned int WIDTH = 640;
constexpr unsigned int HEIGHT = 480;

double deltaTime = 0.0f;
double lastFrame = 0.0f;
double currentFrame = 0.0f;

double lastX = static_cast<double>(WIDTH) / 2.0;
double lastY = static_cast<double>(HEIGHT) / 2.0;

// avoid mouse jumping by checking if it is the first time moving the mouse
bool firstMouse = true;

int main()
{
    // glfw: initialize and configure
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // if you're using antialiasing
    // glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Hello OpenGL!", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // make the window's context current
    glfwMakeContextCurrent(window);

    //use if you want to disable the mouse cursor e.g. for a first person camera
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //enables v-sync
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //main loop
    while (!glfwWindowShouldClose(window))
    {
        currentFrame = glfwGetTime();
        processInput(window);

        // clear all relevant buffers
        glClearColor(0.0f, 0.31f, 0.68f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        //swaps the front and back buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        lastFrame = glfwGetTime();
        deltaTime = lastFrame - currentFrame;
    }

    // deallocating the memory for the windows and cleaning up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {

    // here you can add camera controls and other input stuff

    //escape key closed window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}
//the mouse callback
//this will get called every time the mouse is moved

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    double mouseDx = xpos - lastX;
    double mouseDy = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
}
