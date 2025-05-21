#include <iostream>
#include <vector>

// GLAD
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// Window dimensions
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// === Variáveis Globais ===
GLuint VBO, VAO;
vector<vec3> palette;
int paletteIndex = 0;
vector<vec3> triangleColors;
vector<float> vertices; // x, y, z for each vertex
vector<vec2> clickPositions; // temporary click storage (world coords)

// Shader sources
const char* vertexShaderSource = R"(
#version 400 core
layout(location = 0) in vec3 position;
uniform mat4 projection;
uniform mat4 model;
void main() {
    gl_Position = projection * model * vec4(position, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 400 core
uniform vec4 inputColor;
out vec4 color;
void main() {
    color = inputColor;
}
)";

// Callback: mouse button
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        clickPositions.emplace_back((float)x, (float)y);
        if (clickPositions.size() % 3 == 0) {
            size_t start = clickPositions.size() - 3;
            for (size_t i = start; i < clickPositions.size(); ++i) {
                vertices.push_back(clickPositions[i].x);
                vertices.push_back(clickPositions[i].y);
                vertices.push_back(0.0f);
            }
            triangleColors.push_back(palette[paletteIndex]);
            paletteIndex = (paletteIndex + 1) % palette.size();
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                         vertices.size() * sizeof(float),
                         vertices.data(),
                         GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}

// Compila e linka shaders, retorna ID do programa
GLuint setupShader() {
    GLint success;
    GLchar infoLog[512];
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vs, 512, NULL, infoLog);
        cout << "Vertex shader failed:\n" << infoLog << endl;
    }
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fs, 512, NULL, infoLog);
        cout << "Fragment shader failed:\n" << infoLog << endl;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        cout << "Program linking failed:\n" << infoLog << endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

int main() {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Triângulos Interativos", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }
    glViewport(0, 0, WIDTH, HEIGHT);

    // Inicializa paleta de cores (normalizada)
    vector<vec3> raw = {
        {200,191,231},{174,217,224},{181,234,215},{255,241,182},
        {255,188,188},{246,193,199},{255,216,190},{220,198,224},
        {208,230,165},{183,201,226}
    };
    for (auto& c : raw) {
        palette.push_back(c / 255.0f);
    }

    GLuint shaderID = setupShader();
    glUseProgram(shaderID);
    GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");
    GLint projLoc  = glGetUniformLocation(shaderID, "projection");
    GLint modelLoc = glGetUniformLocation(shaderID, "model");

    mat4 projection = ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, value_ptr(projection));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(mat4(1.0f)));

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0,0,0,1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        for (size_t i = 0; i < triangleColors.size(); ++i) {
            vec3 c = triangleColors[i];
            glUniform4f(colorLoc, c.r, c.g, c.b, 1.0f);
            glDrawArrays(GL_TRIANGLES, (GLint)(i * 3), 3);
        }
        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderID);
    glfwTerminate();
    return 0;
}
