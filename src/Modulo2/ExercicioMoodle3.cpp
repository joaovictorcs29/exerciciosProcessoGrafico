#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

struct TriangleData {
    glm::vec2 pos;
    glm::vec3 color;
};

class Shader {
    GLuint id;
public:
    Shader(const char* vsSrc, const char* fsSrc) {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vs, 1, &vsSrc, nullptr);
        glCompileShader(vs);

        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fs, 1, &fsSrc, nullptr);
        glCompileShader(fs);

        id = glCreateProgram();
        glAttachShader(id, vs);
        glAttachShader(id, fs);
        glLinkProgram(id);

        glDeleteShader(vs);
        glDeleteShader(fs);
    }
    void use() const { glUseProgram(id); }
    GLint uni(const char* name) const { return glGetUniformLocation(id, name); }
};

GLuint makeTriangleVAO() {
    float verts[] = {
        -0.1f, -0.1f,
         0.1f, -0.1f,
         0.0f,  0.1f
    };
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers     (1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return vao;
}

glm::vec2 toNDC(double mx, double my, int w, int h) {
    return {
        float(mx / w * 2.0 - 1.0),
        float(1.0 - my / h * 2.0)
    };
}

int main() {
    srand((unsigned)time(nullptr));
    glfwInit();
    GLFWwindow* win = glfwCreateWindow(800, 600, "Dynamic Triangles", nullptr, nullptr);
    glfwMakeContextCurrent(win);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::vector<TriangleData> tris;
    GLuint triVAO = makeTriangleVAO();

    const char* vs = R"(
        #version 330 core
        layout(location=0) in vec2 aP;
        uniform mat4 uModel;
        void main(){ gl_Position = uModel * vec4(aP,0,1); }
    )";
    const char* fs = R"(
        #version 330 core
        uniform vec3 uColor;
        out vec4 oF;
        void main(){ oF = vec4(uColor,1); }
    )";
    Shader shader(vs, fs);
    GLint uM = shader.uni("uModel");
    GLint uC = shader.uni("uColor");

    glfwSetMouseButtonCallback(win, [](GLFWwindow* w, int b, int a, int){
        if(b==GLFW_MOUSE_BUTTON_LEFT && a==GLFW_PRESS){
            double x,y;
            int W,H;
            glfwGetCursorPos(w,&x,&y);
            glfwGetWindowSize(w,&W,&H);
            TriangleData t;
            t.pos = toNDC(x,y,W,H);
            t.color = { rand()/float(RAND_MAX), rand()/float(RAND_MAX), rand()/float(RAND_MAX) };
            static_cast<std::vector<TriangleData>*>(glfwGetWindowUserPointer(w))->push_back(t);
        }
    });
    glfwSetWindowUserPointer(win, &tris);

    while(!glfwWindowShouldClose(win)) {
        glClearColor(0.1f,0.1f,0.1f,1);
        glClear(GL_COLOR_BUFFER_BIT);
        shader.use();
        glBindVertexArray(triVAO);
        for(auto &t : tris){
            glm::mat4 M = glm::translate(glm::mat4(1), glm::vec3(t.pos,0));
            glUniformMatrix4fv(uM,1,GL_FALSE,&M[0][0]);
            glUniform3fv(uC,1,&t.color[0]);
            glDrawArrays(GL_TRIANGLES,0,3);
        }
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
