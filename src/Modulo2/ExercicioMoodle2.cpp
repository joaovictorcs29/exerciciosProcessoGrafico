#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    float verts[] = { x0, y0, x1, y1, x2, y2 };
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers     (1, &vbo);
    glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return vao;
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "5 Triangles", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::vector<GLuint> vaos;
    for (int i = 0; i < 5; ++i) {
        float dx = (i - 2) * 0.4f;
        vaos.push_back(createTriangle(
            -0.1f + dx, -0.1f,
             0.1f + dx, -0.1f,
             0.0f + dx,  0.1f
        ));
    }

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        for (auto vao : vaos) {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
