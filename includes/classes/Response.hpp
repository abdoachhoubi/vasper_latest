#pragma once

#include "../main.hpp"

class Server;
class Location;
class Client;
// class Cgi;

class Response
{
public:
	bool _isDirHasIndexFiles;

	// Constructors
	Response(Request &req, Server server);
	Response();
	void set_headers(std::string headers);
	bool isResourceFound(const std::string &fullPath);
	std::string generateResponse(std::string fullPath, int flag, Location location);
	void setServer(Server server);
	std::string get_response();
	void setPath(std::string path);
	std::string getPath();
	std::string gettype();
	void setHeader(const std::string &key, const std::string &value);
	const std::string &getHeader(const std::string &key) const;
	void setBody(const std::string &body);
	const std::string &getBody() const;
	void setStatus(int statusCode, const std::string &statusText);
	std::string toString();
	int respond();
	std::string getContentTypeFromExtension(const std::string &filePath);
	std::string getContentType();
	bool isResourceDeletable(const std::string &resourcePath);
	bool deleteResource(const std::string &resourcePath);
	bool isMethodAllowed(std::vector<std::string> methods, std::string req_method);
	bool saveDataToFile(const std::string &filePath, const std::string &data);
	bool fileExists(const std::string &f);
	std::string parseBoundary(std::string &body, std::string &boundary);
	void SaveDataToFile(const std::string &filePath, const std::string &data);
	int myFind(std::string src, std::string target);
	std::string decodePath(std::string path);
	std::string statusTextGen(int code);
	std::string generateErrorResponse(error_pages code, Server server);
	int checkRedirection(std::vector<Location> loc, std::vector<std::string> sub_uris, std::string loc_path);
	int router(std::vector<Location> loc, std::vector<std::string> sub_uris);

	// Controllers
	int getController(Location location);
	int postController(Location location);
	int deleteController();

	Cgi _cgi_obj;

	void setRequest(Request req);
	std::string get_headers();
	std::string _response;
	std::streampos fileSize;
	size_t content_len;
	bool _check;
	bool isCGI;
	int handleCgi(Location location);

private:
	Request _req;
	Server _server_conf;
	std::string contentType;
	std::string _type;
	std::string _headers;
	std::string _path;
	std::string statusText;
	short statusCode;
	std::string body;
	std::map<std::string, std::string> headers;

	short _cgi_state;

};
