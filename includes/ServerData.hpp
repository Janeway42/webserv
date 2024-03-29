#ifndef SERVERDATA_HPP
#define SERVERDATA_HPP

#include "Parser.hpp"
#include "ServerLocation.hpp"

/**
 * An IP address identifies a machine in an IP network and is used to determine the destination of a data packet.
 * Port numbers identify a particular application or service on a system.
 *
 * Directory and file (full/relative) path:
 *      ./ means $path_to_web_server_executable/
 *      a directory or file with no path (i.e.: test_index.html) will be searched on ./
 */

class ServerData : public Parser {
    private:
        std::string _server_name;
		std::string _host;
        std::string _listens_to;
        std::string _root_directory;
        std::string _index_file;
        unsigned int _client_max_body_size;
        std::vector<std::string> _error_page;
        std::string _upload_directory;
        std::string _upload_directoryName;

        /* As more than 1 location block can be added to a server block */
        std::vector<ServerLocation> _location_data_vector;

        bool _server_name_is_ip;
        int _listening_socket;

    public:
        ServerData();
        virtual ~ServerData();
        ServerData(ServerData const & rhs);

        /** Getters */
        std::string getServerName() const;
		std::string getHost() const;
        std::string getListensTo() const;
        std::string getRootDirectory() const;
        std::string getIndexFile() const;
        unsigned int getClientMaxBodySize() const;
        std::vector<std::string> getErrorPages() const;
        std::string getUploadDirectory() const;
        std::string getUploadDirectoryName() const;
        std::vector<ServerLocation> & getLocationBlocks();
        int getListeningSocket() const;

        /** Setters */
        void	setServerName(std::string const & serverName);
		void	setHost(std::string const & host);
        void	setListensTo(std::string const & port);
        void	setRootDirectory(std::string const & rootDirectory);
        void	setIndexFile(std::string const & indexFile);
        void	setClientMaxBodySize(std::string const & bodySize);
        void	setErrorPages(std::string const & errorPage);
        void	setUploadDirectory(std::string const & uploadDirectory);
        void	setListeningSocket();
		void	setExistingListeningSocket(int fd);
};
#endif //SERVERDATA_HPP
