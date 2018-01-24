#pragma once

#include "Config.hpp"

namespace wotbcontrols
{
namespace cli
{
	class Application
	{
	protected:
		Application();

	public:
		virtual ~Application();

		static Application& 			instance();

		std::error_code 				initialize(int argc, const char * argv[]);
		void							run();
		void							shutdown();

	private:
		std::error_code 				initializeNetwork();
		void							shutdownNetwork();

		void							sendKeyboardMessage(bool down, CGKeyCode keycode, CGEventFlags modifiers);
		void							sendMouseMovedMessage(int64_t dX, int64_t dY, CGEventFlags modifiers);
		void							sendMouseButtonMessage(CGEventFlags modifiers);

		void							enterAiming();
		void							leaveAiming();

		CGEventRef						handleCGEvent(CGEventTapProxy proxy, CGEventType type, CGEventRef event);
		CGEventRef						handleCGEventIdle(CGEventTapProxy proxy, CGEventType type, CGEventRef event);
		CGEventRef						handleCGEventGameControls(CGEventTapProxy proxy, CGEventType type, CGEventRef event);



		static CGEventRef				_handleCGEvent(CGEventTapProxy proxy, CGEventType type, CGEventRef event, void *refcon);

	private:
		CFMachPortRef      				m_EventTap;
		CFRunLoopSourceRef				m_RunLoopSource;
		bool							m_ControlsEnabled;
		bool							m_ControlsAiming;
		CGRect							m_DisplayRect;
		CGPoint							m_MouseLockPos;

		asio::io_service				m_IOS;
		asio::ip::tcp::socket 			m_Socket;

	};

} //namespace cli
} //namespace wotbcontrols
