#pragma once

#include <sge_core/math/Rect2.h>

namespace sge {

enum class UIMouseEventType {
	None,
	Up,
	Down,
	Move,
	Scroll,
};
SGE_ENUM_ALL_OPERATOR(UIMouseEventType)

enum class UIMouseEventButton {
	None,
	Left	= 1 << 0,
	Middle	= 1 << 1,
	Right	= 1 << 2,
	Button4 = 1 << 3,
	Button5 = 1 << 4,
};
SGE_ENUM_ALL_OPERATOR(UIMouseEventButton)

enum class UIEventModifier {
	None,
	Shift	= 1 << 0,
	Ctrl	= 1 << 1,
	Atl		= 1 << 2,
	Cmd		= 1 << 3,
	Fn		= 1 << 4,
};
SGE_ENUM_ALL_OPERATOR(UIEventModifier)

struct UIMouseEvent {
	using Type		= UIMouseEventType;
	using Modifier	= UIEventModifier;
	using Button	= UIMouseEventButton;

	bool isUp() const		{ return type == Type::Up; }
	bool isDown() const		{ return type == Type::Down; }
	bool isMove() const		{ return type == Type::Move; }
	bool isScroll() const	{ return type == Type::Scroll; }

	bool isDragging() const { return type == Type::Move && pressedButtons != Button::None; }

	Type		type		= Type::None;
	Modifier	modifier	= Modifier::None;
	Button		button		= Button::None;
	Button		pressedButtons = Button::None;

	Vec2f		pos{0,0};
	Vec2f		deltaPos{0,0};
	Vec2f		scroll{0,0};
};

#define UIKeyEventType_ENUM_LIST(E) \
	E(None,) \
	E(Up,) \
	E(Down,) \
	E(Hold,) \
	E(_END,) \
//-----
SGE_ENUM_CLASS(UIKeyEventType, u8)

enum class UIKeyEventButton
{
	None					= 0x00,
	LButton					= 0x01,								/* Left mouse button */
	RButton					= 0x02,								/* Right mouse button */
	Cancel					= 0x03,								/* Control-break processing */
	MButton					= 0x04,								/* Middle mouse button (three-button mouse) */
	XButton1				= 0x05,								/* Windows 2000/XP: X1 mouse button */
	XButton2				= 0x06,								/* Windows 2000/XP: X2 mouse button */
	/* 0x07 is undefined */
	Back					= 0x08,								/* BACKSPACE key */
	Tab						= 0x09,								/* TAB key */
	/* 0x0A to 0x0B are reserved */
	Clear					= 0x0C,								/* CLEAR key */
	Return					= 0x0D,								/* ENTER key */
	/* 0x0E to 0x0F are undefined */
	Shift					= 0x10,								/* SHIFT key */
	Control					= 0x11,								/* CTRL key */
	Menu					= 0x12,								/* ALT key */
	Pause					= 0x13,								/* PAUSE key */
	Capital					= 0x14,								/* CAPS LOCK key */
	Kana					= 0x15,								/* Input Method Editor (IME) Kana mode */
	Hanguel					= 0x15,								/* IME Hanguel mode (maintained for compatibility; use HANGUL) */
	Hangul					= 0x15,								/* IME Hangul mode */
	/* 0x16 is undefined */
	Junja					= 0x17,								/* IME Junja mode */
	Final					= 0x18,								/* IME final mode */
	Hanja					= 0x19,								/* IME Hanja mode */
	Kanji					= 0x19,								/* IME Kanji mode */
	/* 0x1A is undefined */
	Escape					= 0x1B,								/* ESC key */
	Convert					= 0x1C,								/* IME convert */
	Nonconvert				= 0x1D,								/* IME nonconvert */
	Accept					= 0x1E,								/* IME accept */
	Modechange				= 0x1F,								/* IME mode change request */
	Space					= 0x20,								/* SPACEBAR */
	Prior					= 0x21,								/* PAGE UP key */
	Next					= 0x22,								/* PAGE DOWN key */
	End						= 0x23,								/* END key */
	Home					= 0x24,								/* HOME key */
	Left					= 0x25,								/* LEFT ARROW key */
	Up						= 0x26,								/* UP ARROW key */
	Right					= 0x27,								/* RIGHT ARROW key */
	Down					= 0x28,								/* DOWN ARROW key */
	Select					= 0x29,								/* SELECT key */
	Print					= 0x2A,								/* PRINT key */
	Execute					= 0x2B,								/* EXECUTE key */
	Snapshot				= 0x2C,								/* PRINT SCREEN key */
	Insert					= 0x2D,								/* INS key */
	Delete					= 0x2E,								/* DEL key */
	Help					= 0x2F,								/* HELP key */

	/* Digits, the last 4 bits of the code represent the corresponding digit */
	Key_0					= 0x30,								/* '0' key */
	Key_1					= 0x31,								/* '1' key */
	Key_2					= 0x32,								/* '2' key */
	Key_3					= 0x33,								/* '3' key */
	Key_4					= 0x34,								/* '4' key */
	Key_5					= 0x35,								/* '5' key */
	Key_6					= 0x36,								/* '6' key */
	Key_7					= 0x37,								/* '7' key */
	Key_8					= 0x38,								/* '8' key */
	Key_9					= 0x39,								/* '9' key */

	/* 0x3A to 0x40 are undefined */

	/* The alphabet, the code corresponds to the capitalized letter in the ASCII code */
	A					= 0x41,									/* 'A' key */
	B					= 0x42,									/* 'B' key */
	C					= 0x43,									/* 'C' key */
	D					= 0x44,									/* 'D' key */
	E					= 0x45,									/* 'E' key */
	F					= 0x46,									/* 'F' key */
	G					= 0x47,									/* 'G' key */
	H					= 0x48,									/* 'H' key */
	I					= 0x49,									/* 'I' key */
	J					= 0x4A,									/* 'J' key */
	K					= 0x4B,									/* 'K' key */
	L					= 0x4C,									/* 'L' key */
	M					= 0x4D,									/* 'M' key */
	N					= 0x4E,									/* 'N' key */
	O					= 0x4F,									/* 'O' key */
	P					= 0x50,									/* 'P' key */
	Q					= 0x51,									/* 'Q' key */
	R					= 0x52,									/* 'R' key */
	S					= 0x53,									/* 'S' key */
	T					= 0x54,									/* 'T' key */
	U					= 0x55,									/* 'U' key */
	V					= 0x56,									/* 'V' key */
	W					= 0x57,									/* 'W' key */
	X					= 0x58,									/* 'X' key */
	Y					= 0x59,									/* 'Y' key */
	Z					= 0x5A,									/* 'Z' key */

	LWin					= 0x5B,								/* Left Windows key (Microsoft Natural keyboard) */
	RWin					= 0x5C,								/* Right Windows key (Natural keyboard) */
	Apps					= 0x5D,								/* Applications key (Natural keyboard) */

	/* 0x5E is reserved */
	SLEEP					= 0x5F,								/* Computer Sleep key */
		
	/* Numeric keypad digits, the last four bits of the code represent the corresponding digit */
	Numpad0					= 0x60,								/* Numeric keypad '0' key */
	Numpad1					= 0x61,								/* Numeric keypad '1' key */
	Numpad2					= 0x62,								/* Numeric keypad '2' key */
	Numpad3					= 0x63,								/* Numeric keypad '3' key */
	Numpad4					= 0x64,								/* Numeric keypad '4' key */
	Numpad5					= 0x65,								/* Numeric keypad '5' key */
	Numpad6					= 0x66,								/* Numeric keypad '6' key */
	Numpad7					= 0x67,								/* Numeric keypad '7' key */
	Numpad8					= 0x68,								/* Numeric keypad '8' key */
	Numpad9					= 0x69,								/* Numeric keypad '9' key */

	/* Numeric keypad operators and special keys */
	Multiply				= 0x6A,								/* Multiply key */
	Add						= 0x6B,								/* Add key */
	Separator				= 0x6C,								/* Separator key */
	Subtract				= 0x6D,								/* Subtract key */
	Decimal					= 0x6E,								/* Decimal key */
	Divide					= 0x6F,								/* Divide key */

	/* Function keys, from F1 to F24 */
	F1						= 0x70,								/* F1 key */
	F2						= 0x71,								/* F2 key */
	F3						= 0x72,								/* F3 key */
	F4						= 0x73,								/* F4 key */
	F5						= 0x74,								/* F5 key */
	F6						= 0x75,								/* F6 key */
	F7						= 0x76,								/* F7 key */
	F8						= 0x77,								/* F8 key */
	F9						= 0x78,								/* F9 key */
	F10						= 0x79,								/* F10 key */
	F11						= 0x7A,								/* F11 key */
	F12						= 0x7B,								/* F12 key */
	F13						= 0x7C,								/* F13 key */
	F14						= 0x7D,								/* F14 key */
	F15						= 0x7E,								/* F15 key */
	F16						= 0x7F,								/* F16 key */
	F17						= 0x80,								/* F17 key */
	F18						= 0x81,								/* F18 key */
	F19						= 0x82,								/* F19 key */
	F20						= 0x83,								/* F20 key */
	F21						= 0x84,								/* F21 key */
	F22						= 0x85,								/* F22 key */
	F23						= 0x86,								/* F23 key */
	F24						= 0x87,								/* F24 key */

	/* 0x88 to 0x8F are unassigned */
	NumLock					= 0x90,								/* NUM LOCK key */
	Scroll					= 0x91,								/* SCROLL LOCK key */

	/* 0x92 to 0x96 are OEM specific */
	/* 0x97 to 0x9F are unassigned */

	/* Modifier keys */
	LShift					= 0xA0,								/* Left SHIFT key */
	RShift					= 0xA1,								/* Right SHIFT key */
	LControl				= 0xA2,								/* Left CONTROL key */
	RControl				= 0xA3,								/* Right CONTROL key */
	LMenu					= 0xA4,								/* Left MENU key */
	RMenu					= 0xA5,								/* Right MENU key */

	/* Browser related keys */
	Browser_Back			= 0xA6,								/* Windows 2000/XP: Browser Back key */
	Browser_Forward			= 0xA7,								/* Windows 2000/XP: Browser Forward key */
	Browser_Refresh			= 0xA8,								/* Windows 2000/XP: Browser Refresh key */
	Browser_Stop			= 0xA9,								/* Windows 2000/XP: Browser Stop key */
	Browser_Search			= 0xAA,								/* Windows 2000/XP: Browser Search key */
	Browser_Favorites		= 0xAB,								/* Windows 2000/XP: Browser Favorites key */
	Browser_Home			= 0xAC,								/* Windows 2000/XP: Browser Start and Home key */

	/* Volume related keys */
	Volume_Mute				= 0xAD,								/* Windows 2000/XP: Volume Mute key */
	Volume_Down				= 0xAE,								/* Windows 2000/XP: Volume Down key */
	Volume_Up				= 0xAF,								/* Windows 2000/XP: Volume Up key */

	/* Media player related keys */
	Media_Next_Track		= 0xB0,								/* Windows 2000/XP: Next Track key */
	Media_Prev_Track		= 0xB1,								/* Windows 2000/XP: Previous Track key */
	Media_Stop				= 0xB2,								/* Windows 2000/XP: Stop Media key */
	Media_Play_Pause		= 0xB3,								/* Windows 2000/XP: Play/Pause Media key */

	/* Application launcher keys */
	Launch_Mail				= 0xB4,								/* Windows 2000/XP: Start Mail key */
	Media_Select			= 0xB5,								/* Windows 2000/XP: Select Media key */
	Launch_App1				= 0xB6,								/* Windows 2000/XP: Start Application 1 key */
	Launch_App2				= 0xB7,								/* Windows 2000/XP: Start Application 2 key */

	/* 0xB8 and 0xB9 are reserved */

	/* OEM keys */
	Oem_1					= 0xBA,								/* Used for miscellaneous characters; it can vary by keyboard. */
																/* Windows 2000/XP: For the US standard keyboard, the ';:' key */
	Oem_Plus				= 0xBB,								/* Windows 2000/XP: For any country/region, the '+' key */
	Oem_Comma				= 0xBC,								/* Windows 2000/XP: For any country/region, the ',' key */
	Oem_Minus				= 0xBD,								/* Windows 2000/XP: For any country/region, the '-' key */
	Oem_Period				= 0xBE,								/* Windows 2000/XP: For any country/region, the '.' key */
	Oem_2					= 0xBF,								/* Used for miscellaneous characters; it can vary by keyboard. */
																/* Windows 2000/XP: For the US standard keyboard, the '/?' key */
	Oem_3					= 0xC0,								/* Used for miscellaneous characters; it can vary by keyboard. */
																/* Windows 2000/XP: For the US standard keyboard, the '`~' key */

	/* 0xC1 to 0xD7 are reserved */
	ABNT_C1					= 0xC1,								/* Brazilian (ABNT) Keyboard */
	ABNT_C2					= 0xC2,								/* Brazilian (ABNT) Keyboard */
	/* 0xD8 to 0xDA are unassigned */
	Oem_4					= 0xDB,								/* Used for miscellaneous characters; it can vary by keyboard. */
																/* Windows 2000/XP: For the US standard keyboard, the '[{' key */
	Oem_5					= 0xDC,								/* Used for miscellaneous characters; it can vary by keyboard. */
																/* Windows 2000/XP: For the US standard keyboard, the '\|' key */
	Oem_6					= 0xDD,								/* Used for miscellaneous characters; it can vary by keyboard. */
																/* Windows 2000/XP: For the US standard keyboard, the ']}' key */
	Oem_7					= 0xDE,								/* Used for miscellaneous characters; it can vary by keyboard. */
																/* Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key */
	Oem_8					= 0xDF,								/* Used for miscellaneous characters; it can vary by keyboard. */
	/* 0xE0 is reserved */
	/* 0xE1 is OEM specific */
	Oem_102					= 0xE2,								/* Windows 2000/XP: Either the angle bracket key or */
																/* the backslash key on the RT 102-key keyboard */
	/* 0xE3 and 0xE4 are OEM specific */
	ProcessKey				= 0xE5,								/* Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key */
	/* 0xE6 is OEM specific */
	Packet					= 0xE7,								/* Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. */
																/* The PACKET key is the low word of a 32-bit Virtual Key value used */
																/* for non-keyboard input methods. For more information, */
																/* see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP */
	/* 0xE8 is unassigned */

	/* 0xE9 to 0xF5 are OEM specific */
	Attn					= 0xF6,								/* Attn key */
	Crsel					= 0xF7,								/* CrSel key */
	Exsel					= 0xF8,								/* ExSel key */
	Ereof					= 0xF9,								/* Erase EOF key */
	Play					= 0xFA,								/* Play key */
	Zoom					= 0xFB,								/* Zoom key */
	NoName					= 0xFC,								/* Reserved */
	PA1						= 0xFD,								/* PA1 key */
	Oem_Clear				= 0xFE,								/* Clear key */

	_COUNT					= 0xFF,
};

struct UIKeyboardEvent
{
	using Type		= UIKeyEventType;
	using Modifier	= UIEventModifier;
	using Button	= UIKeyEventButton;

	bool isUp() const		{ return type == Type::Up; }
	bool isDown() const		{ return type == Type::Down; }
	bool isHold() const		{ return type == Type::Hold; }

	bool isUp(Button key_) const		{ return button == key_ && type == Type::Up; }
	bool isDown(Button key_) const		{ return button == key_ && type == Type::Down; }
	bool isHold(Button key_) const		{ return button == key_ && type == Type::Hold; }


	Type		type		= Type::None;
	Modifier	modifier	= Modifier::None;
	Button		button		= Button::None;
};
using KeyButton = UIKeyEventButton;


}