#include "InputEvent.hpp"
#include "Application.hpp"

#include <iostream>


namespace wotbcontrols
{
namespace srv
{
	Application::Application()
		: m_IOS()
		, m_Work(m_IOS)
		, m_ListenAddress(asio::ip::address_v4::any())
		, m_ListenPort(DefaultPort)
		, m_Acceptor(m_IOS)
		, m_Socket(m_IOS)
		, m_InputDevice(m_IOS)
		, m_TouchscreenSize(1280, 720)
		, m_DPadCenter(88, 640)
		, m_DPadRadius(60)
		, m_LookoutArea(Vec2i(845,666), Vec2i(1014,710))
		, m_ButtonMapping()
		, m_Message()
		, m_Direction(0, 0)
		, m_CameraSlotDown(false)
		, m_CameraPos(0,0)
		, m_LookoutSlotDown(false)
		, m_LookoutPos(0,0)
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
		if(auto iide = initializeInputDevice())
		{
			return iide;
		}

		if(auto iime = initializeInputMapping())
		{
			return iime;
		}

		if(auto ine = initializeNetowrk())
		{
			shutdownInputMapping();
			shutdownInputDevice();
			return ine;
		}
		return std::error_code();
	}

	void Application::run()
	{
		m_IOS.run();
	}

	void Application::shutdown()
	{
		shutdownNetwork();
		shutdownInputMapping();
		shutdownInputDevice();
	}

	std::error_code Application::initializeInputDevice()
	{
#if !defined(__APPLE__)
		int idev = open("/dev/input/event1", O_RDWR);
		if(idev < 0)
			return std::make_error_code(std::errc::io_error);

		m_InputDevice.assign(idev);
#endif
		return std::error_code();
	}

	void Application::shutdownInputDevice()
	{
#if !defined(__APPLE__)
		m_InputDevice.close();
#endif
	}

	std::error_code Application::initializeInputMapping()
	{
		m_ButtonMapping[KEY_LSHIFT] = Vec2i(831, 609);
		m_ButtonMapping[KEY_SQR] = Vec2i(12, 12);
		m_ButtonMapping[KEY_Z] = Vec2i(198, 696);
		m_ButtonMapping[KEY_T] = Vec2i(1159, 24);
		m_ButtonMapping[KEY_F4] = Vec2i(1256, 192);
		m_ButtonMapping[KEY_F3] = Vec2i(1210, 192);
		m_ButtonMapping[KEY_F2] = Vec2i(1164, 192);
		m_ButtonMapping[KEY_F1] = Vec2i(1116, 192);
		m_ButtonMapping[KEY_1] = Vec2i(25, 302);
		m_ButtonMapping[KEY_2] = Vec2i(25, 250);
		m_ButtonMapping[KEY_3] = Vec2i(25, 201);
		m_ButtonMapping[KEY_4] = Vec2i(1226, 590);
		m_ButtonMapping[KEY_5] = Vec2i(1149, 666);
		m_ButtonMapping[KEY_6] = Vec2i(1156, 534);
		m_ButtonMapping[KEY_7] = Vec2i(1094, 596);
		m_ButtonMapping[KEY_8] = Vec2i(1236, 501);
		m_ButtonMapping[KEY_9] = Vec2i(1061, 677);
		m_ButtonMapping[KEY_SPACE] = Vec2i(1253, 688);
		m_ButtonMapping[KEY_F] = Vec2i(761, 603);

		return std::error_code();
	}

	void Application::shutdownInputMapping()
	{

	}


	std::error_code Application::initializeNetowrk()
	{
		system::error_code ec;

		asio::ip::tcp::resolver resolver(m_IOS);

		asio::ip::tcp::endpoint endpoint = *resolver.resolve({m_ListenAddress, m_ListenPort}, ec);
		if(ec)
		{
			return std::make_error_code(std::errc::io_error);
		}

		m_Acceptor.open(endpoint.protocol(), ec);
		if(ec)
		{
			return std::make_error_code(std::errc::io_error);

		}

		m_Acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
		m_Acceptor.bind(endpoint, ec);
		if(ec)
		{
			return std::make_error_code(std::errc::io_error);
		}

		m_Acceptor.listen(1, ec);
		if(ec)
		{
			return std::make_error_code(std::errc::io_error);
		}
		
		doAccept();

		return std::error_code();
	}

	void Application::shutdownNetwork()
	{
		system::error_code ignored_ec;
		m_Acceptor.close(ignored_ec);
		m_Socket.close(ignored_ec);
	}



	void Application::doAccept()
	{
		m_Acceptor.async_accept(
			m_Socket,
			[this](system::error_code ec)
			{
				if(!m_Acceptor.is_open())
				{
					return;
				}

				if(ec)
				{
					doAccept();
				}
				else
				{
					onConnected();
					doRead();
				}
			}
		);
	}

	void Application::doRead()
	{
		m_Socket.async_read_some(
			asio::buffer(&m_Message, MessageSize),
			boost::bind(
				&Application::handleRead,
				this,
				asio::placeholders::error(),
				asio::placeholders::bytes_transferred()
			)
		);
	}

	void Application::onConnected()
	{
		std::cout << "connected!" << std::endl;
	}

	void Application::onDisconnected()
	{
		std::cout << "disconnected!" << std::endl;
	}

	void Application::handleRead(system::error_code ec, std::size_t bytes_transferred)
	{
		if(!ec)
		{
			//here handle the message
			if(bytes_transferred == MessageSize)
			{
				handleMessage();
			}
			doRead();
		}
		else
		if(ec == boost::asio::error::eof)
		{
			system::error_code ignored_ec;
			m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			m_Socket.close(ignored_ec);
			onDisconnected();
			doAccept();
		}
		else
		if(ec != boost::asio::error::operation_aborted)
		{
			system::error_code ignored_ec;
			m_Socket.close(ignored_ec);
			onDisconnected();
			doAccept();
		}
	}

	void Application::handleMessage()
	{
		if(m_Message.type == kMessageTypeKeyboard)
		{
			handleKeyboarMessage();
		}
		else
		if(m_Message.type == kMessageTypeMouseButton)
		{
			handleMouseButtonMessage();
		}
		else
		if(m_Message.type == kMessageTypeMouseMoved)
		{
			handleMouseMovedMessage();
		}
	}

	void Application::handleKeyboarMessage()
	{
		switch(m_Message.data.keybaord.key)
		{
		case KEY_LSHIFT:
		case KEY_SQR:
		case KEY_Z:
		case KEY_T:
		case KEY_F4:
		case KEY_F3:
		case KEY_F2:
		case KEY_F1:
		case KEY_1:
		case KEY_2:
		case KEY_3:
		case KEY_4:
		case KEY_5:
		case KEY_6:
		case KEY_7:
		case KEY_8:
		case KEY_9:
		case KEY_SPACE:
			{
				if(m_Message.data.keybaord.down)
				{
					doTouch(SLOT_BUTTONS, m_ButtonMapping[m_Message.data.keybaord.key]);
				}
			}
			break;

		case KEY_A:
		case KEY_S:
		case KEY_D:
		case KEY_W:
			{
				doDirectionPad();
			}
			break;

		case KEY_R:
			{
				sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_CAMERA);
				sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
				sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
				sendInputEvent(EV_SYN, SYN_REPORT);
				m_CameraSlotDown = false;

				sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_LOOKOUT);
				sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
				sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
				sendInputEvent(EV_SYN, SYN_REPORT);
				m_LookoutSlotDown = false;
			}
			break;
		}
	}

	void Application::handleMouseButtonMessage()
	{
		doTouch(SLOT_BUTTONS, m_ButtonMapping[KEY_F]);
	}

	void Application::handleMouseMovedMessage()
	{
		if(m_Message.data.mouseMoved.modifiers & KB_CONTROL_MODIFIER)
		{
			doLookoutMove();
		}
		else
		{
			doCameraMove();
		}
	}


	void Application::doTouch(std::uint32_t slot, Vec2i pos)
	{
		//std::cout << "doTouch(" << slot << ",(" << pos(0) << "," << pos(1) << "))" << std::endl;

		Array2f const scale = Array2f(0x00007fff, 0x00007fff) / m_TouchscreenSize.array().cast<float>();
		Array2i const posScaled = (pos.array().cast<float>() * scale).round().cast<int>();


		sendInputEvent(EV_ABS, ABS_MT_SLOT, slot);
		sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, slot);
		sendInputEvent(EV_ABS, ABS_MT_TOUCH_MAJOR, DEFAULT_TOUCH_MAJOR);
		sendInputEvent(EV_ABS, ABS_MT_PRESSURE, DEFAULT_PRESSURE);
		sendInputEvent(EV_ABS, ABS_MT_POSITION_X, posScaled(0));
		sendInputEvent(EV_ABS, ABS_MT_POSITION_Y, posScaled(1));
		sendInputEvent(EV_SYN, SYN_REPORT);

		sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
		sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
		sendInputEvent(EV_SYN, SYN_REPORT);
	}



	void Application::doDirectionPad()
	{
		bool const wasAnyPressed = m_Direction(0) || m_Direction(1);

		switch(m_Message.data.keybaord.key)
		{
			case KEY_A: m_Direction(0) -= (m_Message.data.keybaord.down ? 1 : -1); break;
			case KEY_D: m_Direction(0) += (m_Message.data.keybaord.down ? 1 : -1); break;
			case KEY_W: m_Direction(1) -= (m_Message.data.keybaord.down ? 1 : -1); break;
			case KEY_S: m_Direction(1) += (m_Message.data.keybaord.down ? 1 : -1); break;
		}

		m_Direction(0) = std::max(-1, std::min(1, m_Direction(0)));
		m_Direction(1) = std::max(-1, std::min(1, m_Direction(1)));

		bool const isAnyPressed = m_Direction(0) || m_Direction(1);

		Vec2f dir(m_Direction(0), m_Direction(1));
		dir.normalize();
		dir *= m_DPadRadius;
		dir += m_DPadCenter.cast<float>();

		Array2f const scale = Array2f(0x00007fff, 0x00007fff) / m_TouchscreenSize.array().cast<float>();
		Array2i const dirScaled = (dir.array() * scale).round().cast<int>();

		if(!wasAnyPressed && isAnyPressed)
		{
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_DIRECTION);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, SLOT_DIRECTION);
			sendInputEvent(EV_ABS, ABS_MT_TOUCH_MAJOR, DEFAULT_TOUCH_MAJOR);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE, DEFAULT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_POSITION_X, dirScaled(0));
			sendInputEvent(EV_ABS, ABS_MT_POSITION_Y, dirScaled(1));
			sendInputEvent(EV_SYN, SYN_REPORT);
		}
		else
		if(wasAnyPressed && !isAnyPressed)
		{
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_DIRECTION);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
			sendInputEvent(EV_SYN, SYN_REPORT);
		}
		else
		{
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_DIRECTION);
			sendInputEvent(EV_ABS, ABS_MT_POSITION_X, dirScaled(0));
			sendInputEvent(EV_ABS, ABS_MT_POSITION_Y, dirScaled(1));
			sendInputEvent(EV_SYN, SYN_REPORT);
		}
	}

	void Application::doCameraMove()
	{
		Array2f const scale = Array2f(0x00007fff, 0x00007fff) / m_TouchscreenSize.array().cast<float>();
		Rect2i const bounds = Rect2i(Vec2i(0,0), m_TouchscreenSize);

		if(m_LookoutSlotDown)
		{
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_LOOKOUT);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
			sendInputEvent(EV_SYN, SYN_REPORT);
			m_LookoutSlotDown = false;
		}


		if(!m_CameraSlotDown)
		{
			m_CameraPos = Vec2i(m_TouchscreenSize(0)/2, m_TouchscreenSize(1)/2);
			m_CameraSlotDown = true;

			Array2i const cameraPosScaled = (m_CameraPos.cast<float>().array() * scale).round().cast<int>();

			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_CAMERA);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, SLOT_CAMERA);
			sendInputEvent(EV_ABS, ABS_MT_TOUCH_MAJOR, DEFAULT_TOUCH_MAJOR);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE, DEFAULT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_POSITION_X, cameraPosScaled(0));
			sendInputEvent(EV_ABS, ABS_MT_POSITION_Y, cameraPosScaled(1));
			sendInputEvent(EV_SYN, SYN_REPORT);
		}

		m_CameraPos += Vec2i(m_Message.data.mouseMoved.deltaX, m_Message.data.mouseMoved.deltaY);

		{
			Array2i const cameraPosScaled = (m_CameraPos.cast<float>().array() * scale).round().cast<int>();
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_CAMERA);
			sendInputEvent(EV_ABS, ABS_MT_POSITION_X, cameraPosScaled(0));
			sendInputEvent(EV_ABS, ABS_MT_POSITION_Y, cameraPosScaled(1));
			sendInputEvent(EV_SYN, SYN_REPORT);
		}

		if(!bounds.contains(m_CameraPos))
		{
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_CAMERA);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
			sendInputEvent(EV_SYN, SYN_REPORT);
			m_CameraSlotDown = false;
		}
	}

	void Application::doLookoutMove()
	{
		//std::cout << "doLookoutMove(): m_LookoutSlotDown = " << m_LookoutSlotDown << std::endl;
		Array2f const scale = Array2f(0x00007fff, 0x00007fff) / m_TouchscreenSize.array().cast<float>();
		Rect2i const bounds = Rect2i(Vec2i(0,0), m_TouchscreenSize);

		if(m_CameraSlotDown)
		{
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_CAMERA);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
			sendInputEvent(EV_SYN, SYN_REPORT);
			m_CameraSlotDown = false;
		}


		if(!m_LookoutSlotDown)
		{
			m_LookoutPos = m_LookoutArea.center();
			m_LookoutSlotDown = true;

			//std::cout << "m_LookoutPos:" << m_LookoutPos << std::endl;

			Array2i const lookoutPosScaled = (m_LookoutPos.cast<float>().array() * scale).round().cast<int>();

			//std::cout << "lookoutPosScaled:" << lookoutPosScaled << std::endl;

			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_LOOKOUT);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, SLOT_LOOKOUT);
			sendInputEvent(EV_ABS, ABS_MT_TOUCH_MAJOR, DEFAULT_TOUCH_MAJOR);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE, DEFAULT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_POSITION_X, lookoutPosScaled(0));
			sendInputEvent(EV_ABS, ABS_MT_POSITION_Y, lookoutPosScaled(1));
			sendInputEvent(EV_SYN, SYN_REPORT);
		}

		m_LookoutPos += Vec2i(m_Message.data.mouseMoved.deltaX, m_Message.data.mouseMoved.deltaY);

		{
			Array2i const lookoutPosScaled = (m_LookoutPos.cast<float>().array() * scale).round().cast<int>();
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_LOOKOUT);
			sendInputEvent(EV_ABS, ABS_MT_POSITION_X, lookoutPosScaled(0));
			sendInputEvent(EV_ABS, ABS_MT_POSITION_Y, lookoutPosScaled(1));
			sendInputEvent(EV_SYN, SYN_REPORT);
		}

		if(!bounds.contains(m_LookoutPos))
		{
			sendInputEvent(EV_ABS, ABS_MT_SLOT, SLOT_LOOKOUT);
			sendInputEvent(EV_ABS, ABS_MT_PRESSURE);
			sendInputEvent(EV_ABS, ABS_MT_TRACKING_ID, TRACKING_ID_NONE);
			sendInputEvent(EV_SYN, SYN_REPORT);
			m_LookoutSlotDown = false;
		}
	}

	void Application::sendInputEvent(std::uint16_t type, std::uint16_t code, std::uint32_t value)
	{
#if !defined(__APPLE__)
		input_event ev;
		gettimeofday(&ev.time, nullptr);
		ev.type = type;
		ev.code = code;
		ev.value = value;
		m_InputDevice.write_some(asio::buffer(&ev, sizeof(ev)));
#endif
	}

} //namespace srv
} //namespace wotbcontrols
