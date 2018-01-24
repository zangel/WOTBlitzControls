#pragma once

#include <system_error>
#include <unordered_map>

#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/high_resolution_timer.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/program_options.hpp>


#include <WOTBControlsProto.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace wotbcontrols
{
namespace srv
{
	namespace fs = boost::filesystem;
	namespace asio = boost::asio;
	namespace system = boost::system;
	namespace po = boost::program_options;
	namespace pt = boost::posix_time;

	using Vec2i = Eigen::Vector2i;
	using Vec2f = Eigen::Vector2f;
	using Array2i = Eigen::Array2i;
	using Array2f = Eigen::Array2f;

	using Rect2i = Eigen::AlignedBox<Eigen::Vector2i::Scalar, 2>;

} //namespace srv
} //namespace wotbcontrols
