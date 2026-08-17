#pragma once
#include"Opal/Core.h"
#include"Opal/Layer.h"
#include"Opal/Events/Event.h"
#include"Opal/Events/MouseEvent.h"

#include"Opal/Log.h"

#include"src/UI.h"
#include"src/Renderer.h"
namespace Opal
{
	class OPAL_API GuiLayer : public Layer
	{
	public:
		GuiLayer();
		virtual ~GuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnUpdate() override; 
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

	private:
		context ui_context;

		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& e);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& e);
	};
}