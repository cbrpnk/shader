#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

GLFWwindow* window;

void errorCallback(int error, const char* msg)
{
    std::cout << error << " " << msg << '\n';
}

bool createWindow(int width, int height)
{
    glfwSetErrorCallback(errorCallback);
    
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    window = glfwCreateWindow(width, height, "Opengl Demo", nullptr, nullptr);
    if(!window) {
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(window);
    if(glewInit() != GLEW_OK) {
    std::cout << glGetError();
        std::cout << "Failed to initialize glew\n";
        glfwTerminate();
        return false;
    }
    
    // Viewport and projection
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 1.0, 425.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_FLAT); // GL_FLAT or GL_SMOOTH
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    return true;
}

void destroyWindow()
{
    glfwDestroyWindow(window);
}

std::string readShaderFile(std::string path) {
    std::ifstream file(path);
    std::string line;
    std::string buffer;
    while(std::getline(file, line)) {
        buffer += line;
        buffer += '\n';
    }
    return buffer;
}

int main(int argc, char** argv)
{
    int width = 640;
    int height = 480;
    
    if(!createWindow(width, height)) return -1;
    
    // Compile vertex shader
    // char shaderSource[] = "asdas";
    std::string vertexShaderSource = readShaderFile("./shaders/main.vert");
    const char* vertexp = vertexShaderSource.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexp, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
        std::cout << "Vertex shader error:" << infoLog << std::endl;
    }
    
    // Compile fragment shader
    std::string fragmentShaderSource = readShaderFile("./shaders/main.frag");
    const char* fragmentp = fragmentShaderSource.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentp, NULL);
    glCompileShader(fragmentShader);
    success = 0;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
        std::cout << "Fragment shader error:" << infoLog << std::endl;
    }
    
    // Link program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    success = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
        std::cout << "Link error:" << infoLog << std::endl;
    }
    
    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // VAO / VBO
    float triangle[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    
    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glfwSwapBuffers(window);
    usleep(1000000);
    
    //while(true) {
        //usleep(16000);
    //}
    
    destroyWindow();
    return 0;
}
