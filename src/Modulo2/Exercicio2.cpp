#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// --- sua função da Parte 1 ---
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
    glGenBuffers      (1, &vbo);

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

int main() {
    // --- inicialização GLFW/GLAD (não detalhado) ---
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800,600,"Ex2",nullptr,nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // --- 1) Instancia 5 triângulos ---
    std::vector<GLuint> tris;
    const float spacing = 0.4f;     // distância entre centros
    for(int i = 0; i < 5; ++i) {
        float offset = (i - 2) * spacing;  
        tris.push_back(createTriangle(
            -0.1f + offset, -0.1f,
             0.1f + offset, -0.1f,
             0.0f + offset,  0.1f
        ));
    }

    // --- 2) Loop de renderização ---
    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        for(auto vao : tris) {
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
