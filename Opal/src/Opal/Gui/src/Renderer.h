#pragma once
#include "UI.h"

#include"Opal/Log.h"

namespace Opal {
    class Renderer {
    public:
        static void Init();
        static void Shutdown();
        static void RenderCommands(command* commands, int count, int windowWidth, int windowHeight);
    };
}
