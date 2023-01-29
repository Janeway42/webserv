#include "includes/ConfigFileParser.hpp"

int main(int ac, char **av, char **env) {
    (void)env;
    if (ac == 2) {
        std::cout << std::boolalpha;
        std::cout << "Configuration file name: " << av[1] << std::endl;
        std::cout << "-----------------------------------------------------------------------------------" << std::endl;
        data::ConfigFile configFile(av[1]);
        data::Server serverData = configFile.getServerData();
        std::cout << "-----------------------------------------------------------------------------------" << std::endl;

//        std::cout << CYN << "Has server block: [" << configFile.getServerData().hasServerBlock() << "]" << BACK << std::endl << std::endl;
        std::cout << std::endl << "Starting server block" << std::endl;

        std::string serverName = configFile.getServerData().getServerName();
        std::cout << GRE << "Value for \"server_name\" on main: [" << serverName << "]" << BACK << std::endl;

        unsigned int listensTo = configFile.getServerData().getListensTo();
        std::cout << GRE << "Value for \"listens_to\" on main: [" << listensTo << "]" << BACK << std::endl;

        std::string ipAddress = configFile.getServerData().getIpAddress();
        std::cout << GRE << "Value for \"ip_address\" on main: [" << ipAddress << "]" << BACK << std::endl;

        std::string rootDirectory = configFile.getServerData().getRootDirectory();
        std::cout << GRE << "Value for \"root_directory\" on main: [" << rootDirectory << "]" << BACK << std::endl;

        std::string indexFile = configFile.getServerData().getIndexFile();
        std::cout << GRE << "Value for \"index_file\" on main: [" << indexFile << "]" << BACK << std::endl;

        unsigned int clientMaxBodySize = configFile.getServerData().getClientMaxBodySize();
        std::cout << GRE << "Value for \"client_max_body_size\" on main: [" << clientMaxBodySize << "]" << BACK << std::endl;

        std::string errorPage = configFile.getServerData().getErrorPage();
        std::cout << GRE << "Value for \"error_page\" on main: [" << errorPage << "]" << BACK << std::endl;

        unsigned int portRedirection = configFile.getServerData().getPortRedirection();
        std::cout << GRE << "Value for \"port_redirection\" on main: [" << portRedirection << "]" << BACK << std::endl;

        //std::cout << std::endl << CYN << "Has location block: [" << configFile.getLocationData().hasLocationBlock() << "]" << BACK << std::endl << std::endl;
        std::cout << std::endl << "Starting location block" << std::endl;

        std::string locationRootDirectory = configFile.getLocationData().getRootDirectory();
        std::cout << BLU << "Value for \"root_directory\" on main: [" << locationRootDirectory << "]" << BACK << std::endl;

        std::vector<AllowMethods> locationAllowMethods = configFile.getLocationData().getAllowMethods();
        std::cout << BLU << "Value for \"allow_methods\" on main: [";
        for (int it = GET; it != NONE; it++) {
            switch (it) {
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
        std::cout << "]" << BACK << std::endl;

        std::string locationIndexFile = configFile.getLocationData().getIndexFile();
        std::cout << BLU << "Value for \"index_file\" on main: [" << locationIndexFile << "]" << BACK << std::endl;

        bool locationAutoIndex = configFile.getLocationData().getAutoIndex();
        std::cout << BLU << "Value for \"auto_index\" on main: [" << locationAutoIndex << "]" << BACK << std::endl;
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
