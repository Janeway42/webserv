#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>


#include <sys/stat.h>	// stat()



/*
	What happens if you dont have a form on your page, but you directly write ?city=aaa in the URL?
	In this case, no action file is specified ???
*/

// #include <sys/types.h>
#include <sys/wait.h>	// for wait() on Linux

#include "../includes/Parser.hpp" // for colors
#include "../includes/RequestParser.hpp"




void Request::runExecve(char *ENV[], char *args[], struct kevent event) {
	//std::cout << BLU << "START runExeve\n" << RES;
	(void)event;

	// Create pipes
	_cgi.createPipes(_data.getKqFd(), event);

	int ret = 0;
	pid_t		retFork;

	retFork = fork();

	if (retFork == 0) { // CHILD
		std::cout << RED "Start CHILD execve()\n" RES;
		if (retFork < 0)
			std::cout << "Error: Fork failed\n";
	
		close(_cgi.getPipeCgiOut_0());
		close(_cgi.getPipeCgiIn_1());

		ret = dup2(_cgi.getPipeCgiIn_0()   ,  0);		// cgi reads from parent via pipe fd_out
		if (ret == -1)
		 	std::cout << RED "Error dup2() of PipeCgiIn_0, child\n" RES;
		close(_cgi.getPipeCgiIn_0());
		
		//sleep(1);
		ret = dup2(_cgi.getPipeCgiOut_1()   ,  1);	// cgi writes to parent via pipe fd_out NONBLOCK
		if (ret == -1)
		 	std::cout << RED "Error dup2() of PipeCgiOut_1, child\n" RES;
		close(_cgi.getPipeCgiOut_1());

		std::cerr << RED "Before execve in child\n" << RES;
		ret = execve(args[0], args, ENV);
	//	ret = execv(args[0], const_cast<char**>(args));
		std::cerr << RED << "Error: Execve failed: " << ret << "\n" << RES;
	}
	else {				// PARENT
		//wait(NULL);
		
		std::cerr << "    Start Parent\n";
		close(_cgi.getPipeCgiOut_1());
		close(_cgi.getPipeCgiIn_0());
		//std::cout << BLU "\n       End runExecve()\n" << RES;
		sleep(1);
	}
	return ;	// just ""  , func can be set to void
}




void Request::callCGI(struct kevent event) {
	std::cout << RED << "START CALL_CGI, cgi path: " << _data.getPath() << "\n" << RES;
	//(void)reqData;

	// Declare all necessary variables
	std::string comspec			= "COMSPEC=";
	std::string request_method	= "REQUEST_METHOD=";
	std::string query_string	= "QUERY_STRING=";
	std::string server_name		= "SERVER_NAME=";
	std::string content_length	= "CONTENT_LENGTH=";

	// Declare a vector and fill it with variables, with attached =values
	std::vector<std::string> temp;
	temp.push_back(comspec.append("default"));
	temp.push_back(request_method.append(_data.getRequestMethod()));
	temp.push_back(query_string.append(_data.getQueryString()));
	temp.push_back(server_name.append("default"));

	std::stringstream ss;	// convert length int to string variable
	ss << _data.getRequestContentLength();
	temp.push_back(content_length.append(ss.str()));

	// std::cout << "Size of vector temp: " << temp.size() << "\n";
	// std::cout << YEL << "POST BODY: " << temp[2] << "\n" << RES;
	// BY HERE, THE HUGE BODY IS STORED OK

	// Make a char** array and copy all content of the above vector
	char **env = new char*[temp.size()  + 1];

	size_t i = 0;
	for (i = 0; i < temp.size(); i++) {
		env[i] = new char[temp[i].length() + 1];
		strcpy(env[i], temp[i].c_str());
	}
	env[i] = NULL;
	//std::cout << YEL << "POST BODY ENV : " << env[2] << "\n" << RES;
	// BY HERE, THE HUGE BODY IS STORED OK

	// Just for printing
	//for (i = 0; env[i]; i++) {
	//   std::cout << env[i] << std::endl;
	//}

	// char *args[3];
	// args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	// args[1] = (char *)"./jaka_cgi/_somePhp.php"; // MUST BE WITH A DOT !!
	// args[2] = NULL;

	char *args[3];
	args[0] = (char *)"/usr/bin/python";
	std::string tempPath = _data.getPath();
	char *path = (char *)tempPath.c_str();	//  ie: "./resources/cgi//python_cgi_GET.py"
	args[1] = path;
	args[2] = NULL;

	// (void)ENV;
	// (void)fdClient;
	//_data.setCgiBody(runExecve(env, args, event));
	runExecve(env, args, event);

	//std::cout << "Stored body from CGI: [\n" << BLU << _data.getCgiBody() << RES << "]\n";

	// Cleanup
	for (size_t j = 0; j < temp.size(); j++) {
		delete env[j];
	}
	delete[] env;
	//std::cout << BLU "\n       End callCGI()\n" << RES;

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

////////////////////////////////////////////////////////////////


/* 	Split string at '&' and store each line into vector<>
	Then split each line in vector into map<> key:value */
// std::map<std::string, std::string> Request::storeFormData(std::string &queryString)
std::map<std::string, std::string> Request::storeFormData(std::string queryString)
{
	std::cout << GRN << "Start store form data()\n" << RES;
	//std::cout << GRN << "    BODY:        ["   << _body << "]\n" << RES;
	//std::cout << GRN << "    queryString:   [" <<  queryString << "]\n" << RES;
	//std::cout << GRN << "   _queryString:   [" << _data.getQueryString() << "]\n" << RES;

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
		//std::cout << YEL << "  ... vector [" << *it << "]\n" << RES;
	}
	_data.setFormData(formDataMap);
	return (formDataMap);
}



// Found GET Method with '?' Form Data
void Request::storePathParts_and_FormData(std::string path) {

	int			temp		= path.find_first_of("?");
	std::string tempStr		= path.substr(0, temp);

	_data.setPath(path.substr(0, temp));
	//std::cout << CYN "StorePathParts() " << _data.getPath() << "\n" << RES;
	int posLastSlash 		= tempStr.find_last_of("/");
	int	posFirstQuestMark	= path.find_first_of("?");
	std::string	queryString	= path.substr(temp, std::string::npos);

	_data.setPathFirstPart(tempStr.substr(0, posLastSlash));
	_data.setPathLastWord(path.substr(posLastSlash, posFirstQuestMark - posLastSlash));

	if (queryString[0] == '?') 	// Skip the '?' in the path
		queryString = &queryString[1];

	if (_data.getRequestMethod() == "GET") {
		_data.setQueryString(queryString);
		// _data.setBody(queryString);  // too early
	}

	std::cout << "Stored GET _queryString [\n" << BLU << _data.getQueryString() << RES << "]\n";
	std::cout << "Stored GET _body [\n"        << BLU << _data.getBody() << RES << "]\n" << RES;

	storeFormData(queryString);	// maybe not needed (the whole vector and map)
	// if the cgi script can handle the whole queryString
}



// !!! Not storing correctly the path part and file name!
// If last '/' is found in path, then this is a folder, not file
void Request::storePath_and_FolderName(std::string path) {

	size_t 	pos1	= 0;
	size_t	pos2	= 0;
	size_t 	count	= 0;

	std::cout << CYN "Start storePath_and_FolderName(}" << path << "\n" << RES;

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

/*
char* ENV[25] = {
	(char*)"COMSPEC=", (char*)"DOCUMENT_ROOT=", (char*)"GATEWAY_INTERFACE=", (char*)"HTTP_ACCEPT=", (char*)"HTTP_ACCEPT_ENCODING=",
	(char*)"HTTP_ACCEPT_LANGUAGE=", (char*)"HTTP_CONNECTION=", (char*)"HTTP_HOST=", (char*)"HTTP_USER_AGENT=", (char*)"PATH=",
	(char*)"QUERY_STRING=", (char*)"REMOTE_ADDR=", (char*)"REMOTE_PORT=", request_method2, (char*)"REQUEST_URI=", (char*)"SCRIPT_FILENAME=",
	(char*)"SCRIPT_NAME=", (char*)"SERVER_ADDR=", (char*)"SERVER_ADMIN=", (char*)"SERVER_NAME=",(char*)"SERVER_PORT=",(char*)"SERVER_PROTOCOL=",
	(char*)"SERVER_SIGNATURE=", (char*)"SERVER_SOFTWARE=", NULL
};
*/


int Request::checkTypeOfFile() {
	std::cout << GRN << "Start checkTypeofFile(), path [" << _data.getPath() << "] " RES;

	std::string path = _data.getPath();
	std::string temp = _data.getPath();


	// CHECK IF THE PATH IS A FILE OR FOLDER, REGARDLES IF IT HAS ANY EXTENTION
    // TODO: USE THE FUNCTION INSIDE PARSER.CPP
	struct stat s;
    if (stat(path.c_str(), &s) == 0) {
        if (S_ISDIR(s.st_mode)) {
            std::cout << CYN "is a directory\n";
			_data.setIsFolder(true);
			// return (0);
        } else if (S_ISREG(s.st_mode)) {
            std::cout << CYN "is a file\n" RES;
        } else {
            std::cout << CYN "is not a valid directory or file\n" RES;
        }
    } else {
        std::cerr << RED << "Error getting file/directory info: " << strerror(errno) << "\n" RES;
    }



	// IF IT IS A FILE, CHECK AND STORE THE SUFFIX
	if (path[0] == '.')
		temp = path.substr(1, std::string::npos);

	std::size_t found = temp.find_last_of(".");

	if (found != std::string::npos) {
		_data.setFileExtention(temp.substr(found, std::string::npos));
	}
	else
		std::cout << GRN << "There is no extention in the last name\n" << RES;
	return (0);
}


// Some arguments not used
static void printPathParts(std::string str, RequestData reqData) {

	std::cout << "Found path:      [" << BLU << str << RES << "]\n";
//	std::cout << "Path trimmed:    [" << BLU << strTrim << RES << "]\n";
	std::cout << "Path:            [" << PUR << reqData.getPath() << RES << "]\n";
	std::cout << "Path first part: [" << PUR << reqData.getPathFirstPart() << RES << "]\n";
	std::cout << "File/Folder:     [" << PUR << reqData.getPathLastWord() << RES << "]\n";
	std::cout << "File extention:  [" << PUR << reqData.getFileExtention() << RES << "]\n";
	std::cout << "Body:            [" << PUR << reqData.getBody() << RES << "]\n";

	std::map<std::string, std::string> formData;
	formData = reqData.getFormData();

	if (! formData.empty()) {
		std::cout << "\nSTORED FORM DATA PAIRS:\n";// Print the map
		std::map<std::string, std::string>::iterator it;
		for (it = formData.begin(); it != formData.end(); it++)
			std::cout << PUR << "   " << it->first << RES << " ---> " << PUR << it->second << "\n" << RES;
	}
	else
		std::cout << "Form Data:    " << YEL << "(not present)\n" << RES;
	std::cout << "\n";
}

static int checkIfPathExists(const std::string& path, struct kevent event) {
	
	(void)event;
	std::cout << GRN << "Start CheckIfFIleExists(), path [" << path << "] \n" << RES;

	
	std::ifstream file(path.c_str());

	if (not file.is_open()) {		// ??? what is this syntax? -> joyce for cpp we can use not in the pace of ! for readability :)
		std::cout << RED << "Error: File " << path << " not found\n" << RES;
		return (404);
	}
	std::cout << GRN << "File/folder " << path << " exists\n" << RES;


	// CHECK IF PATH MATCHES THE SERVER ROOT FOLDER
	// Request *storage = (Request*)event.udata;
	// if (path == storage->getServerData().getRootDirectory()) {
	// 	std::cout << GRN << "Path is the root folder\n" << RES;
	// 	return (0);
	// }

	// LOOP THROUGH LOCATIONS AND CHECK IF THERE IS A MATCH, OTHERWISE ERROR 404
	// std::vector<ServerLocation> location_data_vector = storage->getServerData().getLocationBlocks();
	// size_t i;
	// for (i = 0; i < location_data_vector.size(); i++) {
	// 	std::cout << GRE "   ........ location uri: [" << location_data_vector[i].getLocationUriName()) << "]\n";
	// 	std::cout << GRE "   ... location root dir: [" << location_data_vector[i].getRootDirectory() << "]\n";
	// 	std::cout << GRE "   ....... _responsePath: [" << location_data_vector[i].getRootDirectory() << "]\n";
	// 	if (location_data_vector[i].getRootDirectory() == path) {// TODO here it should be getLocationUriName() ?? talk to joyce
	// 		path = location_data_vector[i].getIndexFile();
	// 		std::cout << BLU "   ....... FinalPath: [" << path << "]\n";
	// 	}
	// }
	// if (i == location_data_vector.size()) {
	// 	std::cout << RED "This path exists but does not match any location: [" << path << "]\n";
	// 	storage->setHttpStatus(NOT_FOUND);
	// }

	return 0;
}


/*
	For Joyce: I think there is no separate getter, just to get the path to the cgi location,
	without having to loop through all locations.
	Maybe it is usefull to have a separate getter() for the cgi path, or just folder.
	in line ***** a)
*/

// int Request::parsePath(std::string str) {
int Request::parsePath(std::string str, struct kevent event) {

	std::cout << GRN "Start parse path: [" << str << "]\n";	// sleep(1);
//	std::string path			= removeDuplicateSlash(str);	// here error: read buffer overflow
	std::string path			= str;
	size_t		ret				= 0;
//	std::string pathLastWord	= "";

// TODO CHECK IF path == the location block paths (getLocationUriName())
	if (path == "")
		return (-1);
	if (path[0] == '/' && path != "/" && path.find("?") == std::string::npos && _data.getRequestMethod() != "POST") {		// no CGI path needed
		std::cout << GRN << "  No '?', no cgi path needed\n" RES;
		path = getServerData().getRootDirectory() + path;// TODO 	SHOULD BE PRE PENDED WITH THE ROOT DIRECTORY OF THE LOCATION, NOT THE PATH FROM THE REQUEST
	}
	if ((path[0] == '/' && path != "/" && path.find("?") != std::string::npos)	 	// yes, CGI path needed
		|| _data.getRequestMethod() == "POST"   ) {
		std::cout << GRN << "  YES '?', The cgi path is needed\n" RES;
		path = getServerData().getRootDirectory() + "/cgi/" + path;		//   ***** a) // TODO 	SHOULD BE PRE PENDED WITH THE ROOT DIRECTORY OF THE LOCATION, NOT THE PATH FROM THE REQUEST
		getResponseData().setIsCgi(true);
	}
	if (path[0] == '/' && path == "/")
		path = getServerData().getRootDirectory();// TODO 	SHOULD BE PRE PENDED WITH THE ROOT DIRECTORY OF THE LOCATION, NOT THE PATH FROM THE REQUEST
	if (path[0] != '/')
	if (path == "./") {
		path = getServerData().getRootDirectory();// TODO 	SHOULD BE PRE PENDED WITH THE ROOT DIRECTORY OF THE LOCATION, NOT THE PATH FROM THE REQUEST
		std::cout << GRN << "Path is the root '/'    [" << path << "]\n" RES;
	}
	std::cout << GRN << "Path with pre-pended root folder [" << path << "]\n" RES;// TODO 	SHOULD BE PRE PENDED WITH THE ROOT DIRECTORY OF THE LOCATION, NOT THE PATH FROM THE REQUEST

	if (path.back() == '/'  && (path.find("?") == std::string::npos)) {
		std::cout << GRN << "The path has no GET-Form data. Last char is '/', it must be a folder.\n" << RES;
		storePath_and_FolderName(path);
	}

		// if the last char is not slash /   then look for question mark
		// else if ((ret = path.find("?")) == std::string::npos ) {
	else if ((ret = path.find("?")) == std::string::npos && _data.getRequestMethod() != "POST") {
		std::cout << YEL << "Simple GET (there is no FORM or POST method, also the '?' not found)\n" << RES;
		_data.setPath(path);
		int pos			= 0;
		pos				= path.find_last_of("/");
		_data.setPathFirstPart(path.substr(0, pos));
		_data.setPathLastWord(path.substr(pos, std::string::npos));
	}

	else if ((ret = path.find("?")) != std::string::npos) {			// Found '?' in the path, maybe also check != "POST"
		std::cout << GRN << "There is GET Form data, the '?' is found\n" << RES;
		storePathParts_and_FormData(path);
	}

	else if (_data.getRequestMethod() == "POST" || _data.getRequestMethod() == "DELETE") {
		std::cout << GRN << "There is POST Form data\n" << RES;
		storePath_and_FolderName(path);	// Not sur if this good here ???
		
		// path is not extracted correctly
		// _data.setQueryString(getRequestBody());
		//_data.setQueryString(_data.getBody());
	}

	ret = checkIfPathExists(_data.getPath(), event);
	if (ret != 0)	{ // What in case of root only "/"  ???
		std::cout << RED << "ret " << ret << ", file not found, should set error to 404)\n" << RES;
        setHttpStatus(NOT_FOUND);
		return (NOT_FOUND);
	}

	
	//std::cout << GRN "FD _kq: " << _data.getKqFd() << "\n" RES;

	// What in case of GET??
	checkTypeOfFile();
	_data.setRequestContentType(_data.getFileExtention());

	printPathParts(str, getRequestData());
	return (0);
}





