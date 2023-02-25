#include <iostream>
#include <stdlib.h>




int main (int ac, char *av[], char *env[]) {
	// std::cout << "Content-type:text/html\r\n\r\n"; // HEADER

	// std::cout << "<html>\n";                       // CONTENT
	// std::cout << "<head>\n";
	// std::cout << "<title>CGI Hello Name</title>\n";
	// std::cout << "</head>\n";
	// std::cout << "<body>\n";

	std::cout << "CPP_CGI\nList values of all variables\n";
	for (int i = 0; env[i]; i++) {
		std::cout << i + 1 << "  " << env[i] << "    [" << getenv(env[i]) << "]\n";
	}




	//std::cout << "       This came from cpp_cgi, via **env: [" << env[i] << "]\n";
	//std::string str1 = getenv(env[0]);
	//std::cout << "       value of env[0], str1 [" << str1 << "]\n";
	

	// std::cout << "</body>\n";
	// std::cout << "</html>\n";
	
	return 0;
}