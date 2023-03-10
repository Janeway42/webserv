#ifndef WEBSERVER_CPP
#define WEBSERVER_CPP

// ---- C ----
#include <unistd.h>
#include <sys/time.h>

// ---- kqueue ----
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/event.h>

#include "RequestParser.hpp"
#include "ConfigFileParser.hpp"

#define MAX_EVENTS 100
#define BUFFER_SIZE 50

class WebServer
{
	private:
		int _kq;
        std::vector<ServerData> _servers;

	public:
        // todo add a default constructor? -> maybe private so no one can call it
        WebServer(std::string const & configFileName);
		~WebServer(void);

		// main functions 
		void runServer();
		void readRequest(struct kevent& event);
		void sendResponse(struct kevent& event);
		void handleTimeout(struct kevent &event);
		void newClient(struct kevent event, ServerData * specificServer);
		void addFilter(int fd, struct kevent& event, int filter, std::string errormMsesage);
		void removeFilter(struct kevent& event, int filter, std::string errorMessage);
		void closeClient(struct kevent& event);
		
		// utils 
		bool isListeningSocket(int fd);
		ServerData * getSpecificServer(int fd);

		void sendProcesssedResponse(struct kevent& event);
		std::string streamFile(std::string file);
		void sendResponseFile(struct kevent& event, std::string file);
		void sendImmage(struct kevent& event, std::string imgFileName);
		// int closeClient(struct kevent event, int filter);

		// getters
		int getKq();  // not actually used anywhere 

        class ServerException: public std::exception
        {
            private:
                std::string _errorMessage;

            public:
                ServerException(std::string message) throw()
                {
                    _errorMessage = message;
                }
                virtual const char* what() const throw()
                {
                    return (_errorMessage.c_str());
                }
                virtual ~ServerException() throw(){}
        };
};

#endif //WEBSERVER_CPP
