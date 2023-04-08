#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

// #include <sys/types.h>
#include <sys/wait.h>	// for wait() on Linux

#include "Parser.hpp"
#include "RequestParser.hpp"

/*
	TODO What happens if you dont have a form on your page, but you directly write ?city=aaa in the URL?
	In this case, no action file is specified ???
*/

void Request::runExecve(char *ENV[], char *args[], struct kevent event) {
	//std::cout << BLU << "START runExeve\n" << RES;
	(void)event;

	// Create pipes
	Request *storage = (Request *)event.udata;

	_cgi.createPipes(storage->getKq(), event);
	// _cgi.createPipes(_data.getKqFd(), event); // moved to Request itself

	int ret = 0;
	pid_t		retFork;

	retFork = fork();

	if (retFork == 0) { // CHILD
		std::cout << YEL "Start CHILD execve()\n" << RES;
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

	//	std::cerr << RED "Before execve in child\n" << RES;
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
		// sleep(1);
	}
}



void Request::callCGI(struct kevent event) {
	std::cout << RED << "START CALL_CGI, cgi path: " << _data.getURLPath() << "\n" << RES;
	//(void)reqData;

	// Declare all necessary variables
	std::string request_method	= "REQUEST_METHOD=";
	std::string content_type	= "CONTENT_TYPE=";
	std::string content_length	= "CONTENT_LENGTH=";
	std::string query_string	= "QUERY_STRING=";
	std::string server_name		= "SERVER_NAME=";
	std::string comspec			= "COMSPEC=";

	// Convert length to string
	std::stringstream ssContLen;
	ssContLen << _data.getRequestContentLength();

	// Declare a vector and fill it with variables, with attached =values
	std::vector<std::string> temp;
	temp.push_back(request_method.append(_data.getRequestMethod()));
	temp.push_back(content_type.append(_data.getRequestContentType()));
	temp.push_back(content_length.append(ssContLen.str()));
	temp.push_back(query_string.append(_data.getQueryString()));
	temp.push_back(server_name.append("default"));
	temp.push_back(comspec.append("default"));

	// std::cout << "Size of vector temp: " << temp.size() << "\n";
	// std::cout << YEL << "POST BODY: " << temp[2] << "\n" << RES;
	// BY HERE, THE HUGE BODY IS STORED OK

	// Make a char** array and copy all content of the above vector
	//std::cout << GRN " ...... TEMP SIZE:  " << temp.size() << " \n" << RES "\n";

	char **env = new char*[temp.size() + 1];

	size_t i = 0;
	for (i = 0; i < temp.size(); i++) {
		env[i] = new char[temp[i].length() + 1];
		strcpy(env[i], temp[i].c_str());
	}
	env[i] = nullptr;
	//std::cout << YEL << "POST BODY ENV : " << env[2] << "\n" << RES;
	// BY HERE, THE HUGE BODY IS STORED OK

	// Just for printing
	std::cout << GRN "STORED ENV:\n" RES;
	for (i = 0; env[i]; i++) {
	  std::cout << "    " << i+1 << " " << env[i] << std::endl;
	}

	// char *args[3];
	// args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	// args[1] = (char *)"./jaka_cgi/_somePhp.php"; // MUST BE WITH A DOT !!
	// args[2] = NULL;

	char *args[3];
	args[0] = (char *)"/usr/bin/python";
	std::string tempPath = _data.getURLPath();
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

	_data.setURLPath(path.substr(0, temp));// TODO change to accept not full path??
    _data.setFullPath(path);// todo keep?
	//std::cout << CYN "StorePathParts() " << _data.getURLPath() << "\n" << RES;
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
	std::cout << YEL "Body:\n" RES;
	std::copy(_data.getBody().begin(), _data.getBody().end(), std::ostream_iterator<uint8_t>(std::cout));  // just to print
	//std::cout << "Stored GET _body [\n"        << BLU << _data.getBody() << RES << "]\n" << RES;

	storeFormData(queryString);	// maybe not needed (the whole vector and map)
	// if the cgi script can handle the whole queryString
}

// !!! Not storing correctly the path part and file name!
// If last '/' is found in newUrlPath, then this is a folder, not file
void Request::storePath_and_FolderName(std::string const & originalUrlPath, std::string const & newUrlPath) {
	std::cout << CYN "Start storePath_and_FolderName(). newUrlPath: [" << GRN_BG << newUrlPath << RES << "]\n" << RES;
	_data.setURLPath(originalUrlPath);
    _data.setFullPath(newUrlPath);// todo keep?

	// Check if there is query '?' and store path before it
	// Probably not needed searching for query here, because the method is POST, so query not possible
    size_t posQuestionMark = newUrlPath.find_first_of("?");
	if (posQuestionMark != std::string::npos) {
        _data.setURLPath(newUrlPath.substr(0, posQuestionMark));
        _data.setFullPath(newUrlPath.substr(0, posQuestionMark));// todo keep?
    }

// JOYCE I commented this part because I think the one below may work
//    size_t 	count = 0;
//    size_t pos1 = 0;
//    size_t pos2	= newUrlPath.find_first_of("/");
//	while (count < newUrlPath.length()) {
//        // count how many / is found on the path
//		if ((count = newUrlPath.find('/', count)) != std::string::npos) {
//			pos1 = pos2;
//			pos2 = count;
//		}
//		if (count == std::string::npos)
//			break ;
//		count++;
//	}
    size_t posLastSlash = newUrlPath.find_last_of("/");
	_data.setPathFirstPart(newUrlPath.substr(0, posLastSlash + 1));
	_data.setPathLastWord(newUrlPath.substr(posLastSlash + 1));// LastWord = File/Folder
}
// caso nao tenha last /
//            int pos = newUrlPath.find_last_of("/");
//            _data.setPathFirstPart(newUrlPath.substr(0, pos));
//            _data.setPathLastWord(newUrlPath.substr(pos));

/*
char* ENV[25] = {
	(char*)"COMSPEC=", (char*)"DOCUMENT_ROOT=", (char*)"GATEWAY_INTERFACE=", (char*)"HTTP_ACCEPT=", (char*)"HTTP_ACCEPT_ENCODING=",
	(char*)"HTTP_ACCEPT_LANGUAGE=", (char*)"HTTP_CONNECTION=", (char*)"HTTP_HOST=", (char*)"HTTP_USER_AGENT=", (char*)"PATH=",
	(char*)"QUERY_STRING=", (char*)"REMOTE_ADDR=", (char*)"REMOTE_PORT=", request_method2, (char*)"REQUEST_URI=", (char*)"SCRIPT_FILENAME=",
	(char*)"SCRIPT_NAME=", (char*)"SERVER_ADDR=", (char*)"SERVER_ADMIN=", (char*)"SERVER_NAME=",(char*)"SERVER_PORT=",(char*)"SERVER_PROTOCOL=",
	(char*)"SERVER_SIGNATURE=", (char*)"SERVER_SOFTWARE=", NULL
};
*/

//int checkTypeOfFile() {
//	std::cout << "Start checkTypeofFile(). Path: [" << GRN_BG << _data.getURLPath() << RES << "] " RES;
//
//	std::string path = _data.getURLPath();
//	std::string temp = _data.getURLPath();
//
//	// CHECK IF THE PATH IS A FILE OR FOLDER, REGARDLlES IF IT HAS ANY EXTENSION
//    // TODO: USE THE FUNCTION INSIDE PARSER.CPP
//	struct stat s;
//    if (stat(path.c_str(), &s) == 0) {
//        if (S_ISDIR(s.st_mode)) {
//            std::cout << CYN "is a directory\n";
//			_data.setIsFolder(true);
//			// return (0);
//        } else if (S_ISREG(s.st_mode)) {
//            std::cout << CYN "is a file\n" RES;
//        } else {
//            std::cout << CYN "is not a valid directory or file\n" RES;
//        }
//    } else {
//        std::cerr << RED << "Error getting file/directory info: " << strerror(errno) << "\n" RES;
//    }
//	return (0);
//}

// Some arguments not used
static void printPathParts(RequestData reqData) {
	std::cout << std::endl;
	std::cout << "URL Path:        [" << PUR << reqData.getURLPath() << RES << "]\n";
	std::cout << "Full URL Path:   [" << PUR << reqData.getFullPath() << RES << "]\n";
	std::cout << "Path first part: [" << PUR << reqData.getURLPathFirstPart() << RES << "]\n";
	std::cout << "File/Folder:     [" << PUR << reqData.getURLPathLastWord() << RES << "]\n";
	std::cout << "File extension:  [" << PUR << reqData.getFileExtention() << RES << "]\n";
    //std::cout << YEL "Body:\n" RES;
	//std::copy(reqData.getBody().begin(), reqData.getBody().end(), std::ostream_iterator<uint8_t>(std::cout));  // just to print
	//std::cout << "Body:            [" << PUR << reqData.getBody() << RES << "]\n";

	std::map<std::string, std::string> formData;
	formData = reqData.getFormData();
	if (not formData.empty()) {
		std::cout << std::endl << "STORED FORM DATA PAIRS:" << std::endl;
		std::map<std::string, std::string>::iterator it;
		for (it = formData.begin(); it != formData.end(); it++) {
            std::cout << PUR << "   " << it->first << RES << " ---> " << PUR << it->second << "\n" << RES;
        }
	} else {
        std::cout << "Form Data:    " << YEL << "(not present)\n" << RES;
    }
	std::cout << std::endl;
}

HttpStatus Request::checkIfPathExists(std::string const newUrlPath) {

    std::cout << "Start CheckIfFIleExists(), newUrlPath [" << newUrlPath << "] \n" << RES;

    std::ifstream file(newUrlPath.c_str());
    if (not file.is_open()) {		// ??? what is this syntax? -> joyce for cpp we can use the keyword "not" in the place of '!', for readability :)
        std::cout << RED << "Error: File " << newUrlPath << " not found\n" << RES;
        setHttpStatus(NOT_FOUND);
        return (NOT_FOUND);
    }
    std::cout << GRN << "File/folder " << RES << newUrlPath << GRN << " exists\n" << RES;
    return OK;
}

HttpStatus Request::parsePath(std::string originalUrlPath) {
//	std::string path = removeDuplicateSlash(originalUrlPath);// here error: read buffer overflow
	std::string newUrlPath;

	if (originalUrlPath.empty()) {
		return NO_CONTENT;// Todo maybe another status?
    }

    std::cout << "originalUrlPath:               [" << GRN_BG << originalUrlPath << RES << "]" << std::endl;
    std::cout << "server block root directory:   [" << GRN_BG << getServerData().getRootDirectory() << RES << "]" << std::endl;
    std::cout << std::endl << GRN << "Starting parsePath() and searching for the correct location block on the config file:" << RES << std::endl << std::endl;

    std::string serverBlockDir = getServerData().getRootDirectory();
    std::vector<ServerLocation>::const_iterator location = getServerData().getLocationBlocks().cbegin();
    for (; location != getServerData().getLocationBlocks().cend(); ++location) {
        std::string locationBlockUriName = location->getLocationUriName();
        std::string locationBlockRootDir = location->getRootDirectory();

        std::cout << "⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻ ⎻" << std::endl;
        std::cout << "locationBlockUriName:            [" << GRN_BG << locationBlockUriName << RES << "]" << std::endl;
        std::cout << "locationBlockRootDir:            [" << GRN_BG << locationBlockRootDir << RES << "]" << std::endl;

        // When a request comes in, nginx will first try to match the URI to a specific location block.
        if (originalUrlPath[0] != '/') {
            //Todo Ex.: ????
            //if (originalUrlPath == "./") {// TODO WHEN IT CAN BE LIKE THIS ./ ???????????????
            //    newUrlPath = serverBlockDir;//
            //    std::cout << "Path is the root '/'    [" << GRN_BG << newUrlPath << RES << "]\n";
            //    break;
            //}
            if (originalUrlPath == "./" && locationBlockUriName == "/") {
                std::cout << BLU << "location block [" << originalUrlPath << "] exists on config file" << RES << std::endl;
                std::cout << BLU << "Its root_directory [" << locationBlockRootDir << "] and configuration will be used" << RES << std::endl;
                _data.setIsFolder(true);
                newUrlPath = locationBlockRootDir;
                break;
            }
        }
        /* the URI is "/", nginx will look for a file named "index.html" in the root directory of the server block.
         * If it finds the file, it will be served to the client as the response to the request.
         * If it doesn't find the file, nginx will return a "404 Not Found" error to the client.
         * So, if you have a file named "index.html" in the root directory of your server block and there is no explicit
         * "location /" block defined in your nginx configuration file, then nginx will serve that file by default when
         * someone requests the root URL of your site. */
        else if (originalUrlPath[0] == '/') {
            // Ex.: localhost:8080
            if (originalUrlPath.size() == 1) {//originalUrlPath == "/"
                if (originalUrlPath == locationBlockUriName) {
                    std::cout << BLU << "location block [" << originalUrlPath << "] exists on config file" << RES << std::endl;
                    std::cout << BLU << "Its root_directory [" << locationBlockRootDir << "] and configuration will be used" << RES << std::endl;
                    _data.setIsFolder(true);
                    newUrlPath = locationBlockRootDir;
                    break;
                }
            }
            else if (originalUrlPath != "/") {
                // Ex.: localhost:8080/favicon.ico or localhost:8080/cgi/script.py
                if (pathType(locationBlockRootDir + originalUrlPath) == REG_FILE) {
                    std::string fileExtensionFromUrl;
                    std::string DirFromUrl;

                    // If it is a file, save the extension
                    std::size_t extension = originalUrlPath.find_last_of(".");
                    if (extension != std::string::npos) {
                        _data.setFileExtention(originalUrlPath.substr(extension));
                    } else {
                        std::cout << YEL << "There is no extension in the file" << RES << std::endl;
                    }

                    if (originalUrlPath.find('?') != std::string::npos || _data.getRequestMethod() == "POST") {
                        getCgiData().setIsCgi(true);
                        std::cout << BLU << "'?' was found, so cgi root_directory is needed" << RES << std::endl;
                        std::cout << "Path is a script file. ";

                        fileExtensionFromUrl = originalUrlPath.substr(0, originalUrlPath.find('.') + 1);

                        // The if block down below is just for logging
                        if (not fileExtensionFromUrl.empty()) {
                            std::cout << "Deleting file name from it so the extension can be matched against the location block uri extension name. Extension: ";
                            std::cout << GRN_BG << fileExtensionFromUrl << RES << std::endl;
                        }

                    } else if (originalUrlPath.find('?') == std::string::npos && _data.getRequestMethod() != "POST") {
                        std::cout << BLU << "No '?' found, so no cgi root_directory needed" << RES << std::endl;
                        std::cout << "Path is a file. ";

                        DirFromUrl = originalUrlPath.substr(0, originalUrlPath.find_last_of('/'));

                        // The if block down below is just for logging
                        if (not DirFromUrl.empty()) {
                            std::cout << "Deleting file from it so it can be matched against the location block uri name. Path: ";
                            std::cout << GRN_BG << DirFromUrl << RES << std::endl;
                        }
                    }

                    /* If the url is a file, the match will be done between the directory where the file is, against the location uri
                     * ex: url localhost/cgi/cgi_index.html -> the /cgi part will be checked against a location uri */
                    /* If the url is a script file, the match will be done between the extension of it, against the location uri
                     * ex: url localhost/cgi/script.py -> the .py part will be checked against a location uri */
                    if (DirFromUrl == locationBlockUriName || fileExtensionFromUrl == locationBlockUriName) {
                        std::cout << BLU << "location block for [" << originalUrlPath << "] exists on config file as [" << locationBlockUriName << "]" << RES << std::endl;
                        std::cout << BLU << "Its root_directory [" << locationBlockRootDir << "] and configuration will be used" << RES << std::endl;
                        newUrlPath = locationBlockRootDir + originalUrlPath;
                        break;
                    }
                }

                // If it is a directory and has a / at the end, delete it so it can be matched against the config file locations
                std::string newUrlPath_NoSlash = originalUrlPath;
                if (originalUrlPath.back() == '/') {
                    newUrlPath_NoSlash = originalUrlPath.substr(0, originalUrlPath.size() - 1);
                }

                std::cout << RED << newUrlPath_NoSlash << " | " << locationBlockUriName << RES << std::endl;

                // Ex.: localhost:8080/test/ or localhost:8080/test
                if (newUrlPath_NoSlash == locationBlockUriName) {
                    _data.setIsFolder(true);
                    if (originalUrlPath.find('?') == std::string::npos) {
                        //std::cout << GRN << "The newUrlPath has no GET-Form data. Last char is '/', it must be a folder.\n" << RES;
                        std::cout << BLU << "No '?' found and newUrlPath has no GET-Form data" << RES << std::endl;
                        std::cout << "Path is a directory." << std::endl << RES;

                        std::cout << GRN << "location block [" << originalUrlPath << "] exists on config file" << RES << std::endl;
                        std::cout << BLU << "Its root_directory [" << locationBlockRootDir << "] and configuration will be used" << RES << std::endl;
                        newUrlPath = locationBlockRootDir;
//                        storePath_and_FolderName(newUrlPath);
                        break;

                    }
                }
            }
        }
//        if (originalUrlPath.back() == '/'  && originalUrlPath.find('?') == std::string::npos) {
//            std::cout << GRN << "The originalUrlPath has no GET-Form data. Last char is '/', it must be a folder.\n" << RES;
//            newUrlPath =
//            storePath_and_FolderName(newUrlPath);
//        }
//        // if the last char is not slash / then look for question mark
//        else if (originalUrlPath.find('?') == std::string::npos && _data.getRequestMethod() != "POST") {
//            std::cout << YEL << "Simple GET (there is no FORM or POST method, also the '?' not found)\n" << RES;
//            newUrlPath =
//            _data.setURLPath(originalUrlPath);
//            _data.setFullPath(newUrlPath);
//            int pos = newUrlPath.find_last_of("/");
//            _data.setPathFirstPart(newUrlPath.substr(0, pos));
//            _data.setPathLastWord(newUrlPath.substr(pos));
//        }
//        // Found '?' in the originalUrlPath, maybe also check != "POST"
//        else if (originalUrlPath.find('?') != std::string::npos) {
//            std::cout << GRN << "There is GET Form data, the '?' is found\n" << RES;
//            newUrlPath =
//            storePathParts_and_FormData(newUrlPath);
//        } else if (_data.getRequestMethod() == "POST" || _data.getRequestMethod() == "DELETE") {
//            std::cout << GRN << "There is POST Form data\n" << RES;
//            newUrlPath =
//            storePath_and_FolderName(newUrlPath);	// Not sur if this good here ???
//
//            // originalUrlPath is not extracted correctly
//            // _data.setQueryString(getRequestBody());
//            //_data.setQueryString(_data.getBody());
//        }
        std::cout << YEL << "The url path [" << originalUrlPath << "] did not match the current location block [" << locationBlockUriName << "] from the config file. ";
        std::cout << "Checking the next locationBlockUriName" << RES << std::endl;
    }
    if (newUrlPath.empty()) {
        std::cout << std::endl << GRN << "As the UrlPath did not match any location block, ";
        std::cout << "the server block root_directory [" << serverBlockDir << "] and configuration will be used" << RES << std::endl;
        newUrlPath = serverBlockDir;
    }
    std::cout << GRN << "Final newUrlPath [" << GRN_BG << newUrlPath << RES << "]\n\n";
    storePath_and_FolderName(originalUrlPath, newUrlPath);
	_data.setResponseContentType(_data.getFileExtention());
	// What in case of GET??
//	checkTypeOfFile();

    printPathParts(_data);
    // What in case of root only "/"  ??? JOYCE -> If its / only, it will the checked against the root directory of the config file (which is already being checked if it exists or not)
    return checkIfPathExists(newUrlPath);//_data.getFullPath()// TODO only needed if its a file, move it up
}
