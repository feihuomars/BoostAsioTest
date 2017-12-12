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
		// ��ʼ�ȴ����ӣ���������
		boost::shared_ptr<tcp::socket> psocket(new tcp::socket(m_iosev));
		// �������¼�ֻ��error_code������������boost::bind��socket�󶨽�ȥ
		m_acceptor.async_accept(*psocket,
			boost::bind(&CHelloWorld_Service::accept_handler, this, psocket, _1));
	}

	// �пͻ�������ʱaccept_handler����
	void accept_handler(boost::shared_ptr<tcp::socket> psocket, error_code ec)
	{
		if (ec) return;
		// �����ȴ�����
		start();
		// ��ʾԶ��IP
		std::cout << psocket->remote_endpoint().address() << std::endl;
		// ������Ϣ(������)
		boost::shared_ptr<std::string> pstr(new std::string("hello async world!"));
		
		psocket->async_read_some(buffer(bufRead),
			boost::bind(&CHelloWorld_Service::read_handler, this, boost::asio::placeholders::error, psocket));
		
		/*psocket->async_write_some(buffer(*pstr),
			boost::bind(&CHelloWorld_Service::write_handler, this, pstr, _1, _2));*/
	}

	// �첽д������ɺ�write_handler����
	void write_handler( error_code ec)
	{
		if (ec)
			std::cout << "����ʧ��!" << std::endl;
		else
			std::cout << " �ѷ���"  << std::endl;
	}

	void read_handler( error_code errorCode, boost::shared_ptr<tcp::socket> psocket) {
		if (errorCode)
			std::cout << "����ʧ��!" << std::endl;
		else {
			std::cout << " �ѽ��գ� " << bufRead << std::endl;
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
	
	//// ����asio�඼��Ҫio_service����
	//io_service iosev;
	//ip::tcp::acceptor acceptor(iosev,
	//	ip::tcp::endpoint(ip::tcp::v4(), 1000));
	//for (;;)
	//{
	//	// socket����
	//	ip::tcp::socket socket(iosev);
	//	// �ȴ�ֱ���ͻ������ӽ���
	//	acceptor.accept(socket);
	//	// ��ʾ���ӽ����Ŀͻ���
	//	std::cout << socket.remote_endpoint().address() << std::endl;
	//	// ��ͻ��˷���hello world!
	//	boost::system::error_code ec;
	//	socket.write_some(buffer("hello world!"), ec);

	//	// ���������ӡ������Ϣ
	//	if (ec)
	//	{
	//		std::cout <<
	//			boost::system::system_error(ec).what() << std::endl;
	//		break;
	//	}
	//	// �뵱ǰ�ͻ�������ɺ�ѭ�������ȴ���һ�ͻ�����
	//}
	char data[1024];
	for (int i = 0; i < 1024; i++) {
		data[i] = i;
	}

	io_service iosev;
	CHelloWorld_Service sev(iosev);
	// ��ʼ�ȴ�����
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