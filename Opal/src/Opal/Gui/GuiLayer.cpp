#include"opalpch.h"
#include"GuiLayer.h"

namespace Opal
{
    struct UIButtonData {
        float x;          // Offset 0: 卡片当前 X
        float y;          // Offset 4: 卡片当前 Y
        float w;          // Offset 8: 卡片宽 (100)
        float h;          // Offset 12: 卡片高 (100)

        float drag_off_x; // Offset 16: 拖拽锚点偏移 X (鼠标 - 卡片)
        float drag_off_y; // Offset 20: 拖拽锚点偏移 Y
        float is_active;  // Offset 24: 激活状态 (用于变黄)
    };

	GuiLayer::GuiLayer():Layer("GuiLayer")
	{

	}
	void GuiLayer::OnAttach() 
    {
		memset(&ui_context, 0, sizeof(Context));
        Renderer::Init();

        OPAL_INFO("GuiLayer Attached & UI Renderer Initialized");
	}
    void GuiLayer::OnDetach() {
        Renderer::Shutdown();
        OPAL_INFO("GuiLayer Detached");
    }
    void GuiLayer::OnUpdate() 
    {
        ui_begin(&ui_context);

        static UIButtonData btn_data = { 300.0f, 200.0f, 100.0f, 100.0f, 0.0f };

        OIcode btn_logic = {};
        btn_logic.data = &btn_data;

        // ==========================================
        // 【触发字节码】：只要鼠标按下就执行
        // ==========================================
        uint8_t trigger_bc[] = {
            OP_GET_VAL_OFFSET, 8,                  // 读取 input_data->mousedown
            OP_PUSH_IMM_F32, 0x00, 0x00, 0x00, 0x3F, // 0.5f
            OP_GT                                  // mousedown > 0.5 ?
        };
        memcpy(btn_logic.trigger_bc, trigger_bc, sizeof(trigger_bc));
        btn_logic.trigger_len = sizeof(trigger_bc);

        // ==========================================
        // 【业务字节码】：纯粹的状态切换
        // 既然触发了，说明鼠标按下了，直接把颜色变黄
        // ==========================================
        uint8_t bc[] = {
            OP_PUSH_IMM_F32, 0x00, 0x00, 0x80, 0x3F, // 压入 1.0f
            OP_WRITE_OFFSET, 16                        // 写入 btn_data.is_active (Offset 16)
        };
        memcpy(btn_logic.bytecode, bc, sizeof(bc));
        btn_logic.bc_len = sizeof(bc);

        // 1. 执行 VM (如果按下，is_active 会变成 1.0)
        VM_Execute(&btn_logic, ui_context.vm_stack, &ui_context.vm_top, &ui_context.state);

        // 2. C++ 层只负责计算拖拽坐标
        static bool was_pressed = false;
        bool is_pressed = (ui_context.state.mousedown > 0);

        if (is_pressed) {
            static float drag_off_x = 0.0f;
            static float drag_off_y = 0.0f;

            if (!was_pressed) {
                drag_off_x = btn_data.x - (float)ui_context.state.mouse_x;
                drag_off_y = btn_data.y - (float)ui_context.state.mouse_y;
            }
            btn_data.x = (float)ui_context.state.mouse_x + drag_off_x;
            btn_data.y = (float)ui_context.state.mouse_y + drag_off_y;
        }

        // 【核心修复】：如果鼠标松开了 (Trigger 失败，VM 不执行)
        // 我们在 C++ 层把颜色重置为 0.0 (红色)
        if (!is_pressed) {
            btn_data.is_active = 0.0f;
        }

        was_pressed = is_pressed;

        // 3. 提交渲染
        RenderCommand cmd = {};
        cmd.x = btn_data.x; cmd.y = btn_data.y;
        cmd.w = btn_data.w; cmd.h = btn_data.h;
        cmd.color = (btn_data.is_active > 0.5f) ? 0xFFFFFF00 : 0xFFFF0000;
        DeclareCard(&ui_context, &btn_logic, &cmd, &ui_context.state);
        ui_end(&ui_context);
        
    }
    void GuiLayer::OnEvent(Event& e) 
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(GuiLayer::OnMouseMoved));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(GuiLayer::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(GuiLayer::OnMouseButtonReleased));

    }
    bool GuiLayer::OnMouseMoved(MouseMovedEvent& e) {
        ui_context.state.mouse_x = (int)e.GetX();
        ui_context.state.mouse_y = (int)e.GetY();
        return false;
    }

    bool GuiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == 0) { // 假设 0 是左键
            ui_context.state.mousedown = 1;
            ui_context.state.mousedown = 1;
        }
        return false;
    }

    bool GuiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == 0) {
            ui_context.state.mousedown = 0;
        }
        return false;
    }
}