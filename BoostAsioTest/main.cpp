#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <string>

#include "server.h"

using namespace boost::asio;
using ip::tcp;
using boost::system::error_code;

void test2(int& a) {
	std::cout << a << std::endl;
}

void test(int& a) {
	std::cout << a << std::endl;
	test2(a);
}



int main(int argc, char* argv[])
{
	

	boost::asio::io_service ioService;
	Server server(ioService, 1000, "D:/test/serverRecv");
	ioService.run();

	int a = 10;

	test(a);

	return 0;
}



