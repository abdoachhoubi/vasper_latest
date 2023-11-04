#pragma once

#include "../main.hpp"

enum error_pages
{
	SUCCESS = 200,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	REQUEST_ENTITY_TOO_LARGE = 413,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	NO_CONTENT = 204,
	MOVED_PERMANENTLY = 301
};

class Location;

class Server
{
public:
	Server();
	~Server();
	Server(const Server &other);
	Server &operator=(const Server &other);

	void setServerName(std::string server_name);
	const std::string &getServerName();

	void setHost(std::string parametr);
	const in_addr_t &getHost();

	void setRoot(std::string root);
	const std::string &getRoot();

	void setFd(int);
	int getFd();

	void setListen(std::string parametr);
	const uint16_t &getPort();

	void setClientMaxBodySize(std::string parametr);
	const size_t &getClientMaxBodySize();

	void setErrorPages(std::vector<std::string> &parametr);
	const std::map<error_pages, std::string> &getErrorPages();

	void setIndex(std::string index);
	const std::string &getIndex();

	void setLocation(std::string nameLocation, std::vector<std::string> parametr);
	const std::vector<Location> &getLocations();

	void setAutoindex(std::string autoindex);
	const bool &getAutoindex();

	void setUploadPath(std::string uploadPath);
	std::string &getUploadPath();


	int isValidLocation(Location &location) const;
	static void checkToken(std::string &parametr);
	bool checkLocations() const;
	static int checkTypePath(std::string const path);
	static int accessFile(std::string const path, int mode);
	static std::string fileToString(std::string path);
	static int isReadableAndExist(std::string const path, std::string const index);
	void createServer();

private:
	std::map<error_pages, std::string> _errorPages;
	std::vector<Location> locations;
	std::string _serverName;
	std::string _root;
	std::string _uploadPath;
	std::string _index;
	uint16_t _port;
	in_addr_t _host;
	unsigned long clientMaxBodySize;
	struct sockaddr_in serveraddress;
	bool autoindex;
	int  listenFd;
};
