#include"opalpch.h"
#include"GuiLayer.h"
namespace Opal
{
	GuiLayer::GuiLayer():Layer("GuiLayer")
	{

	}
	void GuiLayer::OnAttach() 
    {
		memset(&ui_context, 0, sizeof(context));
	}
    void GuiLayer::OnUpdate() 
    {
        ui_begin(&ui_context);

        // 测试 UI 按钮
        if (ui_button(&ui_context, 100, 100, 120, 40, "Start Game")) {
            OPAL_INFO("Start Game Clicked!");
        }
        if (ui_button(&ui_context, 100, 150, 120, 40, "Settings")) {
            OPAL_INFO("Settings Clicked!");
        }

        ui_end(&ui_context);
    }
    void GuiLayer::OnEvent(Event& e) 
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(GuiLayer::OnMouseMoved));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(GuiLayer::OnMouseButtonPressed));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(GuiLayer::OnMouseButtonReleased));

        // 如果 UI 正在被交互，拦截事件，不传给下层游戏逻辑
        if (ui_context.hover_id != 0 || ui_context.focus_id != 0) {
            e.Handled = true;
        }
    }
    bool GuiLayer::OnMouseMoved(MouseMovedEvent& e) {
        ui_context.mousepos.x = (int)e.GetX();
        ui_context.mousepos.y = (int)e.GetY();
        return false;
    }

    bool GuiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e) {
        if (e.GetMouseButton() == 0) { // 假设 0 是左键
            ui_context.mousedown = 1;
            ui_context.mousepress = 1;
        }
        return false;
    }

    bool GuiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == 0) {
            ui_context.mousedown = 0;
        }
        return false;
    }
}