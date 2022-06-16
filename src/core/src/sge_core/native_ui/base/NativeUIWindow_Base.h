#pragma once

#include <sge_core/math/Rect2.h>
#include <sge_core/input/UIEvent.h>

//#include <sge>

namespace sge {

	class NativeUIWindow_Base : public NonCopyable
	{
	public:
		struct CreateDesc
		{
			CreateDesc()
				:
				isTitleBar(true),
				isMainWindow(false),
				isVisible(true),
				isResizable(true),
				isCloseButton(true),
				isMinButton(true),
				isMaxButton(true),
				isCenterToScreen(true),
				isAlwaysOnTop(false)
			{}

			enum class Type
			{
				None = 0,
				NormalWindow,
				ToolWindow,
				PopupWindow,
			};
			Type	type = Type::NormalWindow;
			Rect2f	rect{ 10, 10, 640, 480 };

			bool	isTitleBar			: 1;
			bool	isMainWindow		: 1;
			bool	isVisible			: 1;
			bool	isResizable			: 1;
			bool	isCloseButton		: 1;
			bool	isMinButton			: 1;
			bool	isMaxButton			: 1;
			bool	isCenterToScreen	: 1;
			bool	isAlwaysOnTop		: 1;
		};

		virtual ~NativeUIWindow_Base() = default;

		void create(CreateDesc& desc_)			{ onCreate(desc_); }
		void setWindowTitle(StrView title_)		{ onSetWindowTitleBar(title_); }

		void drawNeeded() { onDrawNeeded(); }

		const Rect2f& clientRect() const { return _clientRect; }

		virtual void onCloseButton() {}
		virtual void onActive(bool isActive_) {};
		virtual void onDraw() {}

		virtual void onUINativeMouseEvent(UIMouseEvent& ev);
		virtual void onUIMouseEvent(UIMouseEvent& ev) {}

	protected:
		virtual void onCreate(CreateDesc& desc_) {}
		virtual void onSetWindowTitleBar(StrView title_) {}
		virtual void onClientRectChanged(const Rect2f& rc_) { _clientRect = rc_; }
		virtual void onDrawNeeded() {};

		Rect2f	_clientRect{ 0,0,0,0 };

		UIMouseEventButton _pressedMouseButtons = UIMouseEventButton::None;
		Vec2f _mousePos{0,0};
	};
}