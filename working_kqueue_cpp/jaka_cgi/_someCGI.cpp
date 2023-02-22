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

/* 
	All the CGI Programs to be executed by the HTTP server are kept in a pre-configured directory. 
	This directory is called CGI directory and by convention it is named as 
		/var/www/cgi-bin. 
	By convention CGI files will have extension as .cgi, though they are C++ executable.
*/


/*	Another example of using CGI   (http://www.openroad.org/cgihelp/cgi.html)

	You can also have a link on a page run a CGI program. To do this, you specify the name of the CGI program as the link, for example(This example also shows how to pass a parameter to the CGI):
	<A HREF="../cgi-bin/CGIFavColor.cgi?FavColor=Black">Favorite Color is black</A>

	You can also run a CGI program when a page is loaded (often used for web counters), by specifying the name of the CGI as an element, like an image. For example:
	<IMG SRC="../cgi-bin/CGIFavColor.cgi">
*/

int main(int ac, char **av, char *env[]) {

	(void)ac;
	(void)av;
	//(void)env;


	//char *  vars[] = {"CITY=Amsterdam", "STREET=Singel"};	// werror
	//std::string vars[2] = {"CITY=Amsterdam", "STREET=Singel"};

	// const std::string ENV[ 24 ] = {
	// 	"COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE", "HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",             
	// 	"HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION", "HTTP_HOST", "HTTP_USER_AGENT", "PATH",            
	// 	"QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT", "REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
	// 	"SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN", "SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",     
	// 	"SERVER_SIGNATURE","SERVER_SOFTWARE"
	// };

	// char *args[3];
	// args[0] = (char *)"helloName.cgi";   // Make sure the path is correct on Mac/Linux
	// args[1] = NULL;
	char *vars[] = {(char*)"CITY=Amsterdam", (char*)"STREET=Singel"};

	char *args[3];
	args[0] = (char *)"/usr/bin/php";   // Make sure the path is correct on Mac/Linux
	args[1] = (char *)"_somePhp.php"; 
	args[2] = NULL;


	int ret = execve(args[0], args, vars);
	printf("Execve failed: %d\n", ret);
}
