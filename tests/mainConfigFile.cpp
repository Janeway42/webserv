#include "ConfigFileParser.hpp"

void func() {
    system("leaks a.out");
}
// c++ -Iincludes -Wall -Werror -Wextra -pedantic -std=c++98 -Wshadow -fsanitize=address -g3 srcs/ConfigFileParser.cpp srcs/ServerData.cpp srcs/ServerLocation.cpp srcs/Parser.cpp tests/mainConfigFile.cpp

int main(int ac, char **av) {
    //atexit(func);
    if (ac == 2) {
        std::cout << std::boolalpha;
        std::cout << "Configuration file name: " << av[1] << std::endl;
        std::cout << "-----------------------------------------------------------------------------------" << std::endl;
        try {
            ConfigFileParser configFileData(av[1]);
            std::cout << "-----------------------------------------------------------------------------------" << std::endl;
            std::cout << RED << "Number of server block(s): " << configFileData.numberOfServerBlocks() << RES << std::endl;
            std::cout << RED << "Number of location + cgi block(s): " << configFileData.numberOfLocationBlocks() << RES << std::endl;

            /* begin() returns an iterator to beginning while cbegin() returns a const_iterator to beginning. */
            std::vector<ServerData>::iterator it_server;
            for (it_server = configFileData.servers.begin(); it_server != configFileData.servers.cend(); ++it_server) {

                /****************************************** server block data *****************************************/
                std::cout << std::endl << "Starting server block " << std::endl;

                std::string serverName = it_server->getServerName();
                std::cout << GRE << std::left << std::setw(30) << "\"server_name\": " << serverName << RES << std::endl;

                std::string listensTo = it_server->getListensTo();
                std::cout << GRE << std::left << std::setw(30) << "\"listens_to\": " << listensTo << RES << std::endl;

                std::string ipAddress = it_server->getIpAddress();
                std::cout << GRE << std::left << std::setw(30) << "\"ip_address\": " << ipAddress << RES << std::endl;

                std::string rootDirectory = it_server->getRootDirectory();
                std::cout << GRE << std::left << std::setw(30) << "\"root_directory\": " << rootDirectory << RES << std::endl;

                std::string indexFile = it_server->getIndexFile();
                std::cout << GRE << std::left << std::setw(30) << "\"index_file\": " << indexFile << RES << std::endl;

                unsigned int clientMaxBodySize = it_server->getClientMaxBodySize();
                std::cout << GRE << std::left << std::setw(30) << "\"client_max_body_size\": " << clientMaxBodySize << RES << std::endl;

                std::string errorPage = it_server->getErrorPage();
                std::cout << GRE << std::left << std::setw(30) << "\"error_page\": " << errorPage << RES << std::endl;

                unsigned int portRedirection = it_server->getPortRedirection();
                std::cout << GRE << std::left << std::setw(30) << "\"port_redirection\": " << portRedirection << RES << std::endl;


                std::vector<ServerLocation>::const_iterator it_location;
                for (it_location = it_server->getLocationBlocks().cbegin(); it_location != it_server->getLocationBlocks().cend(); ++it_location) {

                    if (it_location->isLocationCgi()) {
                        /************************************* cgi location block data ************************************/
                        std::cout << std::endl << "Starting location block" << std::endl;
                        std::cout << "Is this location a CGI location: " << it_location->isLocationCgi() << std::endl;

                        std::string cgiLocationRootDirectory = it_location->getRootDirectory();
                        std::cout << GRE << std::left << std::setw(30) << "\"root_directory\": " << cgiLocationRootDirectory << RES << std::endl;

                        std::string interpreterPath = it_location->getInterpreterPath();
                        std::cout << GRE << std::left << std::setw(30) << "\"interpreter_path\": " << interpreterPath << RES << std::endl;

                        std::string scriptExtension = it_location->getScriptExtension();
                        std::cout << GRE << std::left << std::setw(30) << "\"script_extension\": " << scriptExtension << RES << std::endl;

                        std::string cgiLocationIndexFile = it_location->getIndexFile();
                        std::cout << GRE << std::left << std::setw(30) << "\"index_file\": " << cgiLocationIndexFile << RES << std::endl;
                    } else {
                        /*************************************** location block data **************************************/
                        std::cout << std::endl << "Starting location block " << std::endl;
                        std::cout << "Is this location a CGI location: " << it_location->isLocationCgi() << std::endl;

                        std::string locationRootDirectory = it_location->getRootDirectory();
                        std::cout << GRE << std::left << std::setw(30) << "\"root_directory\": " << locationRootDirectory << RES << std::endl;

                        std::vector<AllowMethods> locationAllowMethods = it_location->getAllowMethods();
                        std::cout << GRE << std::left << std::setw(30) << "\"allow_methods\": ";
                        std::vector<AllowMethods>::const_iterator i;
                        for (i = locationAllowMethods.cbegin(); i != locationAllowMethods.cend(); i++) {
                            switch (i.operator*()) {
                                case GET:
                                    std::cout << "GET";
                                    break;
                                case POST:
                                    std::cout << "POST";
                                    break;
                                case DELETE:
                                    std::cout << "DELETE";
                                    break;
                                default:
                                    break;
                            }
                            std::cout << " ";
                        }
                        std::cout << RES << std::endl;

                        std::string locationIndexFile = it_location->getIndexFile();
                        std::cout << GRE << std::left << std::setw(30) << "\"index_file\": " << locationIndexFile << RES << std::endl;

                        bool locationAutoIndex = it_location->getAutoIndex();
                        std::cout << GRE << std::left << std::setw(30) << "\"auto_index\": " << locationAutoIndex << RES << std::endl;
                    }
                }
            }
            std::cout << std::endl;
        } catch(std::exception const & ex) {
            std::cout << RED << ex.what() << RES << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
/* TESTS
#include <iostream>
class Test {
public:
    struct {
        int test_var;
    } test_struct_untagged; // not a type, so it can't be used to make a copy of this struct
    struct name {
        int test_var;
    } test_struct_tagged;
    struct name test_struct_tagged_second_declaration;
    struct same_name {
        int test_var;
    } same_name;
    // initialized "manually"
    struct test_struct_tagged_not_declared_2 {
        int test_var;
    };
    test_struct_tagged_not_declared_2 test_struct_tagged_declared_2;
};
struct test_struct_tagged_not_declared {
    int test_var;
};// not declaring
// also not declaring "manually
int main() {
    Test test;
    test.test_struct_untagged.test_var = 1;
    std::cout << test.test_struct_untagged.test_var << std::endl;
    test.test_struct_tagged.test_var = 2;
    std::cout << test.test_struct_tagged.test_var << std::endl;
    test.same_name.test_var = 3;
    std::cout << test.same_name.test_var << std::endl;
    test.test_struct_tagged_declared_2.test_var = 4;
    std::cout << test.test_struct_tagged_declared_2.test_var << std::endl;
    struct test_struct_tagged_not_declared joyce = {};
    joyce.test_var = 5;
    std::cout << joyce.test_var << std::endl;
    return 0;
}
*/
