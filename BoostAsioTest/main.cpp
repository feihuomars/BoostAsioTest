#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <string>


#include "server.h"

int main(int argc, char* argv[])
{
	
	
	boost::asio::io_service ioService;
	Server server(ioService, 1000);
	ioService.run();
	//server.run_ioService();
	
	return 0;
}



