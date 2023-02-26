#include <stdlib.h>




int main() {
  
  //list of possible environment variables
  const char* env_vars[3] = {"HOME", "PATH", "LOGNAME"};
  
  for (int i = 0; i < 3; i++)
  {
    //Get the current path form the env variable $PATH
    const char* content = getenv(env_vars[i]); 

    //If variable exists print to stdout
    if (content != NULL){ 
      std::cout << env_vars[i] << " = " << content << std::endl;
    }
    else{
      std::cout << env_vars[i] << " not found!" << std::endl;
    }
  }
  return 0;
}



// int main (int ac, char *av[], char **env) {
// 	(void)ac;
// 	(void)av;
// 	// std::cout << "Content-type:text/html\r\n\r\n"; // HEADER

// 	// std::cout << "<html>\n";                       // CONTENT
// 	// std::cout << "<head>\n";
// 	// std::cout << "<title>CGI Hello Name</title>\n";
// 	// std::cout << "</head>\n";
// 	std::cout << "<body>\n";


// 	std::cout << "  getenv() [" << env[0] << "]\n";
// 	std::cout << "  getenv() [" << getenv("HOME") << "]\n";
// 	std::cout << "  getenv() [" << getenv((const char*)env[0]) << "]\n";

// 	std::cout << "CPP_CGI\nList values of all variables\n";
// 	for (int i = 0; env[i]; i++) {
// 		std::cout << "  ... " << env[i] << "\n";
// 		std::cout << i + 1 << "  " << env[i] << "    [" << getenv(env[i]) << "]\n";
// 	}





// 	//std::cout << "  ... " << env[0] << "    [" << getenv(env[0]) << "]\n";
// 	// std::cout << "  ... " << env[1] << "    [" << getenv(env[1]) << "]\n";
// 	// std::cout << "  ... " << env[2] << "    [" << getenv(env[2]) << "]\n";



// 	//std::cout << "       This came from cpp_cgi, via **env: [" << env[i] << "]\n";
// 	//std::string str1 = getenv(env[0]);
// 	//std::cout << "       value of env[0], str1 [" << str1 << "]\n";
	

// 	// std::cout << "</body>\n";
// 	// std::cout << "</html>\n";
	
// 	return 0;
// }