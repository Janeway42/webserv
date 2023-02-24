#include "Server.hpp"


#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>

Server::Server()
{
	struct addrinfo *_addr = new struct addrinfo();
	struct addrinfo hints;

	hints.ai_family = PF_UNSPEC; 
	hints.ai_flags = AI_PASSIVE; 
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo("127.0.0.1", "8080", &hints, &_addr) != 0)
		throw ServerException(("failed addr"));

	_listening_socket = socket(_addr->ai_family, _addr->ai_socktype, _addr->ai_protocol);
	if (_listening_socket < 0)
		throw ServerException(("failed socket"));
	fcntl(_listening_socket, F_SETFL, O_NONBLOCK);

	int socket_on = 1;
	setsockopt(_listening_socket, SOL_SOCKET, SO_REUSEADDR, &socket_on, sizeof(socket_on));
	if (bind(_listening_socket, _addr->ai_addr, _addr->ai_addrlen) == -1)
		throw ServerException(("failed bind"));
	if (listen(_listening_socket, SOMAXCONN) == -1)  // max nr of accepted connections 
		throw ServerException(("failed listen"));

	struct kevent evSet;
	_kq = kqueue();

	std::cout << "Server _kq " << _kq << "\n";

	if (_kq == -1)
		throw ServerException(("failed kq"));

	EV_SET(&evSet, _listening_socket, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_WRITE	, 0, NULL);
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
		throw ServerException(("failed kevent start"));
}

Server::~Server()
{
	close(_kq);
	freeaddrinfo(_addr);
}

// --------------------------------------------------------- server main loop
// --------------------------------------------------------------------------

void Server::runServer()
{
	struct kevent evList[MAX_EVENTS];
//	struct kevent evSet;
	int i;
//	struct sockaddr_storage socket_addr;
//	socklen_t socklen = sizeof(socket_addr);

	int loop1 = 0;
	while (1)
	{
	//	std::cout << "WHILE LOOP ------------------------------" << loop1 << std::endl;
		int nr_events = kevent(_kq, NULL, 0, evList, MAX_EVENTS, NULL);
	//	std::cout << "runServer errno " << errno << "\n";
	//	std::cout << "NR EVENTS: " << nr_events << std::endl;

		if (nr_events < 1)
			throw ServerException("failed number events");
		else
		{
			for (i = 0; i < nr_events; i++)
			{
				// std::cout << "filter: " << evList[i].filter << std::endl; // test line 

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
		loop1++;
	}
}

// --------------------------------------------------------- server functions 
// --------------------------------------------------------------------------

void Server::readRequest(struct kevent& event)
{
	char buffer[50];
	memset(&buffer, '\0', 50);
	data::Request *storage = (data::Request *)event.udata;

	int ret = recv(event.ident, &buffer, sizeof(buffer) - 1, 0);
//	std::cout << "bytes read: " << ret << std::endl;                 // test line 
	if (ret < 0)
	{
		// if (storage->getEarlyClose() == false) // if it fails 
		// {
			std::cout << "failed recv\n";
			storage->setError(true);
			if (removeEvent(event, EVFILT_READ) == 1)
				throw ServerException("failed EVFILT_READ removal\n");
			std::cout << "send error message back to client\n";
		// }
		// else
		// 	std::cout << "EVFILT_READ already closed\n";  // if it has been closed in write due to timeout  - test line
	}

	// for cgi ret might be 0 - might need to add later, but for kqueue it will never be 0 

	else if (storage->getEarlyClose() == false && storage->getDone() == false)
	{
		storage->appendToRequest(buffer, event.ident);
		//std::cout << "After append buffer\n";

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

void Server::sendResponse(struct kevent& event)
{
	data::Request *storage = (data::Request *)event.udata;

	std::time_t elapsedTime = std::time(NULL);
	if (elapsedTime - storage->getTime() > 30)
	{
		std::cout << "Unable to process request, it takes too long!\n";
		storage->setError(true);
		storage->setEarlyClose(true);
		// if (removeEvent(event, EVFILT_READ) == 1)
		// 	throw ServerException("failed EVFILT_READ removal\n"); 
	}

	if (storage->getError() == true)
	{
		sendResponseFile(event, "error404.html");
		if (removeEvent(event, EVFILT_WRITE) == 1)
			throw ServerException("failed kevent EV_DELETE client - send error");
		closeClient(event);
		std::cout << "closed connection from write - error\n";
	}
	else if (storage->getError() == false && storage->getDone() == true)
	{
		std::string temp = (storage->getRequestData()).getHttpPath();
		std::cout << "Done: " << storage->getDone() << " Error: " << storage->getError() << std::endl;

		// if the image is too big (more than 200kb), the return from send is random
		//		and almost always lower than orig img size ...
		// How to send a big file via send(), if it only allows 65kb max?
		// How to send multiple sends(), if kqueue should only get one send() ???

		if (temp.find(".png") != std::string::npos)
			//sendImmage(event, "images/img_99kb.jpg");		// ok sent
			// sendImmage(event, "images/img_109kb.jpg");	// ok sent
			//sendImmage(event, "images/img_938kb.jpg");	// ok sent
			//  sendImmage(event, "images/img_5000kb.jpg");	// no
			  sendImmage(event, "images/img_13000kb.jpg");// no
		else 
			//sendResponseFile(event, "./html_files/index_just_text.html");
			sendResponseFile(event, "./html_files/index_just_image.html");
			// sendResponseFile(event, "index_post_form.html");
			// sendResponseFile(event, "index_get_form.html");
			// sendResponseFile(event, "index_dummy.html");

		if (removeEvent(event, EVFILT_WRITE) == 1)
			throw ServerException("failed kevent EV_DELETE client - send success");
		closeClient(event);
		std::cout << "closed connection from write - done\n";
	}
}

// --------------------------------------------------------- client functions 
// --------------------------------------------------------------------------

void Server::newClient(struct kevent event)
{
	struct kevent evSet;
	struct sockaddr_storage socket_addr;
	socklen_t socklen = sizeof(socket_addr);

	std::cout << "make new client connection\n";
	
	int opt_value = 1;
	int fd = accept(event.ident, (struct sockaddr *)&socket_addr, &socklen);

	std::cout << "newClient fd " << fd << "\n";

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

void Server::closeClient(struct kevent& event)
{
	data::Request *storage;
	storage = (data::Request *)event.udata; 

	close(event.ident); 
	std::cout << "connection closed\n";
	delete(storage);
}

// --------------------------------------------------------- utils functions
// -------------------------------------------------------------------------

int Server::removeEvent(struct kevent& event, int filter)
{
	 std::cout << "REMOVEEVENT" << errno << std::endl;    

	struct kevent evSet;
	data::Request *storage = (data::Request *)event.udata;

	EV_SET(&evSet, event.ident, filter, EV_DELETE, 0, 0, storage); 
	if (kevent(_kq, &evSet, 1, NULL, 0, NULL) == -1)
	{
		std::cout << "erno: " << errno << "\n";    
		std::cout << "filter: " << filter << "\n";                     // test line - to be removed 
		throw ServerException("failed kevent EV_DELETE client");
	}
	return (0);
}

std::string Server::streamFile(std::string file)
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

void Server::sendResponseFile(struct kevent& event, std::string file)
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
	std::cout << "ret: " << ret << std::endl;

}





// NEW SEND IMAGE, ATTEMPT WITH MEMCPY
void Server::sendImmage(struct kevent& event, std::string imgFileName)
{
	std::cout << RED "FOUND extention .jpg or .png\n" RES;

	FILE *file;
	char *buffer;
	unsigned long imageSize;

	file = fopen(imgFileName.c_str(), "rb");
	if (!file)
	{
		std::cerr << "Unable to open file\n";
		return ;
 	}

	fseek(file, 0L, SEEK_END);	// Get file length
	imageSize = ftell(file);
	fseek(file, 0L, SEEK_SET);

	// NEW FROM CLAUDE
	std::fstream file2;
	std::string content;
	file2.open(imgFileName);
	content.assign(std::istreambuf_iterator<char>(file2), std::istreambuf_iterator<char>());
	file2.close();
	content += "\r\n";

	// send(event.ident, content.c_str(), content.size(), 0);
	//read(sock, buffer, 4095);
	// END CLAUDE

	//imageSize = 65000;

	std::cout << YEL "ImageSize from fseek:   " << imageSize << RES "\n";
	// 13482898

	std::string headerBlock = 	"HTTP/1.1 200 OK\r\n"
								"Content-Type: image/jpg\r\n";
	headerBlock.append("accept-ranges: bytes\r\n");
	std::string contentLen = "Content-Length: ";
	std::string temp = std::to_string(imageSize);
	headerBlock.append(contentLen);
	contentLen.append(temp);
	headerBlock.append("\r\n\r\n");

	buffer = (char *)malloc(imageSize);
	if (!buffer)
		{ fprintf(stderr, "Memory error!"); fclose(file); return ; }


	unsigned long ret;
	ret = fread(buffer, sizeof(char), imageSize, file);
	std::cout << YEL "Returned fread:         " << ret << RES "\n";

	char *buffer_2;
	buffer_2 = (char *)malloc(imageSize);
	memcpy(buffer_2, buffer, imageSize);

	// HEADER BLOCK
	ret = send(event.ident, headerBlock.c_str(), headerBlock.length(), 0);
	std::cout << YEL "Image sent a, returned from send() image: " << ret << RES "\n";


		!!! finished here
		// Seems that sending image in a loop is working. It needs to stop exactly when all bytes 
		// have been sent. 
		// Sometimes it still does not come the whole image !!!
		// Can I keep calling send() in case it keeps returning npos? ??

	// for (unsigned long newImgSize = imageSize; newImgSize > 0; newImgSize = newImgSize - ret) {
		//size_t newImgSize = imageSize;
		ret = 0;
		size_t byteInImage = 0;
	for (unsigned long i = 0; i < 300; i++) {

		// ret = send(event.ident, reinterpret_cast <const char* >(buffer), imageSize, 0);
		ret = send(event.ident, reinterpret_cast <const char* >(&buffer[byteInImage]), 40000, 0);
		//sleep(1);
		if (ret != std::string::npos)
			byteInImage = byteInImage + ret;							//4.992.867
		std::cout << YEL "i" << i << "  b) Image sent " << ret << " bytes, all: " << byteInImage << RES "\n";
		if (ret <= 0)
			break ;
	}






	//ret = send(event.ident, content.c_str(), content.size(), 0);
	//std::cout << YEL "Image sent c (claude), returned from send() image: " << ret << RES "\n";



	fclose(file);
	free(buffer);
	free(buffer_2);
}




// ORIG
// void Server::sendImmage(struct kevent& event, std::string imgFileName)
// {
// 	std::cout << RED "FOUND extention .jpg or .png\n" RES;

// 	FILE *file;
// 	unsigned char *buffer;
// 	unsigned long imageSize;

// 	file = fopen(imgFileName.c_str(), "rb");
// 	if (!file)
// 	{
// 		std::cerr << "Unable to open file\n";
// 		return ;
//  	}

// 	fseek(file, 0L, SEEK_END);	// Get file length
// 	imageSize = ftell(file);
// 	fseek(file, 0L, SEEK_SET);

// 	//std::cout << YEL "ImageSize from fseek:   " << imageSize << RES "\n";
// 	// 13482898


// 	std::string temp = std::to_string(imageSize);
// 	std::string contentLen = "Content-Length: ";
// 	contentLen.append(temp);
// 	contentLen.append("\r\n");

// 	std::string headerBlock = 	"HTTP/1.1 200 OK\r\n"
// 								"Content-Type: image/jpg\r\n";
// 	headerBlock.append(contentLen);
// 	headerBlock.append("accept-ranges: bytes");
// 	headerBlock.append("\r\n\r\n");

// 	buffer = (unsigned char *)malloc(imageSize);
// 	if (!buffer)
// 		{ fprintf(stderr, "Memory error!"); fclose(file); return ; }

// 	int ret = fread(buffer, sizeof(unsigned char), imageSize, file);
// 	std::cout << YEL "Returned fread:     " << ret << RES "\n";
	
// 	ret = send(event.ident, headerBlock.c_str(), headerBlock.length(), 0);
// 	std::cout << YEL "Image sent a, returned from send() image: " << ret << RES "\n";

// 	ret = send(event.ident, reinterpret_cast <const char* >(buffer), imageSize, 0);
// 	std::cout << YEL "Image sent b, returned from send() image: " << ret << RES "\n";
// 	fclose(file);
// 	free(buffer);
// }

// --------------------------------------------------------- get functions
// -----------------------------------------------------------------------

int Server::getSocket(void)
{
	return (_listening_socket);
}

int Server::getKq(void)
{
	return (_kq);
}
