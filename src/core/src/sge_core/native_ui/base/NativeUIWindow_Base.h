#pragma once

#include <sge_core/math/Rect2.h>

namespace sge {

	class NativeUIWindow_Base : public NonCopyable
	{
	public:
		struct CreateDesc
		{
			CreateDesc()
				:
				titleBar(true),
				isMainWindow(false),
				visible(true),
				resizable(true),
				closeButton(true),
				minButton(true),
				maxButton(true),
				centerToScreen(true),
				alwaysOnTop(false)
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

			bool	titleBar			: 1;
			bool	isMainWindow		: 1;
			bool	visible				: 1;
			bool	resizable			: 1;
			bool	closeButton			: 1;
			bool	minButton			: 1;
			bool	maxButton			: 1;
			bool	centerToScreen		: 1;
			bool	alwaysOnTop			: 1;
		};

		virtual ~NativeUIWindow_Base() = default;

		void create(CreateDesc& desc_)			{ onCreate(desc_); }
		void setWindowTitle(StrView title_)		{ onSetWindowTitleBar(title_); }

		virtual void onCloseButton() {}
		virtual void onActive(bool isActive_) {};

	protected:
		virtual void onCreate(CreateDesc& desc_) {}
		virtual void onSetWindowTitleBar(StrView title_) {}
	};
}