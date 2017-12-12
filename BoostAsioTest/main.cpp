#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <string>

using namespace boost::asio;
using ip::tcp;
using boost::system::error_code;


struct CHelloWorld_Service
{
	CHelloWorld_Service(io_service &iosev, char &buf)
		:m_iosev(iosev), m_acceptor(iosev, tcp::endpoint(tcp::v4(), 1000))
	{
		&bufWrite = buf;
	}

	void start()
	{
		// 开始等待连接（非阻塞）
		boost::shared_ptr<tcp::socket> psocket(new tcp::socket(m_iosev));
		// 触发的事件只有error_code参数，所以用boost::bind把socket绑定进去
		m_acceptor.async_accept(*psocket,
			boost::bind(&CHelloWorld_Service::accept_handler, this, psocket, _1));
	}

	// 有客户端连接时accept_handler触发
	void accept_handler(boost::shared_ptr<tcp::socket> psocket, error_code ec)
	{
		if (ec) return;
		// 继续等待连接
		start();
		// 显示远程IP
		std::cout << psocket->remote_endpoint().address() << std::endl;
		// 发送信息(非阻塞)
		boost::shared_ptr<std::string> pstr(new std::string("hello async world!"));
		
		psocket->async_read_some(buffer(bufRead),
			boost::bind(&CHelloWorld_Service::read_handler, this, boost::asio::placeholders::error, psocket));
		
		/*psocket->async_write_some(buffer(*pstr),
			boost::bind(&CHelloWorld_Service::write_handler, this, pstr, _1, _2));*/
	}

	// 异步写操作完成后write_handler触发
	void write_handler( error_code ec)
	{
		if (ec)
			std::cout << "发送失败!" << std::endl;
		else
			std::cout << " 已发送"  << std::endl;
	}

	void read_handler( error_code errorCode, boost::shared_ptr<tcp::socket> psocket) {
		if (errorCode)
			std::cout << "接收失败!" << std::endl;
		else {
			std::cout << " 已接收： " << bufRead << std::endl;
			psocket->async_write_some(buffer(bufWrite),
				boost::bind(&CHelloWorld_Service::write_handler, this, _1));
		}
			
	}

private:
	io_service &m_iosev;
	ip::tcp::acceptor m_acceptor;
	char bufRead[128];
	char bufWrite[1024];
};




int main(int argc, char* argv[])
{
	
	//// 所有asio类都需要io_service对象
	//io_service iosev;
	//ip::tcp::acceptor acceptor(iosev,
	//	ip::tcp::endpoint(ip::tcp::v4(), 1000));
	//for (;;)
	//{
	//	// socket对象
	//	ip::tcp::socket socket(iosev);
	//	// 等待直到客户端连接进来
	//	acceptor.accept(socket);
	//	// 显示连接进来的客户端
	//	std::cout << socket.remote_endpoint().address() << std::endl;
	//	// 向客户端发送hello world!
	//	boost::system::error_code ec;
	//	socket.write_some(buffer("hello world!"), ec);

	//	// 如果出错，打印出错信息
	//	if (ec)
	//	{
	//		std::cout <<
	//			boost::system::system_error(ec).what() << std::endl;
	//		break;
	//	}
	//	// 与当前客户交互完成后循环继续等待下一客户连接
	//}
	char data[1024];
	for (int i = 0; i < 1024; i++) {
		data[i] = i;
	}

	io_service iosev;
	CHelloWorld_Service sev(iosev);
	// 开始等待连接
	sev.start();
	iosev.run();

	
	/*Server server;
	server.run();*/



	return 0;
}



class Server
{
	typedef ip::tcp::socket socket_type;
	typedef std::shared_ptr<socket_type> sock_ptr;
public:
	Server() :m_acceptor(m_io, ip::tcp::endpoint(ip::tcp::v4(), 1000))
	{
		accept();
	}
	void run()
	{
		m_io.run();
	}
private:
	void accept()
	{
		sock_ptr sock(new socket_type(m_io));
		m_acceptor.async_accept(*sock,
			std::bind(&Server::accept_handler, this, std::placeholders::_1, sock));
	}
	void accept_handler(const boost::system::error_code& ec, sock_ptr sock)
	{
		if (ec)
		{
			return;
		}
		std::cout << "client:";
		std::cout << sock->remote_endpoint().address() << std::endl;
		sock->async_write_some(buffer("hello asio"),
			std::bind(&Server::write_handler, this, std::placeholders::_1));
		accept();
	}
	void write_handler(const boost::system::error_code errorCode)
	{
		if (errorCode) {
			std::cout << "send msg failed." << std::endl;
		}
		else {
			std::cout << "send msg complete." << std::endl;
		}

	}
private:
	io_service m_io;
	ip::tcp::acceptor m_acceptor;
};