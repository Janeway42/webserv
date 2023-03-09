// #include "WebServer.hpp"
#include "../includes/WebServer.hpp"  // jaka

WebServer::WebServer(std::string const & configFileName)
{
	_addr = new struct addrinfo();
	struct addrinfo hints;

	hints.ai_family = PF_UNSPEC; 
	hints.ai_flags = AI_PASSIVE; 
	hints.ai_socktype = SOCK_STREAM;

    ConfigFileParser configFileData = ConfigFileParser(configFileName);
    _servers =  configFileData.servers;
    std::vector<ServerData>::iterator it_server = _servers.begin();
    for (; it_server != _servers.cend(); ++it_server) {
        std::cout << "IP ADDRESS: " << it_server->getIpAddress().c_str() << std::endl;
        std::cout << "PORT: " << it_server->getListensTo().c_str() << std::endl;
        if (getaddrinfo(it_server->getIpAddress().c_str(), it_server->getListensTo().c_str(), &hints, &_addr) != 0)
            throw ServerException(("failed addr"));
		else {
			_listening_socket = socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);
			// it_server->setListeningSocket(_listening_socket);
			break;
		}
    }

	if (_listening_socket < 0)
		throw ServerException(("failed socket"));
	fcntl(_listening_socket, F_SETFL, O_NONBLOCK);

	int socket_on = 1;
	setsockopt(_listening_socket, SOL_SOCKET, SO_REUSEADDR, &socket_on, sizeof(socket_on));
	if (bind(_listening_socket, _addr->ai_addr, _addr->ai_addrlen) == -1)
	{
		perror("... bind error: ");
		throw ServerException(("failed bind"));
	}
	if (listen(_listening_socket, SOMAXCONN) == -1)  // max nr of accepted connections 
		throw ServerException(("failed listen"));

	struct kevent evSet;
	_kq = kqueue();
	if (_kq == -1)
		throw ServerException(("failed kq"));

	EV_SET(&evSet, _listening_socket, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_WRITE	, 0, NULL);
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1) 
		throw ServerException(("failed kevent start"));
}

WebServer::~WebServer()
{
	close(_kq);
	freeaddrinfo(_addr);
}

// --------------------------------------------------------- server main loop
// --------------------------------------------------------------------------

void WebServer::runServer()
{
	struct kevent evList[MAX_EVENTS];
//	struct kevent evSet;
	int i;
//	struct sockaddr_storage socket_addr;
//	socklen_t socklen = sizeof(socket_addr);

//	int loop1 = 0;
	while (1)
	{
	//	std::cout << "WHILE LOOP ------------------------------" << loop1 << std::endl;
		int nr_events = kevent(_kq, NULL, 0, evList, MAX_EVENTS, NULL);
	//	std::cout << "NR EVENTS: " << nr_events << std::endl;

		if (nr_events < 1)
			throw ServerException("failed number events");
		else
		{
			for (i = 0; i < nr_events; i++)
			{
			//	std::cout << "filter: " << evList[i].filter << std::endl; // test line 

				if (evList[i].flags & EV_ERROR)
					std::cout << "Event error\n";
				else if (evList[i].ident == _listening_socket)
					newClient(evList[i]);
				else if (evList[i].filter == EVFILT_READ)
				{
				//	std::cout << "READ\n";
					if (evList[i].flags & EV_EOF)  // if client closes connection 
					{
						if (removeEvent(evList[i], EVFILT_READ) == 1)
							throw ServerException("failed kevent eof - read");
						closeClient(evList[i]);
					}
					else
						readRequest(evList[i]);
				}
				else if (evList[i].filter == EVFILT_WRITE)
				{
				//	std::cout << "WRITE\n";
					if (evList[i].flags & EV_EOF)
					{
						if (removeEvent(evList[i], EVFILT_WRITE) == 1)
							throw ServerException("failed kevent eof - write");
						closeClient(evList[i]);
					}
					else
						sendResponse(evList[i]);
				}
				// std::cout << std::endl;
			}
		}	
		// std::cout << std::endl;
//		loop1++;
	}
}

// --------------------------------------------------------- server functions 
// --------------------------------------------------------------------------

void WebServer::readRequest(struct kevent& event)
{
	char buffer[50];
	memset(&buffer, '\0', 50);
	data::Request* storage = (data::Request*)event.udata;
	//Request* storage = (Request*)event.udata;

	int ret = recv(event.ident, &buffer, sizeof(buffer) - 1, 0);
//	std::cout << "bytes read: " << ret << std::endl;                 // test line 
	if (ret < 0)
	{
		if (storage->getEarlyClose() == false) // if it fails 
		{
			std::cout << "failed recv\n";
			storage->setError(true);
			if (removeEvent(event, EVFILT_READ) == 1)
				throw ServerException("failed EVFILT_READ removal\n");
			std::cout << "send error message back to client\n";
		}
		else
			std::cout << "EVFILT_READ already closed\n";  // if it has been closed in write due to timeout  - test line
	}

	// for cgi ret might be 0 - might need to add later, but for kqueue it will never be 0 

	else if (storage->getEarlyClose() == false && storage->getDone() == false)
	{
	//	std::cout << "append buffer\n";
		storage->appendToRequest(buffer, event.ident);

		if (storage->getError() == true)
		{
			std::cout << "error parsing - sending response - failure\n";
			if (removeEvent(event, EVFILT_READ) == 1)
				throw ServerException("failed kevent eof - read failure");
		}
		else if (storage->getDone() == true)
		{
			std::cout << "done parsing - sending response - success\n";
			if (removeEvent(event, EVFILT_READ) == 1)
				throw ServerException("failed kevent eof - read success");
			return ;
		}
	}
	else 
		std::cout << "just passing by\n";
}



void WebServer::sendResponse(struct kevent& event)
{
	data::Request *storage = (data::Request *)event.udata;

	std::time_t elapsedTime = std::time(NULL);
	if (elapsedTime - storage->getTime() > 5)
	{
		std::cout << "Unable to process request, it takes too long!\n";
		storage->setError(true);
		storage->setEarlyClose(true);
		if (removeEvent(event, EVFILT_READ) == 1)
			throw ServerException("failed EVFILT_READ removal\n"); 
	}

	if (storage->getError())
	{
		sendResponseFile(event, "./resources/error404.html");
		if (removeEvent(event, EVFILT_WRITE) == 1)
			throw ServerException("failed kevent EV_DELETE client - send error");
		closeClient(event);
		std::cout << "closed connection from write - error\n";
	}
	else if (storage->getError() == false && storage->getDone() == true)
	{
		std::string temp = (storage->getRequestData()).getHttpPath();

		// METHOD GET AND NO FORM
		if (storage->getRequestData().getFileExtention() == ".html" && storage->getRequestData().getQueryString() == "") { 
			std::cout << YEL "   Method get, just file, no form. File path [" << storage->getRequestData().getPath() << "]\n" RES;
			sendResponseFile(event, storage->getRequestData().getPath());
		}
		// IT'S NOT CGI REQUEST, AND IF PATH IS A FILE (NOT FOLDER), AND IF THIS FILE EXISTS
		else if (storage->getRequestData().getFileExtention() == ".php" && storage->getRequestData().getQueryString() != "") { 
			std::cout << YEL "   Method get/post WITH form. File path [" << storage->getRequestData().getPath() << "]\n" RES;
			sendResponseFile(event, "./resources/index_should_be_cgi_content.html");
		}
		// IT IS AN IMAGE
		else if (storage->getRequestData().getFileExtention() == ".jpg" || storage->getRequestData().getFileExtention() == ".png") {
			std::cout << YEL "   Method get IMAGE. File path [" << storage->getRequestData().getPath() << "]\n" RES;
			sendImmage(event, storage->getRequestData().getPath());
		}
		// IF PATH IS A FOLDER AND IF THERE IS INDEX.HTML IN THE FOLDER
		// !!! BUT ALSO CHECK, IF THIS FOLDER PATH EXISTS AS THE LOCATION BLOCK !
		// IF NOT, ERROR SHOULD BE RETURNED.
		// IF YES, IT CHECKS IF THERE IS AN INDEX FILE
		//     IN NO INDEX FILE, IT CHECKS IF AUTOINDEX IS ON/OFF
		//			IF OFF, RETURN 'NOT ALLOWED'
		else
		{	
			std::vector<ServerData>::iterator it_server = _servers.begin();
    		for (; it_server != _servers.cend(); ++it_server) {
				std::cout << "Index file: " << it_server->getIndexFile().c_str() << std::endl;
				sendResponseFile(event, it_server->getIndexFile());
				break;
			}
		}

		if (removeEvent(event, EVFILT_WRITE) == 1)
			throw ServerException("failed kevent EV_DELETE client - send success");
		closeClient(event);
		std::cout << "closed connection from write - done\n";
	}
}

// --------------------------------------------------------- client functions 
// --------------------------------------------------------------------------

void WebServer::newClient(struct kevent event)
{
	struct kevent evSet;
	struct sockaddr_storage socket_addr;
	socklen_t socklen = sizeof(socket_addr);

	std::cout << "make new client connection\n";
	
	int opt_value = 1;
	int fd = accept(event.ident, (struct sockaddr *)&socket_addr, &socklen);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(opt_value));
	if (fd == -1)
		throw ServerException("failed accept");

	data::Request *storage = new data::Request();
	EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, storage); 
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw ServerException("failed kevent add EVFILT_READ");
	EV_SET(&evSet, fd, EVFILT_WRITE, EV_ADD, 0, 0, storage); 
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw ServerException("failed kevent add EVFILT_WRITE");
}

void WebServer::closeClient(struct kevent& event)
{
	data::Request *storage;
	storage = (data::Request *)event.udata;

	close(event.ident); 
	std::cout << "connection closed\n";
	delete(storage);
}

// --------------------------------------------------------- utils functions
// -------------------------------------------------------------------------

int WebServer::removeEvent(struct kevent& event, int filter)
{
	struct kevent evSet;
	data::Request *storage = (data::Request *)event.udata;

	EV_SET(&evSet, event.ident, filter, EV_DELETE, 0, 0, storage); 
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
	{
		std::cout << "erno: " << errno << std::endl;                         // test line - to be removed 
		throw ServerException("failed kevent EV_DELETE client");
	}
	return (0);
}

std::string WebServer::streamFile(std::string file)
{
	std::string responseNoFav;
	std::fstream    infile;


	infile.open(file, std::fstream::in);
	if (!infile)
		throw ServerException("Error: File not be opened for reading!");
	while (infile)     // While there's still stuff left to read
	{
		std::string strInput;
		std::getline(infile, strInput);
		responseNoFav.append(strInput);
		responseNoFav.append("\n");
	}
	infile.close();
	return (responseNoFav);
}

void WebServer::sendResponseFile(struct kevent& event, std::string file)
{
	data::Request *storage = (data::Request *)event.udata;
	std::string response;
	std::string headerBlock;
	response = streamFile(file);
	
	int temp = response.length();
	std::string fileLen = std::to_string(temp);
	std::string contentLen = "Content-Length: ";
	contentLen.append(fileLen);
	contentLen.append("\r\n");
	// std::cout << RED "ContLen: " << contentLen << "\n" RES;

	headerBlock = 	"HTTP/1.1 200 OK\n"
					"Content-Type: text/html\n";
					// "Content-Type: image/png\n";
	if (storage->getError() == true)
		headerBlock = 	"HTTP/1.1 404 Not Found\n"
						"Content-Type: text/html\n";
	headerBlock.append(contentLen);
	headerBlock.append("\r\n\r\n");
	headerBlock.append(response);

	int ret = send(event.ident, headerBlock.c_str(), headerBlock.length(), 0);
	
	if (ret == -1)
		throw ServerException("Send failed\n");
	std::cout << "Returned from send response file " << ret << std::endl;

}


// NEW SEND_IMAGE
void WebServer::sendImmage(struct kevent& event, std::string imagePath) {
	std::cout << RED "FOUND IMAGE extention .jpg or .png\n" RES;
	unsigned long ret = 0;

	// Stream image and store it into a string
	std::fstream imageFile;
	std::string content;
	imageFile.open(imagePath);

	
	// THIS HAS TO BE CHECKED BEFORE SENDING THE HEADER
	//if (!imageFile) {
		//std::cout << RED "THIS FILE DOES NOT EXIST ON SERVER, SEND THE ERROR PAGE!\n" RES;
		//ret = send(event.ident, "HTTP/1.1 404 Not Found\n"
		//						"Content-Type: text/html\n", 47, 0);
	//}


	content.assign(std::istreambuf_iterator<char>(imageFile), std::istreambuf_iterator<char>());
	content += "\r\n";
	imageFile.close();

	// Send header block
	std::string headerBlock = 	"HTTP/1.1 200 OK\r\n"
								"Content-Type: image/jpg\r\n";
	headerBlock.append("accept-ranges: bytes\r\n");
	std::string contentLen = "Content-Length: ";
	std::string temp = std::to_string(content.size());
	headerBlock.append(contentLen);
	contentLen.append(temp);
	headerBlock.append("\r\n\r\n");
	ret = send(event.ident, headerBlock.c_str(), headerBlock.length(), 0);
	//std::cout << YEL "Image header block sent, ret: " << ret << RES "\n";

	// Send image content and each time reduce the original by ret
	size_t sentSoFar = 0;
	size_t imageSize = content.size();
	for (int i = 0; sentSoFar < imageSize; i++) {
		ret = send(event.ident, content.c_str(), content.size(), 0);
		if (ret == std::string::npos) {
			//std::cout << RED << i << "    Nothing sent (" << ret << RES "),  sentSoFar " << sentSoFar << "\n";
			continue ;
		}
		else {
			content.erase(0, ret);
			sentSoFar += ret;
			//std::cout << YEL << i << "    Sent chunk " << ret << RES ",  sentSoFar " << sentSoFar << "\n";
		}
	}
}


// --------------------------------------------------------- get functions
// -----------------------------------------------------------------------

//int WebServer::getSocket(void)
//{
//	return (_listening_socket);
//}

int WebServer::getKq(void)
{
	return (_kq);
}
