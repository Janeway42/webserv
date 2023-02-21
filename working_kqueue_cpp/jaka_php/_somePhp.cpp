#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

// int main(int ac, char **av, char **env)
// {
// 	int cgi_pid;
// 	int tubes[2];

// 	char *arr[] = {"/bin/ls", "_somePhp", NULL};
// 	// char *arr[] = {"_somePhp.php", "", NULL};

// //    int fd = open("_somePhp", O_RDONLY);

// 	pipe(tubes);
// 	if ((cgi_pid = fork()) == 0) // child process
// 	{
// 		printf("child: start execve()\n");
// 		close(tubes[1]);  
// 		dup2(tubes[0], 0);   
// 		// execve(arr[0], arr, env);
// 		execve(arr[0], arr, env);
// 		//printf("end execve()\n");
// 	}

// //    close(fd);
// 	printf("end main\n");

// 	return (0);
// }



#include <unistd.h>



int main(int ac, char **av, char *env[]) {

	(void)ac;
	(void)av;
	//(void)env;

	std::cout << "\n";
	std::string retr = getenv("HOME");
	std::cout << "Returned: [" << retr << "]\n";
	// std::cout << "\n";

	setenv("CITY", "Tokio", 0);
	std::cout << "getenv(CITY): [" << getenv("CITY") << "]\n";


	char *args[3];
	args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	// args[1] = (char *)"_somePhp.php";
	args[1] = (char *)"php_info.php";
	args[2] = NULL;


	char *vars[] = {(char*)"CITY=Amsterdam", (char*)"STREET=Singel"};
	// char *  vars[] = {"CITY=Amsterdam", "STREET=Singel"};	// werror
	// std::string vars[2] = {"CITY=Amsterdam", "STREET=Singel"};


	//int ret = execve(args[0], args, env);
	int ret = execve(args[0], args, NULL);
	printf("Execve failed: %d\n", ret);
}
