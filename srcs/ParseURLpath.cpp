#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>


#include <sys/types.h>	// send()
#include <sys/socket.h> // send()
#include <sys/stat.h>  // stat()

/*
	setError() ---> shall we immediately set a number, instead of boolean?

	Check if we have accoring to subject: 
		- No send/recv should go outside of kqueue. What about cgi read input and cgi write output???


		- Check with Nginx, how errors work with limited body size, limited querystring size, etc ...
		- Error pages, when cgi has an error, cgi sleeps, ...
				- when page not found, when folder without autoindex

	Evalsheet:


	Curl commands:
		- curl --resolve example.com:80:127.0.0.1 http://example.com/

*/



/*

NORMAL GET PAGE vs RELOAD --------------------
	Normal get page does not send in the header, the field 'If-Modified-Since: Mon, 13 Feb 2023 21:58:52 GMT'
	It returns OK 200

	Reload button sends in the header, 'If-modified-since ...'
	Then, if the page has not been modified, the response will have the code 304: Not modified.
*/

/*
CHatGPT about kqueue
	After creating a kq, you can register file descriptors with it.
	You also register a list of events that a program is interested in: 
				ie:		EV_SET(&event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	Now, the kq monitors this file descriptor for when it will become 'ready for writing'.
		(In case it is already busy writing.)
	
	When the FD becomes 'ready for writing', this is the Event!
	Now the kernel starts notifying the process about this FDs readiness.

	When an event occurs, the kernel provides info about which FD triggered 
	the event, and what type of event occurred.

	In the context of kqueue, an event refers to a change in the state of a file descriptor that our program is interested in monitoring. Examples of events include:

		- Read readiness: the file descriptor has data available for reading.
		- Write readiness: the file descriptor is ready to accept data for writing.
		- Closed: the file descriptor has been closed by the other end.
		- Error: an error has occurred on the file descriptor.
		- etc ...

	You add a file descriptor to a kqueue by using EV_SET() and the EVFILT_WRITE filter.
	This does not yet make the file descriptor 'ready for writing'.
	(Because this FD might be busy for another reason.)

	Now the kernel will monitor the registered FD for the event 'write readiness'.
	When this event occur, the kernel will send a notification to the program (together with additional relevant information, such as the amount of data available for reading ...)

	The event 'write readiness' is triggered when a FD is ready to accept data for writing without blocking. You can now write to this file descriptor without having to wait for it to become available.

	If you don't write anything to this FD, the kernel still keeps notifying the program.
	This can affect CPU usage. So, this event 'write readiness' should only be monitored, when we plan to use write(). Otherwise, this event/filter should be removed from the kqueue.
*/

/*

*/



// #include <sys/types.h>
#include <sys/wait.h>	// for wait() on Linux

#include "../includes/Parser.hpp" // for colors
#include "../includes/RequestParser.hpp"

/*
char* ENV[25] = {
	(char*)"COMSPEC=", (char*)"DOCUMENT_ROOT=", (char*)"GATEWAY_INTERFACE=", (char*)"HTTP_ACCEPT=", (char*)"HTTP_ACCEPT_ENCODING=",             
	(char*)"HTTP_ACCEPT_LANGUAGE=", (char*)"HTTP_CONNECTION=", (char*)"HTTP_HOST=", (char*)"HTTP_USER_AGENT=", (char*)"PATH=",            
	(char*)"QUERY_STRING=", (char*)"REMOTE_ADDR=", (char*)"REMOTE_PORT=", request_method2, (char*)"REQUEST_URI=", (char*)"SCRIPT_FILENAME=",
	(char*)"SCRIPT_NAME=", (char*)"SERVER_ADDR=", (char*)"SERVER_ADMIN=", (char*)"SERVER_NAME=",(char*)"SERVER_PORT=",(char*)"SERVER_PROTOCOL=",     
	(char*)"SERVER_SIGNATURE=", (char*)"SERVER_SOFTWARE=", NULL
};
*/

namespace data {

std::string runExecve(char *ENV[], char *args[], int fdClient) {
	//std::cout << BLU "START runExeve\n" RES;
	//std::cout << "ENV: " << ENV[0] << "\n";

	int    		fd[2];
	
	int    		fdSendBody[2];
	
	pid_t		retFork;
	std::string	incomingStr;

	if (pipe(fd) == -1)
		std::cout << "Error: Pipe failed\n";
	
	//std::cout << "pipe fd[0] " << fd[0] << ", pipe fd[1] " << fd[1] << " \n";
	//dup2(fdClient, fd[1]);
	//std::cout << BLU "POST BODY ENV : " << ENV[2] << "\n" RES;
	// BY HERE, THE HUGE TEXTFILE IS STORED OK


	retFork = fork();

	if (retFork == 0) {
		std::cout << "    Start CHILD execve()\n";
		if (retFork < 0)
			std::cout << "Error: Fork failed\n";

		dup2(fd[1], 1);
		(void)fdClient;
		//dup2(fdClient, fd[1]);
		close(fd[0]);

		// BIG BODY NEEDS TO BE SENT TO THE CGI BY WRITE TO PIPE
		close(fdSendBody[0]);	// close stdout reading from
		dup2(fdSendBody[1], 1);
		int ret2 = write(fdSendBody[1], "Something ..." , 13);
		std::cout << YEL "ret from write to CGI : " << ret2 << "\n" RES;


		// std::cout << YEL "POST BODY ENV : " << ENV[2] << "\n" RES;



		int ret = execve(args[0], args, ENV);
		std::cout << RED "Error: Execve failed: " << ret << "\n" RES;
	}
	else {
		wait(NULL);
		//std::cout << "    Start Parent\n";
		char buff[100];

		close(fd[1]);
		dup2(fd[0], 0);
		//dup2(fdClient, fd[0]);

		close(fdSendBody[1]);
		close(fdSendBody[0]);
		//dup2(fdSendBody[0], 0);

		//std::cout << RED "        Start loop reading from child\n" RES;
		for (int ret = 1; ret != 0; ) {
			memset(buff, '\0', 100);
			ret = read(fd[0], buff, 99);
			incomingStr.append(buff);
		}
		//std::cout << BLU "\n       All content read from CGI\n[" << incomingStr << "]\n" RES;
	}
	return (incomingStr);
}




void Request::callCGI(RequestData reqData, int fdClient) {
	std::cout << RED "START CALL_CGI\n" RES;

	(void)reqData;
	// Declare all necessary variables
	std::string comspec			= "COMSPEC=";
	std::string request_method	= "REQUEST_METHOD=";
	std::string query_string	= "QUERY_STRING=";
	std::string server_name		= "SERVER_NAME=";

	// Declare a vector and fill it with variables, with attached =values
	std::vector<std::string> temp;
	temp.push_back(comspec.append("default"));
	temp.push_back(request_method.append(_data.getRequestMethod()));
	temp.push_back(query_string.append(_data.getQueryString()));
	temp.push_back(server_name.append("default"));

	//std::cout << "Size of vector temp: "<< temp.size() << "\n";
	// std::cout << YEL "POST BODY: " << temp[2] << "\n" RES;
	// BY HERE, THE HUGE BODY IS STORED OK

	// Make a char** array and copy all content of the above vector
	char **env = new char*[temp.size()  + 1];

	size_t i = 0;
	for (i = 0; i < temp.size(); i++) {
	    env[i] = new char[temp[i].length() + 1];
	    strcpy(env[i], temp[i].c_str());
	}
	env[i] = NULL;
	//std::cout << YEL "POST BODY ENV : " << env[2] << "\n" RES;
	// BY HERE, THE HUGE BODY IS STORED OK

	// Just for printing
	//for (i = 0; env[i]; i++) {
	//   std::cout << env[i] << std::endl;
	//}

	// Prepare the array of the correct command/cgi file to be executed
	// The path of the executable must be according to the 'action file' from the URL
	// char *args[2];
	// args[0] = (char *)"./jaka_cgi/cpp_cgi";   // Make sure the path is correct on Mac/Linux
	// args[1] = NULL;


	// char *args[3];
	// args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	// args[1] = (char *)"./jaka_cgi/_somePhp.php"; // MUST BE WITH A DOT !!
	// args[2] = NULL;

	char *args[3];
	args[0] = (char *)"/usr/bin/python";   // Make sure the path is correct on Mac/Linux
	args[1] = (char *)"./resources/cgi/python_cgi.py"; // MUST BE WITH A DOT !!
	args[2] = NULL;

	// (void)ENV;
	// (void)fdClient;
	_data.setCgiBody(runExecve(env, args, fdClient));

	std::cout << "Stored CGI Body: [\n" BLU << _data.getCgiBody() << RES "]\n";

	// Cleanup
	for (size_t j = 0; j < temp.size(); j++) {
	    delete env[j];
	}
	delete[] env;
}



// Some of arguments not used
void printPathParts(std::string str, RequestData reqData) {

	std::cout << "Found path:      [" << BLU << str << RES "]\n";
//	std::cout << "Path trimmed:    [" << BLU << strTrim << RES "]\n";
	std::cout << "Path:            [" << PUR << reqData.getPath() << RES "]\n";
	std::cout << "Path first part: [" << PUR << reqData.getPathFirstPart() << RES "]\n";
	std::cout << "File/Folder:     [" << PUR << reqData.getPathLastWord() << RES "]\n";
	std::cout << "File extention:  [" << PUR << reqData.getFileExtention() << RES "]\n";

	std::map<std::string, std::string> formData;
	formData = reqData.getFormData();

	if (! formData.empty()) {
		std::cout << "\nSTORED FORM DATA PAIRS:\n";// Print the map
		std::map<std::string, std::string>::iterator it;
		for (it = formData.begin(); it != formData.end(); it++)
			std::cout << PUR "   " << it->first << RES " ---> " PUR << it->second << "\n" RES;
	}
	else	
		std::cout << "Form Data:    " << GRE "(not present)\n" RES;
	std::cout << "\n";
}



int checkIfFileExists (const std::string& path) {
	std::ifstream file(path.c_str());

	if (!(file.is_open())) {
		std::cout << RED "Error: File " << path << " not found\n" RES;
		std::cout << RED "THIS FILE DOES NOT EXIST ON SERVER, SEND THE ERROR PAGE!\n" RES;
		return (-1);
	}
	std::cout << GRN "File/folder " << path << " exists\n" RES;
	return 0;
}


int	checkIfFileOrFolder(std::string path) {
	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) == 0) {
		if (S_ISREG(path_stat.st_mode)) {
			std::cout << GRN "Path is a file\n" RES;
			return (1);
		} else if (S_ISDIR(path_stat.st_mode)) {
			std::cout <<  GRN "Path is a folder\n" RES;
			return (2);
		} else {
			std::cout <<  GRN "Path is neither a filer nor s folder\n" RES;
			return (-1);
		}
	} else {
		std::cout <<  RED "Error: failed to stat path\n" RES;
		return (-1);
	}
	//return (0);
}



int Request::checkTypeOfFile() {
	std::cout << GRN "Start checkTypeofFile(), path [" << _data.getPath() << "\n" RES;
	
	std::string path = _data.getPath();
	std::string temp = _data.getPath();

	if (path[0] == '.')
		temp = path.substr(1, std::string::npos);

	std::size_t found = temp.find_last_of(".");

	if (found != std::string::npos) {
		// std::string extention = temp.substr(found, std::string::npos);
		_data.setFileExtention(temp.substr(found, std::string::npos));
	}
	else
		std::cout << GRN "There is no extention in the last name\n" RES;
	return (0);
}



// Not in use
// There is a read buffer overflow
std::string removeDuplicateSlash(std::string pathOld) {
  
	char *temp = (char *)malloc(pathOld.length() * sizeof(char) + 1);
	if (temp == NULL)
	{ std::cout << "Error: removeDuplicate failed mallocing\n";  exit(-1); }  

	int beginOfQuery = false;
	size_t j = 0, i = 0;
	while (i < pathOld.length()) {
		if (pathOld[i] == '/' && pathOld[i - 1] == '/' && i != 0 && beginOfQuery == false) {
			i++;
			continue ;
		}
		if (pathOld[i] == '?')
			beginOfQuery = true;
		temp[j++] = pathOld[i++];
	}
	temp[j] = '\0';
	std::string pathNew(temp);
	free(temp);

	if (pathNew[0] == '/')	{
		std::string prefix = "."; // Not sure why this was necessary ???
		prefix.append(pathNew);
		return prefix;
	}
	return pathNew;
}



/* 	Split string at '&' and store each line into vector<>
	Then split each line in vector into map<> key:value */
// std::map<std::string, std::string> Request::storeFormData(std::string &queryString)
std::map<std::string, std::string> Request::storeFormData(std::string queryString)
{
	std::cout << GRN "Start store form data()\n" RES;
	//std::cout << GRN "    BODY:        ["   << _body << "]\n" RES;
	//std::cout << GRN "    queryString:   [" <<  queryString << "]\n" RES;
	//std::cout << GRN "   _queryString:   [" << _data.getQueryString() << "]\n" RES;

	std::string					line;
	std::vector<std::string>	formList;

	std::stringstream iss(queryString);
	while (std::getline(iss, line, '&'))
		formList.push_back(line);

	_data.setFormList(formList);

	// MAYBE THE FORM MAP WILL NOT BE NEEDED
	std::string							key, val;
	std::map<std::string, std::string>	formDataMap;
	std::vector<std::string>::iterator	it;

	for (it = formList.begin(); it != formList.end(); it++) {
		std::stringstream iss2(*it);								// maybe change name, or reuse the above variable
	 	std::getline(iss2, key, '=') >> val;
		formDataMap[key] = val;
		//std::cout << YEL "  ... vector [" << *it << "]\n" RES;
	}
	_data.setFormData(formDataMap);
	return (formDataMap);
}


// !!! Not storing correctly the path part and file name!
// If last '/' is found in path, then this is a folder, not file
void	Request::storePath_and_FolderName(std::string path) {

	size_t 	pos1	= 0;
	size_t	pos2	= 0;
	size_t 	count	= 0;

	std::cout << CYN "Start storePath_and_FolderName(}" << path << "\n" RES;

	// Check if there is query '?' and store path before it
	// Probably not needed searching for query here, because the method is POST,  so query not possible
	_data.setPath(path);
	pos1 = path.find_first_of("?");
	if (pos1 != std::string::npos)
		_data.setPath(path.substr(0, pos1));

	pos1	= 0;
	pos2	= path.find_first_of("/");
//	pos2	= path.find_last_of("/");
	while (count < path.length()) {
		if ((count = path.find("/", count)) != std::string::npos) {
			pos1 = pos2;
			pos2 = count;
		}
		if ( count == std::string::npos )
			break ;
		count++;
	}
	_data.setPathFirstPart(path.substr(0, pos1 + 1));
	_data.setPathLastWord(path.substr(pos1 + 1, pos2));
}



// Found GET Method with '?' Form Data
void	Request::storePathParts_and_FormData(std::string path) {

	int			temp		= path.find_first_of("?");
	std::string tempStr		= path.substr(0, temp);

	_data.setPath(path.substr(0, temp));
	//std::cout << CYN "StorePathParts() " << _data.getPath() << "\n" RES;
	int posLastSlash 		= tempStr.find_last_of("/");
	int	posFirstQuestMark	= path.find_first_of("?");
	std::string	queryString	= path.substr(temp, std::string::npos);

	_data.setPathFirstPart(tempStr.substr(0, posLastSlash));
	_data.setPathLastWord(path.substr(posLastSlash, posFirstQuestMark - posLastSlash));

	if (queryString[0] == '?') 	// Skip the '?' in the path
		queryString = &queryString[1];

	if (_data.getRequestMethod() == "GET")
		_data.setQueryString(queryString);

	std::cout << "Stored GET _queryString [\n" << BLU << _data.getQueryString() << RES "]\n";
	std::cout << "Stored GET _body [\n" << BLU << _data.getBody() << RES "]\n" RES;
	
	storeFormData(queryString);	// maybe not needed (the whole vector and map)
								// if the cgi script can handle the whole queryString
}



int Request::parsePath(std::string str, int fdClient) {

	std::cout << "    start parse path: [" << str << "]\n";	// sleep(1);
//	std::string path			= removeDuplicateSlash(str);	// here error: read buffer overflow
	std::string path			= str;
	size_t		ret				= 0;
//	std::string pathLastWord	= "";
	

	if (path == "")
		return (-1);
	if (path[0] == '/')
		path = "." + path;

	if (path == "./") {
		std::cout << GRN "Path is the root '/'\n" RES;
	}
	if (path.back() == '/'  && (path.find("?") == std::string::npos)) {
		std::cout << GRN "The path has no GET-Form data. Last char is '/', it must be a folder.\n" RES;
		storePath_and_FolderName(path);
	}

	// if the last char is not slash /   then look for question mark 
	// else if ((ret = path.find("?")) == std::string::npos ) {
	else if ((ret = path.find("?")) == std::string::npos && _data.getRequestMethod() != "POST") {
		std::cout << GRN "There is no GET or POST method, also the '?' not found\n" RES;
		_data.setPath(path);
		int pos			= 0;
		pos				= path.find_last_of("/");	
		_data.setPathFirstPart(path.substr(0, pos));
		_data.setPathLastWord(path.substr(pos, std::string::npos));
	}
	
	else if ((ret = path.find("?")) != std::string::npos) {			// Found '?' in the path, maybe also check != "POST"
		std::cout << GRN "There is GET Form data, the '?' is found\n" RES;
		storePathParts_and_FormData(path);
	}

	else if (_data.getRequestMethod() == "POST") {
		std::cout << GRN "There is POST Form data\n" RES;
		storePath_and_FolderName(path);	// Not sur if this good here ???
										// path is not extracted correctly
		// _data.setQueryString(getRequestBody());
		_data.setQueryString(_data.getBody());
	}

	//std::cout << GRN "XXX)\n" RES;
	if (checkIfFileExists(_data.getPath()) == -1) {	// What in case of root only "/"  ???
		setError(true);								// What in case of GET??
	}
	checkTypeOfFile();
	
	if (checkIfFileOrFolder(_data.getPath()) == -1) {
		setError(true);
	}

	printPathParts(str, getRequestData());

	(void)fdClient;
	//callCGI(getRequestData(), fdClient);
	return (0);
}

} // namespace data
