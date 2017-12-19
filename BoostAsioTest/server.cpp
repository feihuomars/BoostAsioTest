#include <iostream>

#include <boost/asio/read_until.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

#include "server.h"


Session::Session(TcpSocket t_socket)
    : m_socket(std::move(t_socket))
{
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
    /*BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "(" << t_bytesTransferred << ")" 
        << ", in_avail = " << m_requestBuf_.in_avail() << ", size = " 
        << m_requestBuf_.size() << ", max_size = " << m_requestBuf_.max_size() << ".";*/

    std::istream requestStream(&m_requestBuf_);
    readData(requestStream);

    auto pos = m_fileName.find_last_of('\\');
    if (pos != std::string::npos)
        m_fileName = m_fileName.substr(pos + 1);

    createFile();

    // write extra bytes to file
    do {
        requestStream.read(m_buf.data(), m_buf.size());
        //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " write " << requestStream.gcount() << " bytes.";
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
	stream >> m_data;
    stream.read(m_buf.data(), 2);

    /*BOOST_LOG_TRIVIAL(trace) << m_fileName << " size is " << m_fileSize
        << ", tellg = " << stream.tellg();*/
}


void Session::createFile()
{
    m_outputFile.open(m_fileName, std::ios_base::binary);
    if (!m_outputFile) {
        //BOOST_LOG_TRIVIAL(error) << __LINE__ << ": Failed to create: " << m_fileName;
        return;
    }
}


void Session::doReadFileContent(size_t t_bytesTransferred)
{
    if (t_bytesTransferred > 0) {
        m_outputFile.write(m_buf.data(), static_cast<std::streamsize>(t_bytesTransferred));

        //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " recv " << m_outputFile.tellp() << " bytes";

        if (m_outputFile.tellp() >= static_cast<std::streamsize>(m_fileSize)) {
			//接收完成位置
            std::cout << "Received file: " << m_fileName << " size: " << m_fileSize << m_data << std::endl;
			/*auto self = shared_from_this();
			string = "来自服务器的返回信息";
			m_socket.async_write_some(boost::asio::buffer(string), [this, self](boost::system::error_code ec, size_t bytes) {
				if (ec) {
					std::cout << "extra infromaion failed" << std::endl;
				}
				else {
					std::cout << "extra informaiton success " << "size: " << bytes << std::endl;
				}
			});*/
			Sleep(10000);
			const char* str = "data from server\nhello";
			
			std::string string = " data from server";
			string += "";
			//memcpy(m_bufToClient.data(), string.c_str(), 1024);
			//auto buf = boost::asio::buffer(m_bufToClient.data(), m_bufToClient.size());
			
			openFile("D://test/picture.jpg");
			//writeBuffer(m_request);
			m_request.consume(m_request.size());
			//std::ostream requestStream(&m_request);
			//requestStream << "from server" ;

			//boost::asio::async_write(m_socket,
			//	buf,
			//	[this](boost::system::error_code ec, size_t /*length*/)
			//{
			//	
			//});


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
    /*BOOST_LOG_TRIVIAL(error) << __FUNCTION__ << " in " << t_functionName << " due to " 
        << t_ec << " " << t_ec.message() << std::endl;*/
}

void Session::openFile(std::string const& t_path)
{
	m_sourceFile.open(t_path, std::ios_base::binary | std::ios_base::ate);
	if (m_sourceFile.fail()) {
		//throw std::fstream::failure("Failed while opening file " + t_path);
		std::cout << "Failed while opening file " + t_path << std::endl;
	}


	m_sourceFile.seekg(0, m_sourceFile.end);
	auto fileSize = m_sourceFile.tellg();
	m_sourceFile.seekg(0, m_sourceFile.beg);

	std::ostream requestStream(&m_request);
	boost::filesystem::path p(t_path);
	size_t number = 20172222;
	//requestStream << p.filename().string() << "\n" << fileSize << "\n\n";
	requestStream << p.filename().string() << "\n" << fileSize << "\n" << "data" << "\n\n";
	//BOOST_LOG_TRIVIAL(trace) << "Request size: " << m_request.size();
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
		std::cout << "已发送字符串： " << m_string.data() << "大小： " << m_string.size() << std::endl;
		while (m_sourceFile) {
			m_sourceFile.read(m_bufToClient.data(), m_bufToClient.size());
			if (m_sourceFile.fail() && !m_sourceFile.eof()) {
				auto msg = "Failed while reading file";
				//BOOST_LOG_TRIVIAL(error) << msg;
				//throw std::fstream::failure(msg);
			}
			std::stringstream ss;
			ss << "Send " << m_sourceFile.gcount() << " bytes,   total: "
				<< m_sourceFile.tellg() << " bytes" /*<< "   content: " <<m_bufToClient.data()*/;
			// BOOST_LOG_TRIVIAL(trace) << ss.str();
			std::cout << ss.str() << std::endl;

			auto buf = boost::asio::buffer(m_bufToClient.data(), static_cast<size_t>(m_sourceFile.gcount()));
			//writeBuffer(buf);
			
			boost::system::error_code error_code;
			boost::asio::write(m_socket, buf, error_code);

		}
		
			//发送完成位置
			std::cout << "send file completed." << std::endl;
			
		
	}
	else {
		//BOOST_LOG_TRIVIAL(error) << "Error: " << t_ec.message();
	}
}




Server::Server(boost::asio::io_service& t_ioService, short t_port, std::string const& t_workDirectory)
    : m_socket(t_ioService),
    m_acceptor(t_ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), t_port)),
    m_workDirectory(t_workDirectory)
{
    std::cout << "Server started\n";

    createWorkDirectory();

    doAccept();
}


void Server::doAccept()
{
    m_acceptor.async_accept(m_socket,
        [this](boost::system::error_code ec)
    {
        if (!ec)
            std::make_shared<Session>(std::move(m_socket))->start();

        doAccept();
    });
}


void Server::createWorkDirectory()
{
    using namespace boost::filesystem;
    auto currentPath = path(m_workDirectory);
	if (!exists(currentPath) && !create_directory(currentPath)) {
		//BOOST_LOG_TRIVIAL(error) << "Coudn't create working directory: " << m_workDirectory;
	}
        
	current_path(currentPath);
}
