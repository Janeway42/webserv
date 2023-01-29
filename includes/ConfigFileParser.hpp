#ifndef WEBSERV_CONFIGFILEPARSER_HPP
#define WEBSERV_CONFIGFILEPARSER_HPP

#include "Parser.hpp"
#include "ServerData.hpp"
#include "LocationData.hpp"

#include <iostream>

namespace data {
class ConfigFile : public Parser {
    private:
        Server _server_data;
        Location _location_data;

        /** Private Methods */
        //template<typename T>
        std::string keyParser(std::string & lineContent, const std::string& keyToFind);
        bool handleFile(std::string const & configFileName);
        void parseFileServerBlock(std::ifstream & configFile);
        void parseFileLocationBlock(std::ifstream & configFile);

    public:
        ConfigFile();// todo make it unacceptable to construct?
        explicit ConfigFile(std::string const & configFileName);// todo what is it: Clang-Tidy: Single-argument constructors must be marked explicit to avoid unintentional implicit conversions
        virtual ~ConfigFile();

        /** Getters */
        Server const & getServerData() const;
        Location const & getLocationData() const;

    };
} // data
#endif // WEBSERV_CONFIGFILEPARSER_HPP
