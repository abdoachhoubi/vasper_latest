#pragma once

#include "../main.hpp"

class Request;
class Location;

class Cgi
{
private:
	std::map<std::string, std::string> _env;
	char **_ch_env;
	char **_argv;
	int _exit_status;
	std::string _cgi_path;
	pid_t _cgi_pid;
	int	inputpip;
	int	outputpip;
	Request req;

public:
	int	know;
	int filex;
	Cgi();
	Cgi(std::string path);
	~Cgi();
	Cgi(Cgi const &other);
	Cgi &operator=(Cgi const &other);

	void initEnv(Request &req, Location &location);
	void execute(short &error_code);
	void clear();

	void setCgiPath(const std::string &cgi_path);

	const std::string &getCgiPath() const;
	std::string decode(std::string &path);

	std::string getResponse();
};
