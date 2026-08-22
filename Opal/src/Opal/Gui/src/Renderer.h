#pragma once
#include "UI.h"

#include"Opal/Log.h"

namespace Opal {
    class Renderer {
    public:
        static void Init();
        static void Shutdown();
        static void RenderCommands(RenderCommand* commands, uint32_t count, int windowWidth, int windowHeight);
    private:

        static unsigned int s_ShaderProgram ;
        static unsigned int s_QuadVAO ;
        static unsigned int s_InstanceVBO ;
    };
}
