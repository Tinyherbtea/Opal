#include"opalpch.h"

#include "Renderer.h"
#include "Opal/GraphicsAPI/OpenGL/opalgl/include/opalgl.h" // 你的 OpenGL 头文件

namespace Opal {
    const char* s_VertexShaderSrc = R"(
        #version 460 core
        
        // 固定的单位矩形顶点 (1x1)
        layout(location = 0) in vec2 a_Pos; 

        // 每个实例独有的数据 (从 Instance VBO 读取)
        layout(location = 1) in vec4 a_Rect;   // x, y, w, h
        layout(location = 2) in uint a_Color;  // RGBA

        out vec4 v_Color;
        uniform vec2 u_WindowSize;

        void main() {
            // 将 1x1 的单位矩形，根据实例数据拉伸到实际大小
            vec2 finalPos = a_Pos * a_Rect.zw + a_Rect.xy;
            
            // 像素坐标转 NDC (-1.0 ~ 1.0)
            float ndcX = (finalPos.x / u_WindowSize.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (finalPos.y / u_WindowSize.y) * 2.0; // Y轴翻转
            
            gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);
            
            // 将 uint32 颜色解包为 vec4
            v_Color = vec4(
                float((a_Color >> 24) & 0xFF) / 255.0,
                float((a_Color >> 16) & 0xFF) / 255.0,
                float((a_Color >> 8) & 0xFF) / 255.0,
                float(a_Color & 0xFF) / 255.0
            );
        }
    )";

    // 2. 极简的片段着色器
    const char* s_FragmentShaderSrc = R"(
        #version 460 core
        in vec4 v_Color;
        out vec4 FragColor;
        void main() {
            FragColor = v_Color;
        }
    )";

    unsigned int Renderer::s_ShaderProgram = 0;
    unsigned int Renderer::s_QuadVAO = 0;
    unsigned int Renderer::s_InstanceVBO = 0;

    void Renderer::Init() {
        using namespace Opal; // 方便调用 GLFL 里的函数
        OPAL_CORE_INFO("正在检查着色器编译状态...");
        // 1. 编译顶点着色器
        unsigned int vs = GLFL::glCreateShader(GL_VERTEX_SHADER);
        GLFL::glShaderSource(vs, 1, &s_VertexShaderSrc, NULL);
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
        GLFL::glShaderSource(fs, 1, &s_FragmentShaderSrc, NULL);
        GLFL::glCompileShader(fs);

        // 3. 链接着色器程序
        s_ShaderProgram = GLFL::glCreateProgram();
        GLFL::glAttachShader(s_ShaderProgram, vs);
        GLFL::glAttachShader(s_ShaderProgram, fs);
        GLFL::glLinkProgram(s_ShaderProgram);

        // 4. 删除着色器（链接后就不需要了）
        GLFL::glDeleteShader(vs);
        GLFL::glDeleteShader(fs);

        float quadVertices[] = { 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f };
        unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

        unsigned int quadVBO, quadEBO;
        GLFL::glGenVertexArrays(1, &s_QuadVAO);
        GLFL::glBindVertexArray(s_QuadVAO);

        GLFL::glGenBuffers(1, &quadVBO);
        GLFL::glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        GLFL::glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
        GLFL::glEnableVertexAttribArray(0);
        GLFL::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        GLFL::glGenBuffers(1, &quadEBO);
        GLFL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
        GLFL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // ================= 3. 创建实例化 VBO =================
        GLFL::glGenBuffers(1, &s_InstanceVBO);
        GLFL::glBindBuffer(GL_ARRAY_BUFFER, s_InstanceVBO);
        GLFL::glBufferData(GL_ARRAY_BUFFER, MAX_RENDER_CMDS * sizeof(RenderCommand), nullptr, GL_DYNAMIC_DRAW);

        // 绑定 location=1 (vec4 Rect)
        GLFL::glEnableVertexAttribArray(1);
        GLFL::glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RenderCommand), (void*)0);
        GLFL::glVertexAttribDivisor(1, 1);

        // 绑定 location=2 (uint Color)
        GLFL::glEnableVertexAttribArray(2);
        GLFL::glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(RenderCommand), (void*)(4 * sizeof(float)));
        GLFL::glVertexAttribDivisor(2, 1);

        GLFL::glBindVertexArray(0);
    }

    void Renderer::Shutdown() {
        // 1. 删除实例化 VBO
        if (s_InstanceVBO != 0) {
            GLFL::glDeleteBuffers(1, &s_InstanceVBO);
            s_InstanceVBO = 0;
        }

        // 2. 删除 VAO (这也会连带删除内部的 quadVBO 和 quadEBO)
        if (s_QuadVAO != 0) {
            GLFL::glDeleteVertexArrays(1, &s_QuadVAO);
            s_QuadVAO = 0;
        }

        // 3. 删除 Shader 程序
        if (s_ShaderProgram != 0) {
            GLFL::glDeleteProgram(s_ShaderProgram);
            s_ShaderProgram = 0;
        }
    }

    void Renderer::RenderCommands(RenderCommand* commands, uint32_t count, int windowWidth, int windowHeight) 
    {
        if (count == 0) return;

        GLFL::glUseProgram(s_ShaderProgram);
        GLFL::glUniform2f(GLFL::glGetUniformLocation(s_ShaderProgram, "u_WindowSize"),
            (float)windowWidth, (float)windowHeight);

        // 【核心动作】：把 CPU 端的 render_pool 一次性拷贝到 GPU 显存
        GLFL::glBindBuffer(GL_ARRAY_BUFFER, s_InstanceVBO);
        GLFL::glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderCommand), commands);

        // 绑定 VAO 并执行实例化绘制
        GLFL::glBindVertexArray(s_QuadVAO);
        // 画 6 个顶点 (2个三角形)，总共画 count 个实例！
        GLFL::glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, count);

        GLFL::glBindVertexArray(0);
        GLFL::glUseProgram(0);
    }
}