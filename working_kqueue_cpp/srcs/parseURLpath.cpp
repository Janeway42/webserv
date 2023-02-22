#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>


// #include "_colors.h"
#include "../includes/RequestParser.hpp"


namespace data {


void runExecve(char *ENV[], char *arr[]) {
	std::cout << BLU "START runExeve\n" RES;

	int    		fd[2];
	pid_t		retFork;
	std::string	incomingStr;

	if (pipe(fd) == -1)
		std::cout << "Error: Pipe failed\n";
	retFork = fork();

	if (retFork == 0) {
		std::cout << "    Start CHILD\n";
		if (retFork < 0)
			std::cout << "Error: Fork failed\n";
		dup2(fd[1], 1);
		close(fd[0]);

		//char *arr[4] = {(char*)"/bin/ls", (char*)"-la", path, NULL};
		int ret = execve(arr[0], arr, ENV);
		std::cout << RED "Error: Execve failed: " << ret << "\n" RES;
	}
	else {
		sleep(1);
		close(fd[1]);
		//char buff[100];
		dup2(fd[0], 0);

		// for (int ret = 1; ret != 0; ) {
		// 	memset(buff, '\0', 100);
		// 	ret = read(fd[0], buff, 99);
		// 	//std::cout << "Returned buffer, ret " << ret << "\n";
		// 	incomingStr.append(buff);
		// }
		//std::cout << "\nIncoming [" << incomingStr << "]\n";
	}    
}





void Request::callCGI(RequestData reqData) {
	std::cout << RED "START CALL_CGI\n" RES;

	//char *vars[] = {(char*)"CITY=Amsterdam", (char*)"STREET=Singel"};
	// const std::string ENV[ 24 ] = {
	char* ENV[24] = {
		(char*)"COMSPEC", (char*)"DOCUMENT_ROOT", (char*)"GATEWAY_INTERFACE", (char*)"HTTP_ACCEPT", (char*)"HTTP_ACCEPT_ENCODING",             
		(char*)"HTTP_ACCEPT_LANGUAGE", (char*)"HTTP_CONNECTION", (char*)"HTTP_HOST", (char*)"HTTP_USER_AGENT", (char*)"PATH",            
		(char*)"QUERY_STRING", (char*)"REMOTE_ADDR", (char*)"REMOTE_PORT", (char*)"REQUEST_METHOD", (char*)"REQUEST_URI", (char*)"SCRIPT_FILENAME",
		(char*)"SCRIPT_NAME", (char*)"SERVER_ADDR", (char*)"SERVER_ADMIN", (char*)"SERVER_NAME",(char*)"SERVER_PORT",(char*)"SERVER_PROTOCOL",     
		(char*)"SERVER_SIGNATURE", (char*)"SERVER_SOFTWARE"
	};


	// Get formData, count all keys, and them to **ENV array
	size_t nrKeys = 0;
	std::map<std::string, std::string> formData = reqData.getFormData();
	std::map<std::string, std::string>::iterator it;
	for (it = formData.begin(); it != formData.end(); it++, nrKeys++)
	//{ std::cout << "NrKeys " << nrKeys << "\n"; }


	// Make new 2D array with all envs AND query keys
	// size_t i = 0;
	// std::string arr[24 + nrKeys];
	// const char *arr[3 + nrKeys];
	// for (i = 0; i < 3; i++) {
	// 	arr[i] = ENV[i];
	// }


	// Store query keys to the ENV array, after the mandatory variables
	// Give each key the submitted value from the formData Map
	//		THIS SHOULD BE DONE INSIDE CGI SCRIPT - RECOGNIZING THE QUERY KEY PAIRS
	// for (it = formData.begin(); it != formData.end(); it++, i++) {
	// 	arr[i] = (it->first).c_str();
	// 	setenv((arr[i]).c_str(), (it->second).c_str(), 0);
	// }

	// Set the mandatory variables (not sure if necessary)
	setenv("SERVER_NAME", "Servy", 0);
	setenv("QUERY_STRING", reqData.getHttpPath().c_str(), 0); // IT MUST BE THE CORRECT QUERY STRING



	// Just print out the whole ENV array
	// for (i = 0; i < 24 + nrKeys; i++) {
	// 	std::cout << MAG << i << "  FormList [" << arr[i] << "]\n" RES;
	// }
	//std::cout << "getenv SERVERNAME: " << getenv("SERVER_NAME") << "\n";
	//std::cout << "getenv street: " << getenv("street") << "\n";


	// Prepare the array of the correct command/cgi file to be executed
	// The path of the executable must be according to the 'action file' from the URL
	// char *args[2];
	// args[0] = (char *)"../jaka_cgi/helloNamex.cgi";   // Make sure the path is correct on Mac/Linux
	// args[1] = NULL;


	char *args[3];
	args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	args[1] = (char *)"../jaka_cgi/_somePhp.php";
	args[2] = NULL;


	runExecve(ENV, args);

	//int ret = execve(args[0], args, vars);
	//printf("Execve failed: %d\n", ret);
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


int Request::parsePath(std::string str) {
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

	callCGI(getRequestData());



	//checkTypeOfFile(_data.getPathLastWord());
	//std::cout << RED "Last word " << _data.getPathLastWord() << RES "\n";
	return (0);
}
} // namespace data


/*
localhost:8080/folder//////folder/something.html?city=Tokio&street=Singel
*/



int mainXXX()
// int main()
{
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

	// parsePath("kostja.se////folder//folder/folder/folder///folder/folder//index.html?city=tokio&street=singel");
	
	

	std::cout << data::checkIfFileExists("test.html") << "\n";
	std::cout << data::checkIfFileExists("_testFolder") << "\n";
	
	
	return (0);
}



