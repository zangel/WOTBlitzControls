#pragma once

#include "Config.hpp"

namespace wotbcontrols
{
namespace srv
{
	std::uint16_t const EV_ABS 					= 0x0003;
	std::uint16_t const EV_SYN 					= 0x0000;

	std::uint16_t const SYN_REPORT				= 0x0000;


	std::uint16_t const ABS_MT_SLOT				= 0x002f;
	std::uint16_t const ABS_MT_TOUCH_MAJOR		= 0x0030;    /* Major axis of touching ellipse */
	std::uint16_t const ABS_MT_TOUCH_MINOR		= 0x0031;    /* Minor axis (omit if circular) */
	std::uint16_t const ABS_MT_WIDTH_MAJOR		= 0x0032;    /* Major axis of approaching ellipse */
	std::uint16_t const ABS_MT_WIDTH_MINOR		= 0x0033;    /* Minor axis (omit if circular) */
	std::uint16_t const ABS_MT_ORIENTATION		= 0x0034;    /* Ellipse orientation */
	std::uint16_t const ABS_MT_POSITION_X		= 0x0035;    /* Center X touch position */
	std::uint16_t const ABS_MT_POSITION_Y		= 0x0036;    /* Center Y touch position */
	std::uint16_t const ABS_MT_TOOL_TYPE		= 0x0037;    /* Type of touching device */
	std::uint16_t const ABS_MT_BLOB_ID			= 0x0038;    /* Group a set of packets as a blob */
	std::uint16_t const ABS_MT_TRACKING_ID		= 0x0039;    /* Unique ID of initiated contact */
	std::uint16_t const ABS_MT_PRESSURE			= 0x003a;    /* Pressure on contact area */
	std::uint16_t const ABS_MT_DISTANCE			= 0x003b;    /* Contact hover distance */
	std::uint16_t const ABS_MT_TOOL_X			= 0x003c;    /* Center X tool position */
	std::uint16_t const ABS_MT_TOOL_Y			= 0x003d;    /* Center Y tool position */

	std::uint32_t const SLOT_BUTTONS			= 0x00000000;
	std::uint32_t const SLOT_DIRECTION			= 0x00000001;
	std::uint32_t const SLOT_CAMERA				= 0x00000002;
	std::uint32_t const SLOT_LOOKOUT			= 0x00000003;

	std::uint32_t const DEFAULT_TOUCH_MAJOR		= 0x00000020;
	std::uint32_t const DEFAULT_PRESSURE		= 0x00000081;
	std::uint32_t const TRACKING_ID_NONE		= 0xffffffff;




	#pragma pack(1)
	struct input_event
	{
		timeval				time;

		std::uint16_t		type;
		std::uint16_t		code;
		std::uint32_t		value;
	};
	#pragma pack()




} //namespace srv
} //namespace wotbcontrols
