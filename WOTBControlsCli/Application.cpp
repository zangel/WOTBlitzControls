#include "Application.hpp"
#include <iostream>

namespace wotbcontrols
{
namespace cli
{
	Application::Application()
		: m_EventTap(nullptr)
		, m_RunLoopSource(nullptr)
		, m_ControlsEnabled(false)
		, m_ControlsAiming(false)
		, m_DisplayRect()
		, m_MouseLockPos()
		, m_IOS()
		, m_Socket(m_IOS)
	{
	}

	Application::~Application()
	{

	}

	Application& Application::instance()
	{
		static Application _instance;
		return _instance;
	}

	std::error_code Application::initialize(int argc, const char * argv[])
	{
		if(std::error_code ine = initializeNetwork())
		{
			return ine;
		}

		CGEventMask eventMask =
			CGEventMaskBit(kCGEventKeyDown)				|
			CGEventMaskBit(kCGEventKeyUp)				|
			CGEventMaskBit(kCGEventLeftMouseDown)		|
			CGEventMaskBit(kCGEventLeftMouseUp)			|
			CGEventMaskBit(kCGEventRightMouseDown)		|
			CGEventMaskBit(kCGEventRightMouseUp)		|
			CGEventMaskBit(kCGEventMouseMoved)			|
			CGEventMaskBit(kCGEventFlagsChanged)		|
			CGEventMaskBit(kCGEventScrollWheel);

		m_EventTap = CGEventTapCreate(
			kCGSessionEventTap,
			kCGHeadInsertEventTap,
			kCGEventTapOptionDefault,
			eventMask,
			&_handleCGEvent,
			nullptr
		);

		if(!m_EventTap)
		{
			return std::make_error_code(std::errc::io_error);
		}

		m_RunLoopSource = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, m_EventTap, 0);

		if(!m_RunLoopSource)
		{
			return std::make_error_code(std::errc::io_error);
		}

		CFRunLoopAddSource(CFRunLoopGetCurrent(), m_RunLoopSource, kCFRunLoopCommonModes);
		
		CGEventTapEnable(m_EventTap, true);

		m_DisplayRect = CGDisplayBounds(CGMainDisplayID());
		return std::error_code();
	}

	void Application::run()
	{
		CFRunLoopRun();
	}

	void Application::shutdown()
	{
		CGEventTapEnable(m_EventTap, false);
		shutdownNetwork();
	}

	std::error_code Application::initializeNetwork()
	{
		asio::ip::tcp::resolver resolver(m_IOS);

		system::error_code ec;
		asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(
			asio::ip::tcp::resolver::query(std::to_string(DefaultPort)),
			ec
		);

		if(ec)
		{
			return std::make_error_code(std::errc::io_error);
		}

		asio::connect(m_Socket, endpoint, ec);
		if(ec)
		{
			return std::make_error_code(std::errc::io_error);
		}
		return std::error_code();
	}

	void Application::shutdownNetwork()
	{
		m_Socket.close();
	}

	void Application::sendKeyboardMessage(bool down, CGKeyCode keycode, CGEventFlags modifiers)
	{
		Message msg;
		msg.type = kMessageTypeKeyboard;
		msg.data.keybaord.down = down ? 1 : 0;
		msg.data.keybaord.key = keycode;
		msg.data.keybaord.modifiers = modifiers &
			(
				KB_SHIFT_MODIFIER 		|
			 	KB_CONTROL_MODIFIER 	|
			 	KB_ALT_MODIFIER			|
			 	KB_COMMAND_MODIFIER
			);

		system::error_code ec;
		asio::write(m_Socket, asio::buffer(&msg, MessageSize), ec);
		if(ec)
		{

		}
	}

	void Application::sendMouseMovedMessage(int64_t dX, int64_t dY, CGEventFlags modifiers)
	{
		Message msg;
		msg.type = kMessageTypeMouseMoved;
		msg.data.mouseMoved.deltaX = (int32_t)dX;
		msg.data.mouseMoved.deltaY = (int32_t)dY;
		msg.data.mouseMoved.modifiers = modifiers &
			(
		 		KB_SHIFT_MODIFIER 		|
		 		KB_CONTROL_MODIFIER 	|
		 		KB_ALT_MODIFIER			|
		 		KB_COMMAND_MODIFIER
			);

		system::error_code ec;
		asio::write(m_Socket, asio::buffer(&msg, MessageSize), ec);
		if(ec)
		{

		}
	}

	void Application::sendMouseButtonMessage(CGEventFlags modifiers)
	{
		Message msg;
		msg.type = kMessageTypeMouseButton;
		msg.data.mouseButton.modifiers = modifiers &
			(
		 		KB_SHIFT_MODIFIER 		|
		 		KB_CONTROL_MODIFIER 	|
		 		KB_ALT_MODIFIER			|
		 		KB_COMMAND_MODIFIER
			);

		system::error_code ec;
		asio::write(m_Socket, asio::buffer(&msg, MessageSize), ec);
		if(ec)
		{

		}

	}

	void Application::enterAiming()
	{
		CGDisplayHideCursor(kCGDirectMainDisplay);
		CGWarpMouseCursorPosition(m_MouseLockPos);
		m_ControlsAiming = true;

		std::cout << "aiming!" << std::endl;
	}

	void Application::leaveAiming()
	{
		CGDisplayShowCursor(kCGDirectMainDisplay);
		m_ControlsAiming = false;
		std::cout << "not aiming!" << std::endl;
	}

	CGEventRef Application::handleCGEvent(CGEventTapProxy proxy, CGEventType type, CGEventRef event)
	{
		
		if(m_ControlsEnabled)
			return handleCGEventGameControls(proxy, type, event);
		else
			return handleCGEventIdle(proxy, type, event);
	}

	CGEventRef Application::handleCGEventIdle(CGEventTapProxy proxy, CGEventType type, CGEventRef event)
	{
		if(type == kCGEventKeyDown)
		{
			CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
			CGEventFlags modifiers = CGEventGetFlags(event);

			if(keycode == 0x2e && (modifiers & kCGEventFlagMaskCommand))
			{
				m_ControlsEnabled = true;
				m_ControlsAiming = false;
				std::cout << "controls enabled!" << std::endl;
				return nullptr;
			}

			//std::cout << "key: " << keycode << "|" << std::hex << modifiers << std::endl;
		}
		else
		if(type == kCGEventFlagsChanged)
		{
			CGKeyCode keycode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
			CGEventFlags modifiers = CGEventGetFlags(event);
			//std::cout << "key: " << keycode << "|" << std::hex << modifiers << std::endl;
		}
		return event;
	}

	CGEventRef Application::handleCGEventGameControls(CGEventTapProxy proxy, CGEventType type, CGEventRef event)
	{
		if(type == kCGEventKeyDown)
		{
			CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
			CGEventFlags modifiers = CGEventGetFlags(event);

			if(keycode == 0x2e && (modifiers & kCGEventFlagMaskCommand))
			{
				m_ControlsEnabled = false;
				std::cout << "controls disabled!" << std::endl;
				return nullptr;
			}

			sendKeyboardMessage(true, keycode, modifiers);
			return nullptr;
		}
		else
		if(type == kCGEventKeyUp)
		{
			CGKeyCode keycode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
			CGEventFlags modifiers = CGEventGetFlags(event);

			if(keycode == 0x2e && (modifiers & kCGEventFlagMaskCommand))
			{
				return nullptr;
			}
			sendKeyboardMessage(false, keycode, modifiers);
		}
		else
		if(type == kCGEventFlagsChanged)
		{
			CGKeyCode keycode = (CGKeyCode) CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
			CGEventFlags modifiers = CGEventGetFlags(event);

			if(keycode == KEY_LSHIFT || keycode == KEY_RSHIFT)
			{
				sendKeyboardMessage(modifiers & KB_SHIFT_MODIFIER, keycode, modifiers);
			}
			else
			if(keycode == KEY_LCONTROL || keycode == KEY_RCONTROL)
			{
				sendKeyboardMessage(modifiers & KB_CONTROL_MODIFIER, keycode, modifiers);
			}
			else
			if(keycode == KEY_COMMAND)
			{
				sendKeyboardMessage(modifiers & KB_COMMAND_MODIFIER, keycode, modifiers);
			}

		}
		else
		if(type == kCGEventRightMouseDown)
		{

			if(m_ControlsAiming)
			{
				leaveAiming();
				return nullptr;
			}
			else
			{
				m_MouseLockPos = CGEventGetLocation(event);
				enterAiming();
			}
		}
		else
		if(type == kCGEventLeftMouseDown)
		{
			if(m_ControlsAiming)
			{
				CGEventFlags modifiers = CGEventGetFlags(event);
				sendMouseButtonMessage(modifiers);
			}
		}
		else
		if(type == kCGEventMouseMoved)
		{
			if(m_ControlsAiming)
			{
				CGEventFlags modifiers = CGEventGetFlags(event);
				auto dX = CGEventGetIntegerValueField(event, kCGMouseEventDeltaX);
				auto dY = CGEventGetIntegerValueField(event, kCGMouseEventDeltaY);

				if(dX || dY)
				{
					//std::cout << "delta: (" << dX << "," << dY << ")" << std::endl;
					CGWarpMouseCursorPosition(m_MouseLockPos);
					sendMouseMovedMessage(dX, dY, modifiers);
				}
			}
		}

		return m_ControlsAiming ? nullptr : event;
	}

	CGEventRef Application::_handleCGEvent(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon)
	{
		return Application::instance().handleCGEvent(proxy, type, event);
	}


} //namespace cli
} //namespace wotbcontrols
