#include "ServerData.hpp"
#include "WebServer.hpp"

// ---- kqueue ----
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/event.h>

#include <arpa/inet.h>
#include <sstream>

#include "ServerData.hpp"

/** Default constructor */
ServerData::ServerData()
	/** Initializing default values for the server block */
	: _server_name("localhost"),
	_host("0.0.0.0"),
	_listens_to("8080"),
	_root_directory("./standard_server"),
	_index_file("index.html"),
	_client_max_body_size(1024),
	_error_page(std::vector<std::string>()),
	_upload_directory("uploads"),
    _upload_directoryName("uploads"),
    _server_name_is_ip(false),
	_listening_socket(-1) {
//	std::cout << PUR << "ServerData Default constructor" << RES << std::endl;
}

/** Copy constructor */
ServerData::ServerData(ServerData const & rhs)
	: _server_name(rhs._server_name),
	_host(rhs._host),
	_listens_to(rhs._listens_to),
	_root_directory(rhs._root_directory),
	_index_file(rhs._index_file),
	_client_max_body_size(rhs._client_max_body_size),
	_error_page(rhs._error_page),
	_upload_directory(rhs._upload_directory),
    _upload_directoryName(rhs._upload_directoryName),
	_location_data_vector(rhs._location_data_vector),
    _server_name_is_ip(rhs._server_name_is_ip),
	_listening_socket(rhs._listening_socket) {
//	std::cout << PUR << "ServerData Copy constructor" << RES << std::endl;
}

/** Destructor */
ServerData::~ServerData() {
	/** Cleaning default values for the server block */
	_server_name.clear();
	_host.clear();
	_listens_to.clear();
	_root_directory.clear();
	_index_file.clear();
	_client_max_body_size = 0;
	_error_page.clear();
	_upload_directory.clear();
    _upload_directoryName.clear();
    _server_name_is_ip = false;
	if (signalCall == true && fcntl(_listening_socket, F_GETFD) != -1)
		close(_listening_socket);
//	std::cout << PUR << "ServerData Destructor" << RES << std::endl;
}

/** #################################### Getters #################################### */

std::string ServerData::getServerName() const {
	return _server_name;
}

std::string ServerData::getHost() const{
	return _host;
}

std::string ServerData::getListensTo() const {
	return _listens_to;
}

std::string ServerData::getRootDirectory() const {
	return _root_directory;
}

std::string ServerData::getIndexFile() const {
	return _index_file;
}

unsigned int ServerData::getClientMaxBodySize() const {
	return _client_max_body_size;
}

std::vector<std::string> ServerData::getErrorPages() const {
	return _error_page;
}

std::string ServerData::getUploadDirectory() const {
    return _upload_directory;
}

std::string ServerData::getUploadDirectoryName() const {
    return _upload_directoryName;
}

std::vector<ServerLocation> & ServerData::getLocationBlocks() {
	return _location_data_vector;
}

int ServerData::getListeningSocket() const {
	return _listening_socket;
}

/** #################################### Setters #################################### */

static bool isIp(int ch) {
    if ((ch >= '0' && ch <= '9') || ch == '-' || ch == '.') {
        return true;
    }
    return false;
}

/*
 * To add a hostname or IP address to your macOS machine, you can modify the "hosts" file located at /etc/hosts.
 * This file maps hostnames to IP addresses and is used by the operating system to resolve domain names to IP addresses locally, without querying a DNS server.
 * To edit the hosts file, you can follow these steps:
 * 1. Open /etc/hosts (it requires sudo rights to modify system files)
 * 2. Add a new line for the hostname or IP address you want to add. The format of the line should be: <IP address> <hostname>
 *    ex: 192.168.0.1 example.com
 *    2.1. Or add a new hostname to an existing IP address
 *         ex:127.0.0.1 localhost example.com.br
 * 3. Finally, flush the DNS cache by typing the following command in the Terminal: sudo dscacheutil -flushcache
 *    This will ensure that the updated hosts file is used by the operating system.
 * After making these changes, you should be able to use the hostname or IP address you added to access the associated network resource on your Mac.
 * Obs.: We don't have right's permission to /etc :/
 */
void ServerData::setServerName(std::string const & serverName) {
    /* not mandatory | default: localhost (127.0.0.1) */
	if (not serverName.empty()) {
        struct sockaddr_in sockAddr = {};
        /* The inet_pton() converts a format address (char*) to network format in network byte order (sin_addr)
         * It returns:
         *      1 if the address was valid for the specified address family (AF_INET here)
         *      0 if the address was not parseable in the specified address family
         *      -1 if some system error occurred (in which case errno will have been set).
         * AF_INET (int) = for TCP address family
         * serverName (char*) = a presentation format address
         * sin_addr (void*) = base type for internet address (in_addr -> in_addr_t)
         * Another option:
         * in_addr_t if ((ret = inet_addr(serverName.c_str()) != INADDR_NONE) */
        if (std::all_of(serverName.begin(), serverName.end(), isIp)) {
            int ret = inet_pton(AF_INET, serverName.c_str(), &(sockAddr.sin_addr));
            if (ret == 1) {
                _server_name = serverName;
                _server_name_is_ip = true;
            } else {
                if (ret == -1) {
                    std::string err = "(errno: " + std::to_string(errno) + ") ";
                    throw ParserException(err + CONFIG_FILE_ERROR( "server_name", NOT_SUPPORTED));
                }
                throw ParserException(CONFIG_FILE_ERROR("server_name", NOT_SUPPORTED));
            }
        }
        // If serverName is a string (as localhost or www.test for example)
        else {
            _server_name = serverName;
        }
	}
}

void ServerData::setHost(std::string const & host) {
    /* not mandatory | default 0.0.0.0 */
    if (not host.empty()) {
        std::string temp = host;
        int blocks = 0;
        int dots = 0;
        while (temp != "")
        {
            size_t out = temp.find(".");
            if (out != std::string::npos)
                dots++;
            std::string tempBlock = temp.substr(0, out);
            try{
                int blockNr = stoi(tempBlock);
                if (blockNr < 0 || blockNr > 255)
                    throw ParserException(CONFIG_FILE_ERROR("host", NOT_SUPPORTED));
            }
            catch (...)	{
                throw ParserException(CONFIG_FILE_ERROR("host", NOT_SUPPORTED));
            }
            blocks++;
            if (out != std::string::npos)
                temp = temp.substr(out + 1);
            else
                temp = "";
        }
        if (dots != 3 || blocks != 4)
            throw ParserException(CONFIG_FILE_ERROR("host", NOT_SUPPORTED));
        _host = host;
    }
}

/* Available ports:
 * - Port 80 (standard): a well-known system ports (they are assigned and controlled by IANA).
 *   Obs.: Ports under 1024 need to be run as root (so we can't have access to it).
 * - Port 8080 (second most used): a user or registered port (they are not assigned and controlled but registered by IANA only).
 * - Ports ranging from 49152 to 65536: are available for anyone to use IF you register it.
 *
 * E.g.: If a web server is already running on the default port (80) and another web server needs to be hosted on
 * the HTTP service, it's best practice to host it on port 8080 (but not mandatory, any other alternative or custom
 * port can be used instead).
 * https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml?search=http
 */
void ServerData::setListensTo(std::string const & port) {
	/* not mandatory | default 8080 */
	if (not port.empty()) {
        for (std::string::const_iterator it = port.begin(); it != port.end(); it++) {
            if (*it < '0' || *it > '9') {
                throw ParserException(CONFIG_FILE_ERROR("Too many ports in one server block. listens_to", NOT_SUPPORTED));
            }
        }
        const int & portInt = std::stoi(port, nullptr, 10);
        if (portInt < 0 || portInt >= USHRT_MAX) {
            throw ParserException(CONFIG_FILE_ERROR("Port is out of range. listens_to", NOT_SUPPORTED));
        }
        /* No need to check port => 65536 since port is an unsigned short already */
        _listens_to = port;
	}
}

void ServerData::setRootDirectory(std::string const & rootDirectory) {
	/* not mandatory | default: ./$server_name */
    if (not rootDirectory.empty() && rootDirectory != "/" && rootDirectory != "./") {
		PathType type = pathType(rootDirectory);
		if (type == DIRECTORY) {
			_root_directory = addCurrentDirPath(rootDirectory) + rootDirectory;
		} else if (type == PATH_TYPE_ERROR) {
			throw ParserException(CONFIG_FILE_ERROR("root_directory", MISSING));
		} else {
			throw ParserException(CONFIG_FILE_ERROR("root_directory", NOT_SUPPORTED));
		}
	}
}

void ServerData::setIndexFile(std::string const & indexFile) {
	/* not mandatory | default: index.html */
	if (indexFile.find('/') != std::string::npos) {
		throw ParserException(CONFIG_FILE_ERROR("index_file", NOT_SUPPORTED));
	}

	std::string index_file_copy = indexFile;
	if (indexFile.empty()) {
		index_file_copy = _index_file;
	}
	std::string index_file = addRootDirectoryPath(_root_directory, index_file_copy);
	// doesn't contain regexp (regular expressions), wildcards or full/relative path
	if (pathType(index_file) == REG_FILE) {
		_index_file = index_file_copy;
	}
}

void ServerData::setClientMaxBodySize(std::string const & bodySize) {
	/* not mandatory | default: 1024 (1KB) -> Max: INT_MAX (2GB) */
	if (not bodySize.empty()) {
		try {
			unsigned int const & body_size = std::strtol(bodySize.c_str(), nullptr, 10);
			if (body_size <= INT32_MAX) {
				_client_max_body_size = body_size;
			} else {
				throw ParserException(CONFIG_FILE_ERROR("client_max_body_size", NOT_SUPPORTED));
			}
		} catch (...) {
			throw ParserException(CONFIG_FILE_ERROR("client_max_body_size", NOT_SUPPORTED));
		}
	}
}

void ServerData::setErrorPages(std::string const & errorPage) {
    /* not mandatory | default: empty, no set error page, webserver will decide */
    if (not errorPage.empty()) {
        std::vector<std::string> error_page_vector;
        std::string error_page;
        std::stringstream ss(errorPage);
        while(getline(ss, error_page, ' ')){
            if (error_page.find(".html") != std::string::npos) {
                std::string path_error_page = addRootDirectoryPath(_root_directory, "error_pages/" + error_page);
                if (pathType(path_error_page) == REG_FILE) {
                    error_page_vector.push_back(path_error_page);
                } else {
                    throw ParserException(CONFIG_FILE_ERROR("error_page", MISSING));
                }
            } else {
                throw ParserException(CONFIG_FILE_ERROR("error_page", NOT_SUPPORTED));
            }
        }
        _error_page = error_page_vector;
    }
}

void ServerData::setUploadDirectory(const std::string &uploadDirectory) {
    /* not mandatory | default: ./uploads */
    if (not uploadDirectory.empty() && uploadDirectory != "/" && uploadDirectory != "./") {
        std::string upload_dir = addRootDirectoryPath(_root_directory, uploadDirectory);
        PathType type = pathType(upload_dir);
        // doesn't contain regexp (regular expressions), wildcards or full/relative path
        if (type == DIRECTORY) {
            _upload_directory = upload_dir;
			_upload_directoryName = uploadDirectory;
        } else if (type == PATH_TYPE_ERROR) {
            throw ParserException(CONFIG_FILE_ERROR("upload_directory", MISSING));
        } else {
            throw ParserException(CONFIG_FILE_ERROR("upload_directory", NOT_SUPPORTED));
        }
    }
}

void ServerData::setExistingListeningSocket(int fd) {
	_listening_socket = fd;
}

void ServerData::setListeningSocket() {
	struct addrinfo *addr = NULL;
	struct addrinfo hints = addrinfo();

	hints.ai_family = PF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

    // Apparently Siege wants the IP address, not ServerName 'localhost'.

	// hostname: is either a valid host name or a numeric host address string consisting of a dotted decimal IPv4 address or an IPv6 address.
	// servname: is either a decimal port number or a service name listed in services(5).
	std::cout << "====================\n" << std::endl;

	if (getaddrinfo(_host.c_str(), _listens_to.c_str(), &hints, &addr) != 0)
	{
		freeaddrinfo(addr);
		throw std::runtime_error("Failed getaddrinfo");
	}
	std::cout << "_addr in set listening socket: " << &addr << " / addr->protocol: " << addr->ai_protocol << std::endl;
	std::cout << "hostname used on getaddrinfo: " << _host << std::endl;

	_listening_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (_listening_socket < 0)
		throw std::runtime_error("Failed socket");
	fcntl(_listening_socket, F_SETFL, O_NONBLOCK);

	int socket_on = 1;
	setsockopt(_listening_socket, SOL_SOCKET, SO_REUSEADDR, &socket_on, sizeof(socket_on));
	if (bind(_listening_socket, addr->ai_addr, addr->ai_addrlen) == -1)
	{
		freeaddrinfo(addr);
		throw std::runtime_error("Failed bind");
		// throw std::runtime_error("Failed bind (errno: " + std::to_string(errno) + ")");
	}

	freeaddrinfo(addr);
	if (listen(_listening_socket, SOMAXCONN) == -1)  // max nr of accepted connections
		throw std::runtime_error("Failed listen");
}
