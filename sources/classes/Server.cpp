#include "../../includes/main.hpp"

// TAG: ORTHODOX CANONICAL FORM
Server::Server() {}
Server::~Server() {}
Server::Server(const Server &other) { *this = other; }
Server &Server::operator=(const Server &other)
{
	if (this != &other)
	{
		_port = other._port;
		_host = other._host;
		_serverName = other._serverName;
		_root = other._root;
		clientMaxBodySize = other.clientMaxBodySize;
		_index = other._index;
		autoindex = other.autoindex;
		_errorPages = other._errorPages;
		locations = other.locations;
		serveraddress = other.serveraddress;
		listenFd = other.listenFd;
		_uploadPath = other._uploadPath;
	}
	return (*this);
}

// TAG: GETTERS
const std::string &Server::getServerName() { return _serverName; }
const uint16_t &Server::getPort() { return _port; }
const in_addr_t &Server::getHost() { return _host; }
const size_t &Server::getClientMaxBodySize() { return clientMaxBodySize; }
const std::vector<Location> &Server::getLocations() { return locations; }
const std::string &Server::getRoot() { return _root; }
const std::string &Server::getIndex() { return _index; }
const bool &Server::getAutoindex() { return autoindex; }
int Server::getFd() { return listenFd; }
void Server::setRoot(std::string root) { this->_root = root; }
std::string &Server::getUploadPath() { return _uploadPath; }
const std::map<error_pages, std::string> &Server::getErrorPages() { return _errorPages; }

// TAG: SETTERS
void Server::setFd(int fd) { listenFd = fd; }
void Server::setListen(std::string parametr) { _port = atoi(parametr.c_str()); }
void Server::setClientMaxBodySize(std::string parametr) { clientMaxBodySize = atof(parametr.c_str()); }
void Server::setUploadPath(std::string uploadPath) { this->_uploadPath = uploadPath; }
void Server::setServerName(std::string server_name) { _serverName = server_name; }
void Server::setIndex(std::string index) { this->_index = index; }
void Server::setHost(std::string parametr)
{
	struct sockaddr_in addr;

	if (parametr == "localhost")
		parametr = "127.0.0.1";
	if (inet_pton(AF_INET, parametr.c_str(), &addr.sin_addr) == 0)
		throw std::runtime_error("Error: invalid host");
	_host = inet_addr(parametr.c_str());
}

void Server::setErrorPages(std::vector<std::string> &parametr)
{
	if (parametr.size() % 2 != 0)
		throw std::runtime_error("Error: invalid error_page 1");
	for (size_t i = 0; i < parametr.size(); i++)
	{
		for (size_t j = 0; j < parametr[i].size(); j++)
		{
			if (!std::isdigit(parametr[i][j]))
				throw std::runtime_error("Error: invalid error_page 2");
		}
		if (atoi(parametr[i].c_str()) < 100 || atoi(parametr[i].c_str()) > 599)
			throw std::runtime_error("Error: invalid error_page 3");
		error_pages key = static_cast<error_pages>(atoi(parametr[i].c_str()));
		++i;
		std::string value = parametr[i];
		value = (_root[_root.size() - 1] == '/') ? _root + value : _root + "/" + value;
		if (Server::checkTypePath(value) != 2)
			throw std::runtime_error("Error: invalid error_page 4");
		if (Server::accessFile(value, F_OK) < 0 || Server::accessFile(value, R_OK) < 0)
			throw std::runtime_error("Error: invalid error_page");
		_errorPages[key] = value;
	}
}
void Server::setLocation(std::string nameLocation, std::vector<std::string> parametr)
{
	Location location;
	bool checkRoot = false;
	for (size_t i = 0; i < parametr.size(); i++)
	{
		std::string str = parametr[i];
		Server::checkToken(str);
		std::vector<std::string> param = ft_split(str);
		if (param.size() > 1)
		{
			std::string key = param[0];
			std::vector<std::string> value;
			for (size_t i = 1; i < param.size(); i++)
				value.push_back(param[i]);
			if (!checkRoot)
				location.setRootLocation(_root);
			if (key == "root")
			{
				checkRoot = true;
				location.setRootLocation(value[0]);
			}
			else if (key == "allow_methods")
			{
				location.setMethods(value);
				location.setAllowedMethods(value);
			}
			else if (key == "autoindex")
				location.setAutoindex(value[0]);
			else if (key == "index")
				location.setIndexLocation(value[0]);
			else if (key == "return")
				location.setReturn(value[0]);
			else if (key == "cgi")
				location.setCGI(value[0]);
			else if (key == "cgi_path")
				location.setCgiPath(value);
			else if (key == "cgi_extension")
				location.setCgiExtension(value);
		}
	}
	location.setPath(nameLocation);
	locations.push_back(location);
}
void Server::setAutoindex(std::string autoindex)
{
	if (autoindex == "on")
		this->autoindex = true;
	else if (autoindex == "off")
		this->autoindex = false;
	else
		throw std::runtime_error("Error: invalid autoindex");
}

// TAG: Check if location is valid when it comes to CGI params
int Server::isValidLocation(Location &location) const
{

	if (location.getCGI())
	{
		if (location.getCgiPath().empty() || location.getCgiExtension().empty())
			return (0);
		if (location.getCgiPath().size() != location.getCgiExtension().size())
			return (0);
		std::vector<std::string>::const_iterator it;
		for (it = location.getCgiPath().begin(); it != location.getCgiPath().end(); ++it)
		{
			if (ConfParser::getTypePath(*it) < 0)
				return (0);
		}
		std::vector<std::string> cgi_exts = location.getCgiExtension();
		std::vector<std::string> paths = location.getCgiPath();
		int i = 0;
		while (i < (int)cgi_exts.size())
		{
			location._ext_path.insert(std::make_pair(cgi_exts[i], paths[i]));
			i++;
		}
		if (location.getCgiPath().size() != location.getExtensionPath().size())
			return (0);
	}
	return 1;
}

// TAG: Check if token is valid
void Server::checkToken(std::string &parametr)
{
	std::string::size_type pos = parametr.find('#');
	if (pos != std::string::npos)
		parametr.erase(pos, parametr.find('\n', pos) - pos);
	pos = parametr.find(";");
	if (pos != std::string::npos)
		parametr.erase(pos);
	parametr = strtrim(parametr);
}

// TAG: Loop through all locations and check if they are valid
bool Server::checkLocations() const
{
	std::vector<Location>::const_iterator it = locations.begin();
	std::vector<Location>::const_iterator ite = locations.end();
	for (; it != ite; ++it)
	{
		if (!isValidLocation(const_cast<Location &>(*it)))
		{
			std::cerr << "Error: invalid location ---> " << it->getPath() << std::endl;
			return false;
		}
	}
	return true;
}

int Server::checkTypePath(std::string const path)
{
	// TAG: Check if path is directory or file
	struct stat buf;
	if (stat(path.c_str(), &buf) == -1)
		return -1;
	if (S_ISDIR(buf.st_mode))
		return 1;
	if (S_ISREG(buf.st_mode))
		return 2;
	return 0;
}

// TAG: Check if file is readable
int Server::accessFile(std::string const path, int mode)
{
	if (access(path.c_str(), mode) == -1)
		return -1;
	return 0;
}

// TAG: Read file and return it as a string
std::string Server::fileToString(std::string path)
{
	std::string file;
	std::string line;
	std::ifstream in(path.c_str());

	if (!in.is_open())
		throw std::runtime_error("Error: file " + path + " not found");
	while (getline(in, line))
		file += line + "\n";
	in.close();
	return file;
}

// TAG: Check if file is readable and exists
int Server::isReadableAndExist(std::string const path, std::string const index)
{
	int type;
	std::string path_p = path + "/" + index;
	type = checkTypePath(path_p);
	if (type == -1)
		return -1;
	if (type == 1)
		if ((accessFile(path_p, R_OK | X_OK)) == -1)
			return -1;
	if (type == 2)
		if ((accessFile(path_p, R_OK)) == -1)
			return -1;
	if (type == 0)
		if ((accessFile(path_p, R_OK)) == -1)
			return -1;
	return 0;
}

// TAG: Creates a server based on the information in the server object
void Server::createServer(void)
{
	int option_value = 1;
	if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "Listen failed: Unable to listen on the socket."
				  << " Closing ...." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(int)) < 0)
	{
		std::cerr << "Error: cannot reuse socket" << std::endl;
		exit(EXIT_FAILURE);
	}
	memset(&serveraddress, 0, sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_addr.s_addr = _host;
	serveraddress.sin_port = htons(_port);
	if (bind(listenFd, (struct sockaddr *)&serveraddress, sizeof(serveraddress)) == -1)
	{
		std::cerr << "Bind failed: Unable to bind the socket."
				  << " Closing ...." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (listen(listenFd, SOMAXCONN) == -1)
	{
		std::cerr << "Listen failed: Unable to listen on the socket." << std::endl;
		close(listenFd);
		exit(EXIT_FAILURE);
	}
	if (fcntl(listenFd, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Fcntl failed: Unable to set file descriptor options." << std::endl;
		close(listenFd);
		exit(EXIT_FAILURE);
	}
}
