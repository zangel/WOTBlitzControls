#pragma once

#include "Config.hpp"

namespace wotbcontrols
{
namespace srv
{
	class Application
	{
	protected:
		Application();

	public:
		virtual ~Application();

		static Application& 				instance();

		std::error_code 					initialize(int argc, const char * argv[]);
		void								run();
		void								shutdown();

	private:
		std::error_code 					initializeInputDevice();
		void								shutdownInputDevice();

		std::error_code 					initializeInputMapping();
		void								shutdownInputMapping();

		std::error_code 					initializeNetowrk();
		void								shutdownNetwork();


		void								doAccept();
		void								doRead();
		void								handleRead(system::error_code ec, std::size_t bytes_transferred);

		void								onConnected();
		void								onDisconnected();
		void								handleMessage();
		void								handleKeyboarMessage();
		void								handleMouseButtonMessage();
		void								handleMouseMovedMessage();

		void								doTouch(std::uint32_t slot, Vec2i pos);
		void								doDirectionPad();
		void								doCameraMove();
		void								doLookoutMove();

		void								sendInputEvent(std::uint16_t type, std::uint16_t code, std::uint32_t value = 0x0000000);

	private:
		asio::io_service					m_IOS;
		asio::io_service::work 				m_Work;

		asio::ip::address					m_ListenAddress;
		std::uint16_t 						m_ListenPort;

		asio::ip::tcp::acceptor 			m_Acceptor;
		asio::ip::tcp::socket 				m_Socket;
		asio::posix::stream_descriptor		m_InputDevice;
		Vec2i								m_TouchscreenSize;
		Vec2i								m_DPadCenter;
		int									m_DPadRadius;
		Rect2i								m_LookoutArea;

		std::unordered_map<uint8_t, Vec2i>	m_ButtonMapping;
		Message								m_Message;

		Vec2i								m_Direction;

		bool								m_CameraSlotDown;
		Vec2i								m_CameraPos;

		bool								m_LookoutSlotDown;
		Vec2i								m_LookoutPos;


	};

} //namespace srv
} //namespace wotbcontrols
