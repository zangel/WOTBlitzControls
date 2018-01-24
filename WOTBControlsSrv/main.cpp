#include "Config.hpp"
#include "Application.hpp"

int main(int argc, const char * argv[])
{
	using namespace wotbcontrols::srv;

	if(auto aie = Application::instance().initialize(argc, argv))
		return aie.value();

	Application::instance().run();

	Application::instance().shutdown();
	
	return 0;
}
