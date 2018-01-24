#pragma once

#include <system_error>

#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/program_options.hpp>

#include <WOTBControlsProto.hpp>
#include <ApplicationServices/ApplicationServices.h>

namespace wotbcontrols
{
namespace cli
{
	namespace fs = boost::filesystem;
	namespace asio = boost::asio;
	namespace system = boost::system;
	namespace po = boost::program_options;
	namespace pt = boost::posix_time;

} //namespace cli
} //namespace wotbcontrols
