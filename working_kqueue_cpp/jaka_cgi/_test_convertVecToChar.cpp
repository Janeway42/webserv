#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <string.h>
#include <map>
#include <vector>
#include <unistd.h>


// #include <sys/types.h>
#include <sys/wait.h>	// for wait() on Linux

#include "../includes/Parser.hpp"






void runExecve(char *ENV[], char *args[], int fdClient) {
	std::cout << BLU "START runExeve\n" RES;
	
	//std::cout << "ENV[0]: " << ENV[0] << "\n";

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

		//(void)ENV;
		//(void)arr;
		// LS -LA WORKING
		// char *arr[3] = {(char*)"/bin/ls", (char*)"-la", NULL};
		// int ret = execve(arr[0], arr, NULL);
		// std::cout << RED "Error: Execve failed: " << ret << "\n" RES;


		
		// char * const arr2[2] = {(char*)"ABC", NULL};
		// setenv("ABC", "somePWD", 0);
		// setenv(ENV[0], "FIRST VARIABLE IN ENV", 0);
		// std::cout << "      value of ENV[0] [" << ENV[0] << "]\n";
		// std::cout << "      value of ABC [" << getenv("ABC") << "]\n";
		// arr2[0] = (char*)"somePWD";

		//setenv("COMSPEC", "somevalue", 0);
		//std::cout << "     getenv COMPSPEC" << getenv("COMSPEC") << "\n";

	setenv("SERVER_NAME", "MyServy", 0);
	setenv("ABC", "abc", 0);


		// MY COMMAND 
		//int ret = execve(args[0], args, NULL);
		int ret = execve(args[0], args, ENV);
		std::cout << RED "Error: Execve failed: " << ret << "\n" RES;
	}
	else {
		wait(NULL);
		//sleep(2);
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





void callCGI(char **env) {
	std::cout << RED "START CALL_CGI\n" RES;

		// Declare all necessary variables
		std::string comspec = "COMSPEC=";
		std::string request_method = "REQUEST_METHOD=";
		std::string query_string = "QUERY_STRING=";
		std::string server_name = "SERVER_NAME=";


		// Declare a vector and fill it with variables, with attached =values
		std::vector<std::string> allVariables;
		allVariables.push_back(comspec.append("someComspec"));
		allVariables.push_back(request_method.append("POST"));
		allVariables.push_back(query_string.append("SomeStringHTTPURL"));
		allVariables.push_back(query_string.append("MyServy"));

	std::cout << "Size of vector AllVariables: "<< allVariables.size() << "\n";

	// Make a char** array and copy all content of the aboce vector
	char **newAllVariables = new char*[allVariables.size()  + 1];

	size_t i = 0;
	for (i = 0; i < allVariables.size(); i++) {
	    newAllVariables[i] = new char[allVariables[i].length() + 1];
	    strcpy(newAllVariables[i], allVariables[i].c_str());
	}
	newAllVariables[i] = NULL;
	for (i = 0; newAllVariables[i]; i++) {
	    std::cout << newAllVariables[i] << std::endl;
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
	args[0] = (char *)"./cpp_cgi";   // Make sure the path is correct on Mac/Linux
	args[1] = NULL;


	// char *args[3];
	// args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	// args[1] = (char *)"./jaka_cgi/_somePhp.php"; // MUST BE WITH A DOT !!
	// args[2] = NULL;

	 //(void)ENV;
	 //(void)args;
	// (void)fdClient;
	runExecve(env, args, 33);

		// Cleanup
	for (size_t i = 0; i < allVariables.size(); i++) {
	    delete newAllVariables[i];
	}
	delete newAllVariables;



	//int ret = execve(args[0], args, vars);
	//printf("Execve failed: %d\n", ret);
}









int main (int ac, char**av, char**env)
{
	(void)ac;
	(void)av;
	callCGI(env);


	return 0;
}