// main.cpp

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

// -------------------------------------------------
// 1) Função createTriangle (Parte 1)
// -------------------------------------------------
GLuint createTriangle(float x0, float y0,
                      float x1, float y1,
                      float x2, float y2)
{
    float vertices[] = {
        x0, y0,
        x1, y1,
        x2, y2
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                            2 * sizeof(float), (void*)0);

      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vao;
}

// -------------------------------------------------
// 2) Helpers para compilar shaders e linkar programa
// -------------------------------------------------
static GLuint CompileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint ok;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetShaderInfoLog(shader, 512, nullptr, buf);
        std::cerr << "Shader compile error: " << buf << "\n";
    }
    return shader;
}

static GLuint CompileShaderProgram() {
    const char* vertSrc = R"(
        #version 330 core
        layout(location = 0) in vec2 aPos;
        uniform mat4 model;
        void main() {
            gl_Position = model * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char* fragSrc = R"(
        #version 330 core
        uniform vec3 inColor;
        out vec4 FragColor;
        void main() {
            FragColor = vec4(inColor, 1.0);
        }
    )";

    GLuint v = CompileShader(GL_VERTEX_SHADER,   vertSrc);
    GLuint f = CompileShader(GL_FRAGMENT_SHADER, fragSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);

    // checa link
    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetProgramInfoLog(prog, 512, nullptr, buf);
        std::cerr << "Program link error: " << buf << "\n";
    }
    // shaders já linkados: podemos descartar
    glDeleteShader(v);
    glDeleteShader(f);
    return prog;
}

// -------------------------------------------------
// 3) Struct Triangle e globals
// -------------------------------------------------
struct Triangle {
    glm::vec2 position;
    glm::vec3 color;
};

std::vector<Triangle> triangles;
GLuint triangleVAO;
GLuint shaderProgram;
int winWidth = 800, winHeight = 600;

// converte coords do mouse (pixels) → NDC [-1,1]
glm::vec2 screenToNDC(double x, double y) {
    float ndcX = float( (x / winWidth)  * 2.0 - 1.0 );
    float ndcY = float( 1.0 - (y / winHeight) * 2.0 );
    return { ndcX, ndcY };
}

void mouse_button_callback(GLFWwindow* w, int button, int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(w, &mx, &my);
        Triangle t;
        t.position = screenToNDC(mx, my);
        t.color = { float(rand())/RAND_MAX,
                    float(rand())/RAND_MAX,
                    float(rand())/RAND_MAX };
        triangles.push_back(t);
    }
}

// -------------------------------------------------
// 4) main()
// -------------------------------------------------
int main() {
    srand((unsigned)time(nullptr));

    // inicia GLFW + GLAD
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight,
                                          "Triangular Cloners", 
                                          nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr<<"Failed to init GLAD\n";
        return -1;
    }

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glViewport(0,0,winWidth,winHeight);

    // cria o VAO do triângulo “padrão”
    triangleVAO = createTriangle(
        -0.1f, -0.1f,
         0.1f, -0.1f,
         0.0f,  0.1f
    );

    // compila shaders
    shaderProgram = CompileShaderProgram();
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");
    GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");

    // loop
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(triangleVAO);

        for (auto &t : triangles) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f),
                                             glm::vec3(t.position, 0.0f));
            glUniformMatrix4fv(uniModel, 1, GL_FALSE, &model[0][0]);
            glUniform3fv     (uniColor, 1, &t.color[0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
