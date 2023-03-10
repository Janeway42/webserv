#ifndef CGIDATA_HPP
#define CGIDATA_HPP

#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/time.h>

#include "Parser.hpp"

class CgiData: public Parser
{
	private:
		int _fd_in[2];  // we write to cg  - we write on _fd_in[1]
		int _fd_out[2]; // we read from cgi - we read from _fd_out[0]
		int _bytesToCgi;
		bool _pipesDone;

	public:
		CgiData();
		virtual ~CgiData();

		void createPipes(int kq, struct kevent & event);

		// getters
		int		getBytesToCgi();
		int		getPipeCgiIn();
		int		getPipeCgiOut();
		bool	getPipesDone();

		// setters
		void setPosition(int val);
		void setPipesDone(bool val);
		void setBytesToCgi(int sent);

		// utils 
		bool checkFdCgi(struct kevent & event, int filter);
		void closeFileDescriptors(struct kevent & event);


		class CgiException: public std::exception
        {
            private:
                std::string _errorMessage;

            public:
                CgiException(std::string message) throw()
                {
                    _errorMessage = message;
                }
                virtual const char* what() const throw()
                {
                    return (_errorMessage.c_str());
                }
                virtual ~CgiException() throw(){}
        };
};

#endif
