#include <iostream>
#include <stdlib.h>








int main (int ac, char **av, char **env) {
	std::cout << "Content-type:text/html\r\n\r\n"; // HEADER

	std::cout << "<html>\n";                       // CONTENT
	std::cout << "<head>\n";
	std::cout << "<title>CGI Hello Name</title>\n";
	std::cout << "</head>\n";
	std::cout << "<body>\n";

	std::cout << env[0] << "\n";

	std::cout << "</body>\n";
	std::cout << "</html>\n";
	
	return 0;
}