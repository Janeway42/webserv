#include "ServerLocation.hpp"

/** Default constructor */
//ServerLocation::ServerLocation()// todo private???
//    /** Initializing default values for the location block */
//    : _is_location_cgi(false),
//    _root_directory(std::string()),
//    _allow_methods(),
//    _index_file(std::string()),
//    _auto_index(false),
//    _interpreter_path(std::string()),
//    _script_extension(std::string()) {
//    std::cout << CYN << "ServerLocation Default constructor" << RES << std::endl;
//}

/** Overloaded constructor */
ServerLocation::ServerLocation(std::string const & server_root_directory, std::string const & server_index_file)
    /** Initializing default values for the location block */
    : _is_location_cgi(false),
    _location_path(std::string()),
    _root_directory(server_root_directory),
    _index_file(server_index_file),
    _auto_index(false),
    _interpreter_path(std::string()),
    _script_extension(std::string()) {
    _allow_methods.push_back(GET);
    std::cout << CYN << "ServerLocation Overloaded constructor" << RES << std::endl;
}

/** Destructor */
ServerLocation::~ServerLocation() {
    /** Cleaning default values for the location block */
    _is_location_cgi = false;
    _location_path.clear();
    _root_directory.clear();
    _allow_methods = std::vector<AllowMethods>(NONE);
    _index_file.clear();
    _auto_index = false;
    _interpreter_path.clear();
    _script_extension.clear();
    std::cout << CYN << "ServerLocation Destructor" << RES << std::endl;
}

/** #################################### Methods #################################### */

bool ServerLocation::isLocationCgi() const {
    return _is_location_cgi;
}

/** #################################### Getters #################################### */

std::string ServerLocation::getLocationPath() const {
    return _location_path;
}

std::string ServerLocation::getRootDirectory() const {
    return _root_directory;
}

std::vector<AllowMethods> ServerLocation::getAllowMethods() const {
    return _allow_methods;
}

std::string ServerLocation::getIndexFile() const {
    return _index_file;
}

bool ServerLocation::getAutoIndex() const {
    return _auto_index;
}

std::string ServerLocation::getInterpreterPath() const {
    return _interpreter_path;
}

std::string ServerLocation::getScriptExtension() const {
    return _script_extension;
}

/** #################################### Setters #################################### */

void ServerLocation::setLocationAsCgi(bool isCgi) {
    /* not mandatory | default: python cgi with a default index.html inside */
    _is_location_cgi = isCgi;
}

bool ServerLocation::setLocationPath(std::string const & locationPath) {
    /* not mandatory | if request contains an uri directory path, it can be made accessible by making it a location block */
    if (not locationPath.empty()) {
        std::string location_path = addRootDirectoryPath(_root_directory, locationPath);
        PathType type = pathType(location_path);
        if (type == DIRECTORY) {
            _location_path = addCurrentDirPath(location_path) + location_path;
            return true;
        } else if (type == PATH_TYPE_ERROR) {
            throw ParserException(CONFIG_FILE_ERROR("location block", MISSING));
        } else {
            throw ParserException(CONFIG_FILE_ERROR("location block", NOT_SUPPORTED));
        }
    }
    return false;
}

bool ServerLocation::setRootDirectory(std::string const & rootDirectory) {
    /* not mandatory | default: $server.root_directory */
    if (not rootDirectory.empty()) {
        PathType type = pathType(rootDirectory);
        if (type == DIRECTORY) {
            _root_directory = addCurrentDirPath(rootDirectory) + rootDirectory;
            return true;
        } else if (type == PATH_TYPE_ERROR) {
            throw ParserException(CONFIG_FILE_ERROR("root_directory", MISSING));
        } else {
            throw ParserException(CONFIG_FILE_ERROR("root_directory", NOT_SUPPORTED));
        }
    } else {
        /* cgi -> mandatory */
        if (isLocationCgi()) {
            throw ParserException(CONFIG_FILE_ERROR("root_directory", MANDATORY));
        }
    }
    return false;
}

bool ServerLocation::setAllowMethods(std::string const & allowMethods) {
    /* not mandatory | default: GET */
    if (not allowMethods.empty()) {
        std::vector<AllowMethods> locationAllowMethods;
        if (allowMethods.find("GET") != std::string::npos) {
            locationAllowMethods.push_back(GET);
        }
        if (allowMethods.find("POST") != std::string::npos) {
            locationAllowMethods.push_back(POST);
        }
        if (allowMethods.find("DELETE") != std::string::npos) {
            locationAllowMethods.push_back(DELETE);
        }
        std::string::size_type numberOfSpaces = 0;
        for (std::string::size_type it = 0; allowMethods[it] ; it++) {
            if (isSpace(allowMethods[it])) {
                numberOfSpaces++;
            }
        }
        if (locationAllowMethods.size() == numberOfSpaces) {
            throw ParserException(CONFIG_FILE_ERROR("allow_methods", NOT_SUPPORTED));
        }
        _allow_methods = locationAllowMethods;
        return true;
    }
    return false;
}

bool ServerLocation::setIndexFile(std::string const & indexFile) {
    /* not mandatory | default: index_file */
    /* cgi -> not mandatory | default: stays in the same html page */
    if (not indexFile.empty()) {
        std::string index_file = addRootDirectoryPath(_root_directory, indexFile);
        if (pathType(index_file) == REG_FILE) {
            /* if the index_file (with the root_directory added to it) exists and is a regular file, it can be added
             * * to the _index_file private variable (without the root_directory added to it, since for a location, if
             * * auto_index is on, this same index_file would be used for the subdirectories) */
            _index_file = indexFile;
            return true;
        } else {
            throw ParserException(CONFIG_FILE_ERROR("index_file", NOT_SUPPORTED));
        }
    }
    return false;
}

bool ServerLocation::setAutoIndex(std::string const & autoIndex) {
    /* not mandatory | default: off */
    if (not autoIndex.empty()) {
        if (autoIndex.find("on") != std::string::npos) {
            _auto_index = true;
        } else if (autoIndex.find("off") != std::string::npos) {
            _auto_index = false;
        } else {
            throw ParserException(CONFIG_FILE_ERROR("auto_index", NOT_SUPPORTED));
        }
        return true;
    }
    return false;
}

bool ServerLocation::setInterpreterPath(std::string const & interpreterPath) {
    /* mandatory */
    if (not interpreterPath.empty()) {
        if (interpreterPath[0] != '/') {
            throw ParserException(CONFIG_FILE_ERROR("interpreter_path", NOT_SUPPORTED));
        } else {
            _interpreter_path = interpreterPath;
            return true;
        }
    } else {
        if (isLocationCgi()) {
            throw ParserException(CONFIG_FILE_ERROR("interpreter_path", MANDATORY));
        }
    }
    return false;
}

bool ServerLocation::setScriptExtension(std::string const & scriptExtension) {
    /* mandatory */
    if (not scriptExtension.empty()) {
        if (scriptExtension == ".py") {// todo or php?
            _script_extension = scriptExtension;
            return true;
        } else {
            throw ParserException(CONFIG_FILE_ERROR("script_extension", NOT_SUPPORTED));
        }
    } else {
        if (isLocationCgi()) {
            throw ParserException(CONFIG_FILE_ERROR("script_extension", MANDATORY));
        }
    }
    return false;
}
