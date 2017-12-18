#pragma once

#include <array>
#include <fstream>
#include <string>
#include <memory>
#include <boost/asio.hpp>


class Session
    : public std::enable_shared_from_this<Session>
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;

    Session(TcpSocket t_socket);

    void start()
    {
        doRead();
    }

private:
    void doRead();
    void processRead(size_t t_bytesTransferred);
    void createFile();
    void readData(std::istream &stream);
    void doReadFileContent(size_t t_bytesTransferred);
    void handleError(std::string const& t_functionName, boost::system::error_code const& t_ec);

	//回传客户端所需函数
	void openFile(std::string const& t_path);
	void doWriteFile(const boost::system::error_code& t_ec);
	template<typename Buffer>
	void writeBuffer(Buffer& t_buffer);


    TcpSocket m_socket;
    enum { MaxLength = 40960 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf_;
    std::ofstream m_outputFile;
    size_t m_fileSize;
	std::string m_data;
    std::string m_fileName;
	std::string string;

	//回传给客户端所需变量
	enum { MessageSize = 1024 };
	std::array<char, MessageSize> m_bufToClient;
	std::array<char, 1024> m_string;
	boost::asio::streambuf m_request;
	std::ifstream m_sourceFile;
	std::string m_path;
	

};

template<typename Buffer>
void Session::writeBuffer(Buffer& t_buffer)
{
	boost::asio::async_write(m_socket,
		t_buffer,
		[this](boost::system::error_code ec, size_t /*length*/)
	{
		if (ec) {
			std::cout << "send data failed" << std::endl;
		}
		else {
			std::cout << "send data success" << std::endl;
		}
		doWriteFile(ec);
		
	});
}




class Server
{
public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using IoService = boost::asio::io_service;

    Server(IoService& t_ioService, short t_port, std::string const& t_workDirectory);

private:
    void doAccept();
    void createWorkDirectory();

    TcpSocket m_socket;
    TcpAcceptor m_acceptor;

    std::string m_workDirectory;
};
