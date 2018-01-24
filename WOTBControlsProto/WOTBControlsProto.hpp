#pragma once

#include <cstdint>

namespace wotbcontrols
{
	#pragma pack(1)
	enum MessageType
	{
		kMessageTypeInvalid = 0,
		kMessageTypeKeyboard,
		kMessageTypeMouseMoved,
		kMessageTypeMouseButton
	};

	struct MessageKeyboard
	{
		std::uint8_t			down;
		std::uint8_t			key;
		std::uint32_t			modifiers;
	};

	struct MessageMouseMoved
	{
		std::int32_t			deltaX;
		std::int32_t			deltaY;
		std::uint32_t			modifiers;
	};

	struct MessageMouseButton
	{
		std::uint32_t			modifiers;
	};

	struct Message
	{
		std::uint32_t 			type;
		union
		{
			MessageKeyboard 	keybaord;
			MessageMouseMoved	mouseMoved;
			MessageMouseButton	mouseButton;
		} data;
	};
	std::size_t const MessageSize  = sizeof(Message);
	#pragma pack()

	std::uint32_t const KB_SHIFT_MODIFIER			= 0x00020000;
	std::uint32_t const KB_CONTROL_MODIFIER			= 0x00040000;
	std::uint32_t const KB_ALT_MODIFIER				= 0x00080000;
	std::uint32_t const KB_COMMAND_MODIFIER			= 0x00100000;

	std::uint8_t const KEY_1						= 0x12;
	std::uint8_t const KEY_2						= 0x13;
	std::uint8_t const KEY_3						= 0x14;
	std::uint8_t const KEY_4						= 0x15;
	std::uint8_t const KEY_5						= 0x17;
	std::uint8_t const KEY_6						= 0x16;
	std::uint8_t const KEY_7						= 0x1a;
	std::uint8_t const KEY_8						= 0x1c;
	std::uint8_t const KEY_9						= 0x19;

	std::uint8_t const KEY_SQR						= 0x32;

	std::uint8_t const KEY_F1						= 0x7a;
	std::uint8_t const KEY_F2						= 0x78;
	std::uint8_t const KEY_F3						= 0x63;
	std::uint8_t const KEY_F4						= 0x76;
	std::uint8_t const KEY_F5						= 0x60;
	std::uint8_t const KEY_F6						= 0x61;
	std::uint8_t const KEY_F7						= 0x62;
	std::uint8_t const KEY_F8						= 0x64;

	std::uint8_t const KEY_SPACE					= 0x31;

	std::uint8_t const KEY_A						= 0x00;
	std::uint8_t const KEY_S						= 0x01;
	std::uint8_t const KEY_D						= 0x02;
	std::uint8_t const KEY_W						= 0x0d;

	std::uint8_t const KEY_M						= 0x2e;
	std::uint8_t const KEY_T						= 0x11;
	std::uint8_t const KEY_Z						= 0x06;

	std::uint8_t const KEY_F						= 0x03;
	std::uint8_t const KEY_R						= 0x0f;

	std::uint8_t const KEY_LSHIFT					= 0x38;
	std::uint8_t const KEY_LCONTROL					= 0x3b;
	std::uint8_t const KEY_LALT						= 0x3a;

	std::uint8_t const KEY_RSHIFT					= 0x3c;
	std::uint8_t const KEY_RCONTROL					= 0x3e;
	std::uint8_t const KEY_RALT						= 0x3d;

	std::uint8_t const KEY_COMMAND					= 0x37;


	std::uint16_t const DefaultPort 				= 3562;



} //namespace wotbcontrols
