#include <sys/stat.h>
#include <iomanip>
#include <dirent.h>

#include "Parser.hpp"

///** Default Constructor */
//Parser::Parser() {
//
//}

std::string Parser::keyParser(std::string & lineContent, std::string const & keyToFind) {
    if (not lineContent.empty() || not keyToFind.empty()) {
        if (lineContent.find(keyToFind) != std::string::npos) {
            std::cout << YEL << lineContent << RES << std::endl;
            return getOneCleanValueFromKey(lineContent, keyToFind);
        }
    }
    return std::string();
}

/* define if path is file(1), folder(2) or something else(3) */
// todo: can we use it? stat
PathType Parser::pathType(std::string const & path) {
    struct stat	buffer = {};

    /* The & operator computes the logical AND of its operands. The result of x & y is true if both x and y
     * evaluate to true. Otherwise, the result is false */
    DIR* dir = opendir(path.c_str());
    if (stat(path.c_str(), &buffer) == 0) {
        if (buffer.st_mode & S_IFREG) {
            return (REG_FILE);
        } else if (buffer.st_mode & S_IFDIR || dir != NULL) {
            (void)closedir(dir);
            return (DIRECTORY);
        } else
            return (OTHER_PATH_TYPE);
    }
    return (PATH_TYPE_ERROR);
}

std::string Parser::addCurrentDirPath(std::string const & fileOrDir) const {
    if (fileOrDir.at(0) != '.') {
        return "./";
    }
    return std::string();
}

std::string Parser::isPath(std::string const & rootDirectory, std::string const & possiblePath) {
    std::string is_root_dir_or_has_path = possiblePath;
    if (possiblePath.find('/') == std::string::npos) {
        std::string::size_type lastIndex = rootDirectory.size() - 1;
        if (rootDirectory[lastIndex] == '/') {
            is_root_dir_or_has_path = rootDirectory + possiblePath;
        } else {
            is_root_dir_or_has_path = rootDirectory + "/" + possiblePath;
        }
    }
    return is_root_dir_or_has_path;
}

bool Parser::isSpace(char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}

//std::string Parser::removeSpaces(std::string content) {
//    while (!content.empty()) {
//        content.erase(std::find(content.begin(), content.end(), ' '));
//    }
//    return content;
//}

std::string Parser::getOneCleanValueFromKey(std::string & contentLine, std::string const & key) {
	std::string content = contentLine.substr(contentLine.find(key) + key.size());
	if (content.empty())	// added jaka, it was giving error abort, when the content was empty string
		return "Key is empty!\n";
	size_t i = 0;
	for (; content.at(i) == ' '; i++) {}
	std::string trimmed_content = content.substr(i);
    if (trimmed_content.find(';') != std::string::npos) {
        return trimmed_content.substr(0, trimmed_content.size() - 1);
    }
	return trimmed_content.substr(0, trimmed_content.size() - 0);
}

DataType Parser::getValueType(std::string & lineContent) {// Todo: Maybe not needed
	if (lineContent.find("server_name") != std::string::npos) {
		return STRING;
	} else if (lineContent.find("listens_to") != std::string::npos) {
		return PORT;
	} else if (lineContent.find("ip_address") != std::string::npos) {
		return IP_ADDRESS;
	} else if (lineContent.find("root_directory") != std::string::npos) {
		std::string value = getOneCleanValueFromKey(lineContent, "root_directory");
		if (value[0] == '/') {
			return ABSOLUTE_PATH;
		} else {
			/* Will be set to ./$server_name */
			return RELATIVE_PATH;
		}
	} else if (lineContent.find("index_file") != std::string::npos) {
		if (lineContent.find(".html") != std::string::npos) {
			return RELATIVE_PATH_HTML;
		}
	} else if (lineContent.find("client_max_body_size") != std::string::npos) {
		return BYTES;
	} else if (lineContent.find("error_page") != std::string::npos) {
		std::string value = getOneCleanValueFromKey(lineContent, "error_page");
		if (value[0] == '/') {
			return ABSOLUTE_PATH_HTML;
		} else {
			/* Will be set to ./$server_name */
			return RELATIVE_PATH_HTML;
		}
	} else if (lineContent.find("port_redirection") != std::string::npos) {
		return PORT;
	} else if (lineContent.find("location") != std::string::npos) {
		return RELATIVE_PATH;
	} else if (lineContent.find("allow_methods") != std::string::npos) {
		return HTTP_METHOD;
	} else if (lineContent.find("auto_index") != std::string::npos) {
		return BOOL;
	}
	return STRING;
	// todo: add cgi location?
}

