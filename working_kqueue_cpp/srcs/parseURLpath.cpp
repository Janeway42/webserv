#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>

// #include <sys/types.h>
#include <sys/wait.h>	// for wait() on Linux

#include "../includes/RequestParser.hpp"


namespace data {

void runExecve(char *ENV[], char *args[], int fdClient) {
	std::cout << BLU "START runExeve\n" RES;
	//std::cout << "ENV: " << ENV[0] << "\n";

	int    		fd[2];
	pid_t		retFork;
	std::string	incomingStr;

	if (pipe(fd) == -1)
		std::cout << "Error: Pipe failed\n";
	
	std::cout << "pipe fd[0] " << fd[0] << ", pipe fd[1] " << fd[1] << " \n";
	//dup2(fdClient, fd[1]);
	
	retFork = fork();

	if (retFork == 0) {
		std::cout << "    Start CHILD\n";
		if (retFork < 0)
			std::cout << "Error: Fork failed\n";

		dup2(fd[1], 1);
		(void)fdClient;
		//dup2(fdClient, fd[1]);
		close(fd[0]);

		int ret = execve(args[0], args, ENV);
		std::cout << RED "Error: Execve failed: " << ret << "\n" RES;
	}
	else {
		wait(NULL);
		std::cout << "    Start Parent\n";
		close(fd[1]);
		char buff[100];

		dup2(fd[0], 0);
		//dup2(fdClient, fd[0]);

		std::cout << RED "        Start loop reading from child\n" RES;
		for (int ret = 1; ret != 0; ) {
			memset(buff, '\0', 100);
			ret = read(fd[0], buff, 99);
			incomingStr.append(buff);
		}
		std::cout << BLU "\n       All content read from CGI\n[" << incomingStr << "]\n" RES;
	}

}




void Request::callCGI(RequestData reqData, int fdClient) {
	std::cout << RED "START CALL_CGI\n" RES;

	// Declare all necessary variables
	std::string comspec			= "COMSPEC=";
	std::string request_method	= "REQUEST_METHOD=";
	std::string query_string	= "QUERY_STRING=";
	std::string server_name		= "SERVER_NAME=";


	// Declare a vector and fill it with variables, with attached =values
	std::vector<std::string> temp;
	temp.push_back(comspec.append("someComspec"));
	temp.push_back(request_method.append("POST"));
	temp.push_back(query_string.append("SomeStringHTTPURL"));
	temp.push_back(query_string.append("MyServy"));

	std::cout << "Size of vector temp: "<< temp.size() << "\n";

	// Make a char** array and copy all content of the aboce vector
	char **env = new char*[temp.size()  + 1];

	size_t i = 0;
	for (i = 0; i < temp.size(); i++) {
	    env[i] = new char[temp[i].length() + 1];
	    strcpy(env[i], temp[i].c_str());
	}
	env[i] = NULL;
	for (i = 0; env[i]; i++) {
	    std::cout << env[i] << std::endl;
	}


	// char *request_method2 = (char*)"REQUEST_METHOD=GET";
	// char* ENV[25] = {
	// 	(char*)"COMSPEC=", (char*)"DOCUMENT_ROOT=", (char*)"GATEWAY_INTERFACE=", (char*)"HTTP_ACCEPT=", (char*)"HTTP_ACCEPT_ENCODING=",             
	// 	(char*)"HTTP_ACCEPT_LANGUAGE=", (char*)"HTTP_CONNECTION=", (char*)"HTTP_HOST=", (char*)"HTTP_USER_AGENT=", (char*)"PATH=",            
	// 	(char*)"QUERY_STRING=", (char*)"REMOTE_ADDR=", (char*)"REMOTE_PORT=", request_method2, (char*)"REQUEST_URI=", (char*)"SCRIPT_FILENAME=",
	// 	(char*)"SCRIPT_NAME=", (char*)"SERVER_ADDR=", (char*)"SERVER_ADMIN=", (char*)"SERVER_NAME=",(char*)"SERVER_PORT=",(char*)"SERVER_PROTOCOL=",     
	// 	(char*)"SERVER_SIGNATURE=", (char*)"SERVER_SOFTWARE=", NULL
	// };


	// Prepare the array of the correct command/cgi file to be executed
	// The path of the executable must be according to the 'action file' from the URL
	char *args[2];
	args[0] = (char *)"./jaka_cgi/cpp_cgi";   // Make sure the path is correct on Mac/Linux
	args[1] = NULL;


	// char *args[3];
	// args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	// args[1] = (char *)"./jaka_cgi/_somePhp.php"; // MUST BE WITH A DOT !!
	// args[2] = NULL;

	// (void)ENV;
	// (void)fdClient;
	runExecve(env, args, fdClient);

	// Cleanup
	for (size_t i = 0; i < temp.size(); i++) {
	    delete env[i];
	}
	delete env;
}









// Some of arguments not used
void printPathParts(std::string str, std::string strTrim, std::string path,
					std::string fileName, RequestData reqData) {
	(void)path;
	(void)fileName;

	std::cout << "Found path:   [" << BLU << str << RES "]\n";
	std::cout << "Path trimmed: [" << BLU << strTrim << RES "]\n";
	std::cout << "Path part:    [" << MAG << reqData.getPathFirstPart() << RES "]\n";
	std::cout << "File/Folder:  [" << MAG << reqData.getPathLastWord() << RES "]\n";

	std::map<std::string, std::string> formData;
	formData = reqData.getFormData();

	if (! formData.empty()) {
		std::cout << "\nSTORED FORM DATA PAIRS:\n";// Print the map
		std::map<std::string, std::string>::iterator it;
		for (it = formData.begin(); it != formData.end(); it++)
			std::cout << MAG "   " << it->first << RES " ---> " MAG << it->second << "\n" RES;
	}
	else	
		std::cout << "Form Data:    " << GRE "(not present)\n" RES;
	std::cout << "\n";
}



int checkIfFileExists (const std::string& path) {
	std::ifstream file(path.c_str());

	if (!(file.is_open())) {
		std::cout << RED "File " << path << " not found\n" RES;
		return (-1);
	}
	std::cout << GRN "File " << path << " exists\n" RES;
	return 0;
}



int checkTypeOfFile(const std::string path) {
	
	std::string temp = path;
	if (path[0] == '.')
		temp = path.substr(1, std::string::npos);

	std::size_t found = temp.find_last_of(".");

	if (found != std::string::npos) {
		std::string extention = temp.substr(found, std::string::npos);
		std::cout << GRN "Found Extension: [" << extention << "]\n" RES;
	}
	else
		std::cout << GRN "There is no extention in the last name\n" RES;
	return (0);
}



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
std::map<std::string, std::string> Request::storeFormData(std::string &pathForm)
{
	//std::cout << GRN "Start store form data()\n" RES;
	std::cout << GRN "   BODY:        [" << _body << "]\n" RES;
	std::cout << GRN "   FORM PATH:   [" << pathForm << "]\n" RES;

	std::string					line;
	std::vector<std::string>	formList;

	std::stringstream iss(pathForm);
	while (std::getline(iss, line, '&'))
		formList.push_back(line);

	_data.setFormList(formList);

	// MAYBE THE FORM MAP WILL NOT BE NEEDED
	std::string							key, val;
	std::map<std::string, std::string>	formDataMap;
	std::vector<std::string>::iterator	it;

	for (it = formList.begin(); it != formList.end(); it++) {
		std::stringstream iss(*it);
	 	std::getline(iss, key, '=') >> val;
		formDataMap[key] = val;
		//std::cout << YEL "  ... vector [" << *it << "]\n" RES;
	}
	_data.setFormData(formDataMap);
	return (formDataMap);
}



// Last word in path must be a folder (last '/' found)
// The 2nd and 3rd args not needed anymore
// void	Request::storePath_and_FolderName(std::string path, std::string pathFirstPart, std::string pathLastWord, RequestData reqData) {
void	Request::storePath_and_FolderName(std::string path) {

		int 	pos1	= 0;
		int		pos2	= 0;
		size_t 	count	= 0;
		pos2 			= path.find_first_of("/");

		while (count < path.length()) {
			if ((count = path.find("/", count)) != std::string::npos) {
				pos1 = pos2;
				pos2 = count;
			}
			if ( count == std::string::npos )
				break ;
			count++;
		}
	//	pathFirstPart	= path.substr(0, pos1 + 1);
	//	pathLastWord	= path.substr(pos1 + 1, pos2);

		_data.setPathFirstPart(path.substr(0, pos1 + 1));
		_data.setPathLastWord(path.substr(pos1 + 1, pos2));
	//	reqData.setPathLastWord(path.substr(pos1 + 1, pos2));
}


// Found GET Method with '?' Form Data
void	Request::storePathParts_and_FormData(std::string path) {

	int temp				= path.find_first_of("?");
	std::string tempStr		= path.substr(0, temp);
	int posLastSlash 		= tempStr.find_last_of("/");
	int	posFirstQuestMark	= path.find_first_of("?");
	std::string	pathForm	= path.substr(temp, std::string::npos);

	_data.setPathFirstPart(tempStr.substr(0, posLastSlash));
	_data.setPathLastWord(path.substr(posLastSlash, posFirstQuestMark - posLastSlash));

	if (pathForm[0] == '?') 	// Skip the '?' in the path
		pathForm = &pathForm[1];
	storeFormData(pathForm);
}



int Request::parsePath(std::string str, int fdClient) {
	// maybe also trim white spaces front and back
//	Request		req;
	std::string path			= removeDuplicateSlash(str);
	size_t		ret				= 0;
	std::string pathLastWord	= "";
	

	if (path == "")
		return (-1);
	else if (path == "/") {
		std::cout << GRN "The path has no GET-Form data. Path is the root '/'\n" RES;
	}
	else if (path.back() == '/'  && (path.find("?") == std::string::npos)) {
		std::cout << GRN "The path has no GET-Form data. Last char is '/', it must be a folder.\n" RES;
		storePath_and_FolderName(path);
		printPathParts(str, path, "", "", getRequestData());
	}

	// if the last char is not slash /   then look for question mark 
	else if ((ret = path.find("?")) == std::string::npos ) {
		std::cout << GRN "There is no Form data, the '?' not found\n" RES;
		int pos			= 0;
		pos				= path.find_last_of("/");	

		_data.setPathFirstPart(path.substr(0, pos));
		_data.setPathLastWord(path.substr(pos, std::string::npos));
		printPathParts(str, path, "", "", getRequestData());
	}
	
	else if ((ret = path.find("?")) != std::string::npos) {			// Found '?' in the path
		std::cout << GRN "There is GET Form data, the '?' is found\n" RES;
		storePathParts_and_FormData(path);
		printPathParts(str, path, "", "", getRequestData());
	}

	checkIfFileExists(path);	// What in case of root only "/"  ???
	checkTypeOfFile(path);


	//(void)fdClient;
	callCGI(getRequestData(), fdClient);



	//checkTypeOfFile(_data.getPathLastWord());
	//std::cout << RED "Last word " << _data.getPathLastWord() << RES "\n";
	return (0);
}


/*
localhost:8080/folder//////folder/something.html?city=Tokio&street=Singel
*/

} // namespace data



#include <iostream>
#include <string>
int mainXXXXX()
{




	//Request request;
	// parsePath("/");
	// parsePath("/home/");						// must be folder
	// parsePath("/home");							// check if folder or file
	
	// parsePath("//////home/////folderA/");                	// must be folder
	// parsePath("/home////folderB/");                	// must be folder
	// parsePath("/home/folderC/////");                 	// check if folder or file
	// parsePath("/home/folderD");                 	// check if folder or file
	
	
	// parsePath("/home/index.html");                 	// check if folder or file
	// parsePath("/home/folderD/index.html?street=///singel///");                 	// check if folder or file

	// parsePath("/home/index.html/");             	// check if folder or file
	// parsePath("/home/folder/index.html");		// check if folder or file
	// parsePath("/home/folder/response.php");		// check if folder or file
	// parsePath("/home/folder/response.php?street=Singel&city=London");

	//request.parsePath("kostja.se////folder//folder/folder/folder///folder/folder//index.html?city=tokio&street=singel", 33);
	
	

	//std::cout << data::checkIfFileExists("test.html") << "\n";
	//std::cout << data::checkIfFileExists("_testFolder") << "\n";
	
	
	return (0);
}




/*	EXAMPLE SENDING A FILE
void	send(int port, std::string filename)
{
	int					sock;
	struct sockaddr_in	serv_addr;
	char				buffer[4096] = {0};
	std::fstream		file;
	std::string			content;

	file.open(filename);
	content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cout << std::endl << RED << "< Socket creation error >" << RESET << std::endl << std::endl;
		return ;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);


	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		std::cout << std::endl << RED << "< Connection failed >" << RESET << std::endl << std::endl;
		return ;
	}

	content += "\r\n";

	std::cout << std::endl << "Sending :" << std::endl;
	std::cout << "[" << RED << content << RESET << "]" << std::endl << std::endl;

	send(sock, content.c_str(), content.size(), 0);
	read(sock, buffer, 4095);

	std::cout << std::endl << "Response :" << std::endl;
	std::cout << "[" << GREEN << std::string(buffer) << RESET << "]" << std::endl << std::endl;

	close(sock);

	return ;
}
*/



