#include"opalpch.h"

#include "Renderer.h"
#include "Opal/GraphicsAPI/OpenGL/opalgl/include/opalgl.h" // 你的 OpenGL 头文件

namespace Opal {
    static unsigned int s_ShaderProgram = 0;
    static unsigned int s_VAO = 0, s_VBO = 0;

    // 极其简单的顶点着色器
    const char* s_VertexShader = R"(
        #version 460 core
        layout (location = 0) in vec2 aPos;
        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    // 极其简单的片段着色器
    const char* s_FragmentShader = R"(
        #version 460 core
        out vec4 FragColor;
        uniform vec4 u_Color;
        void main() {
            FragColor = u_Color;
        }
    )";

    void Renderer::Init() {
        using namespace Opal; // 方便调用 GLFL 里的函数
        OPAL_CORE_INFO("正在检查着色器编译状态...");
        // 1. 编译顶点着色器
        unsigned int vs = GLFL::glCreateShader(GL_VERTEX_SHADER);
        GLFL::glShaderSource(vs, 1, &s_VertexShader, NULL);
        GLFL::glCompileShader(vs);
        int success;
        char infoLog[512];
        GLFL::glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLFL::glGetShaderInfoLog(vs, 512, NULL, infoLog);
            OPAL_CORE_ERROR("顶点着色器编译失败: {}", infoLog);
        }
        // 2. 编译片段着色器
        unsigned int fs = GLFL::glCreateShader(GL_FRAGMENT_SHADER);
        GLFL::glShaderSource(fs, 1, &s_FragmentShader, NULL);
        GLFL::glCompileShader(fs);

        // 3. 链接着色器程序
        s_ShaderProgram = GLFL::glCreateProgram();
        GLFL::glAttachShader(s_ShaderProgram, vs);
        GLFL::glAttachShader(s_ShaderProgram, fs);
        GLFL::glLinkProgram(s_ShaderProgram);

        // 4. 删除着色器（链接后就不需要了）
        GLFL::glDeleteShader(vs);
        GLFL::glDeleteShader(fs);

        // 5. 创建 VAO 和 VBO
        GLFL::glGenVertexArrays(1, &s_VAO);
        GLFL::glGenBuffers(1, &s_VBO);

        GLFL::glBindVertexArray(s_VAO);
        GLFL::glBindBuffer(GL_ARRAY_BUFFER, s_VBO);
        // 预分配内存，提高每帧更新顶点的效率
        GLFL::glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 6 * MAX_COMMANDS, nullptr, GL_DYNAMIC_DRAW);
        GLFL::glEnableVertexAttribArray(0);
        GLFL::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        GLFL::glBindVertexArray(0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void Renderer::Shutdown() {
        GLFL::glDeleteVertexArrays(1, &s_VAO);
        GLFL::glDeleteBuffers(1, &s_VBO);
        GLFL::glDeleteProgram(s_ShaderProgram);
    }

    void Renderer::RenderCommands(command* commands, int count, int windowWidth, int windowHeight) {
        if (count == 0 || windowWidth == 0 || windowHeight == 0) return;

        GLFL::glUseProgram(s_ShaderProgram);
        GLFL::glBindVertexArray(s_VAO);

        int colorLoc = GLFL::glGetUniformLocation(s_ShaderProgram, "u_Color");
        float winW = (float)windowWidth;
        float winH = (float)windowHeight;

        for (int i = 0; i < count; ++i) {
            const command& cmd = commands[i];

            // 1. 设置颜色 (假设你的颜色是 0xRRGGBBAA 格式)
        float r = ((cmd.color >> 24) & 0xFF) / 255.0f; // Red   (最高位)
        float g = ((cmd.color >> 16) & 0xFF) / 255.0f; // Green
        float b = ((cmd.color >> 8) & 0xFF) / 255.0f;  // Blue
        float a = (cmd.color & 0xFF) / 255.0f;  
            GLFL::glUniform4f(colorLoc, r, g, b, a);

            // 2. 将 UI 像素坐标转换为 OpenGL 的 -1.0 ~ 1.0 坐标
            // 注意：OpenGL 的 Y 轴向上，UI 的 Y 轴向下，所以需要翻转
            float x1 = (cmd.rect.x / winW) * 2.0f - 1.0f;
            float y1 = 1.0f - (cmd.rect.y / winH) * 2.0f;
            float x2 = ((cmd.rect.x + cmd.rect.w) / winW) * 2.0f - 1.0f;
            float y2 = 1.0f - ((cmd.rect.y + cmd.rect.h) / winH) * 2.0f;

            // 3. 构造两个三角形（6个顶点）
            float vertices[] = {
                x1, y1, // 左下
                x2, y1, // 右下
                x2, y2, // 右上
                x1, y1, // 左下
                x2, y2, // 右上
                x1, y2  // 左上
            };

            // 4. 更新 VBO 并绘制
            GLFL::glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glDrawArrays(GL_TRIANGLES, 0, 6); // glDrawArrays 是 OpenGL 1.0 就有的，不需要手动加载
        }

        GLFL::glBindVertexArray(0);
        GLFL::glUseProgram(0);
    }
}