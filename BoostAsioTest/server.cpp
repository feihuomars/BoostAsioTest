#include <iostream>

#include <boost/asio/read_until.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread/thread.hpp>
#include <thread>

#include "server.h"


Session::Session(TcpSocket t_socket)
    : m_socket(std::move(t_socket))
{
	//doRead();
}


void Session::doRead()
{
    auto self = shared_from_this();
    async_read_until(m_socket, m_requestBuf_, "\n\n",
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            if (!ec)
                processRead(bytes);
            else
                handleError(__FUNCTION__, ec);
        });
}


void Session::processRead(size_t t_bytesTransferred)
{
    std::cout << __FUNCTION__ << "(" << t_bytesTransferred << ")" 
        << ", in_avail = " << m_requestBuf_.in_avail() << ", size = " 
        << m_requestBuf_.size() << ", max_size = " << m_requestBuf_.max_size() << "." << std::endl;

    std::istream requestStream(&m_requestBuf_);
    readData(requestStream);

    auto pos = m_fileName.find_last_of('\\');
    if (pos != std::string::npos)
        m_fileName = m_fileName.substr(pos + 1);

    createFile();

    // write extra bytes to file
    do {
        requestStream.read(m_buf.data(), m_buf.size());
        std::cout << __FUNCTION__ << " write " << requestStream.gcount() << " bytes." << std::endl;
        m_outputFile.write(m_buf.data(), requestStream.gcount());
    } while (requestStream.gcount() > 0);

    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            if (!ec)
                doReadFileContent(bytes);
            else
                handleError(__FUNCTION__, ec);
        });
}


void Session::readData(std::istream &stream)
{
    stream >> m_fileName;
    stream >> m_fileSize;
	stream >> startTime;
	stream >> endTime;
	stream >> pictureID;
    stream.read(m_buf.data(), 2);

    std::cout << m_fileName << " size is " << m_fileSize
        << ", tellg = " << stream.tellg() << std::endl;
}


void Session::createFile()
{
    m_outputFile.open(m_fileName, std::ios_base::binary);
    if (!m_outputFile) {
        std::cout << __LINE__ << ": Failed to create: " << m_fileName << std::endl;
        return;
    }
}


void Session::doReadFileContent(size_t t_bytesTransferred)
{
    if (t_bytesTransferred > 0) {
        m_outputFile.write(m_buf.data(), static_cast<std::streamsize>(t_bytesTransferred));

        std::cout << __FUNCTION__ << " recv " << m_outputFile.tellp() << " bytes" << std::endl;

        if (m_outputFile.tellp() >= static_cast<std::streamsize>(m_fileSize)) {
			//接收完成位置
            std::cout << "Received file: " << m_fileName << " size: " << m_fileSize << "\n startTime: "<< startTime 
				<< "\n endTime: " << endTime << "\n pictureID: " << pictureID << boost::filesystem::current_path().string() << std::endl;
			
			Sleep(3000);
			
			resultPos = "电子科技大学";
			resultTime = "2018-01-01*01:01:01";
			openFile("D://test/picture.jpg");
			//writeBuffer(m_request);
			
            return;
        }
    }
    auto self = shared_from_this();
    m_socket.async_read_some(boost::asio::buffer(m_buf.data(), m_buf.size()),
        [this, self](boost::system::error_code ec, size_t bytes)
        {
            doReadFileContent(bytes);
        });
}


void Session::handleError(std::string const& t_functionName, boost::system::error_code const& t_ec)
{
    std::cout << __FUNCTION__ << " in " << t_functionName << " due to " 
        << t_ec << " " << t_ec.message() << std::endl;
}

void Session::openFile(std::string const& t_path)
{
	//打开文件
	m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
	if (m_sourceFile.fail()) {
		//throw std::fstream::failure("Failed while opening file " + t_path);
		std::cout << "Failed while opening file " + t_path << std::endl;
	}

	//文件指针移至末尾确定文件大小
	m_sourceFile.seekg(0, m_sourceFile.end);
	auto fileSize = m_sourceFile.tellg();
	m_sourceFile.seekg(0, m_sourceFile.beg);

	std::ostream requestStream(&m_request);
	boost::filesystem::path p(t_path);
	//需要返回的信息
	requestStream << p.filename().string() << "\n" << fileSize << "\n" << 
		resultPos << "\n" << resultTime << "\n" << pictureID << "\n\n";
	std::cout << "Request size: " << m_request.size() << std::endl;
	
	std::stringstream ss;
	ss << fileSize;
	std::string string = "";
	string = p.filename().string() + "\n" + ss.str() + "\n" + "data" + "\n\n";
	memcpy(m_string.data(), string.c_str(), m_string.size());
	
	auto self = shared_from_this();
	boost::asio::async_write(m_socket, 
		m_request,
		[this, self] (boost::system::error_code ec, size_t){
		doWriteFile(ec);
	});

}

void Session::doWriteFile(const boost::system::error_code& t_ec)
{
	if (!t_ec) {
		//改为同步操作
		std::cout << "开始发送文件" << std::endl;
		while (m_sourceFile) {
			m_sourceFile.read(m_bufToClient.data(), m_bufToClient.size());
			if (m_sourceFile.fail() && !m_sourceFile.eof()) {
				auto msg = "Failed while reading file";
				std::cout << msg << std::endl;
				//throw std::fstream::failure(msg);
			}
			std::stringstream ss;
			ss << "Send " << m_sourceFile.gcount() << " bytes,   total: "
				<< m_sourceFile.tellg() << " bytes";
			
			//std::cout << ss.str() << std::endl;

			auto buf = boost::asio::buffer(m_bufToClient.data(), static_cast<size_t>(m_sourceFile.gcount()));
			//writeBuffer(buf);
			
			boost::system::error_code error_code;
			boost::asio::write(m_socket, buf, error_code);

		}
		
		//发送完成位置
		std::cout << "文件发送完毕" << std::endl;
			
		
	}
	else {
		std::cout << "Error: " << t_ec.message() << std::endl;
	}
}




Server::Server(IoService& t_ioService, short t_port, std::string const& t_workDirectory)
    : m_socket(t_ioService),
    m_acceptor(t_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), t_port)),
    m_workDirectory(t_workDirectory),
	m_ioservice(t_ioService)
{
    std::cout << "Server started\n";
	
    createWorkDirectory();

    doAccept();
	
}

void Server::startThread(boost::asio::ip::tcp::socket socket) {
	std::make_shared<Session>(std::move(socket))->start();
	
}

void Server::doAccept()
{
  //  m_acceptor.async_accept(m_socket,
  //      [this](boost::system::error_code ec)
  //  {
		//if (!ec) {
		//	
		//	//std::make_shared<Session>(std::move(m_socket))->start();
		//	//boost::shared_ptr<boost::asio::io_service> ioServicePtr;
		//	//std::thread(boost::bind(&boost::asio::io_service::run, &m_ioservice)).join();
		//	//std::make_shared<Session>(std::move(m_socket))->start();
		//	std::thread(startThread, std::move(m_socket)).join();
		//}
		//
  //      doAccept();
  //  });

	while (true) {
		boost::system::error_code ec;
		m_acceptor.accept(m_socket, ec);
		if (ec) {
			std::cout << ec.message() << std::endl;
		}
		std::thread(startThread, std::move(m_socket)).join();
		
	}
	
}


void Server::createWorkDirectory()
{
	//设置当前工作目录
    auto currentPath = boost::filesystem::path(m_workDirectory);
	if (!exists(currentPath) && !create_directory(currentPath)) {
		std::cout << "Coudn't create working directory: " << m_workDirectory << std::endl;
	}
        
	current_path(currentPath);
}
