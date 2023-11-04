#include "../../includes/main.hpp"

/* Constructor */
Cgi::Cgi()
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
}

Cgi::Cgi(std::string path)
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = path;
	this->_ch_env = NULL;
	this->_argv = NULL;
}

Cgi::~Cgi()
{

	if (this->_ch_env)
	{
		for (int i = 0; this->_ch_env[i]; i++)
			free(this->_ch_env[i]);
		free(this->_ch_env);
	}
	if (this->_argv)
	{
		for (int i = 0; this->_argv[i]; i++)
			free(_argv[i]);
		free(_argv);
	}
	this->_env.clear();
}

Cgi::Cgi(const Cgi &other)
{
	this->_env = other._env;
	this->_ch_env = other._ch_env;
	this->_argv = other._argv;
	this->_cgi_path = other._cgi_path;
	this->_cgi_pid = other._cgi_pid;
	this->_exit_status = other._exit_status;
}

Cgi &Cgi::operator=(const Cgi &other)
{
	if (this != &other)
	{
		this->_env = other._env;
		this->_ch_env = other._ch_env;
		this->_argv = other._argv;
		this->_cgi_path = other._cgi_path;
		this->_cgi_pid = other._cgi_pid;
		this->_exit_status = other._exit_status;
	}
	return (*this);
}

/*Set functions */
void Cgi::setCgiPid(pid_t cgi_pid) { this->_cgi_pid = cgi_pid; }
void Cgi::setCgiPath(const std::string &cgi_path) { this->_cgi_path = cgi_path; }

/* Get functions */
const std::map<std::string, std::string> &Cgi::getEnv() const { return (this->_env); }
const pid_t &Cgi::getCgiPid() const { return (this->_cgi_pid); }
const std::string &Cgi::getCgiPath() const { return (this->_cgi_path); }

/* initialization environment variable */
void Cgi::initEnv(Request &req, Location &location)
{
	this->req = req;
	std::string extension;
	std::string ext_path;

	extension = this->_cgi_path.substr(this->_cgi_path.find("."));
	ext_path = location._ext_path[extension];
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = this->_cgi_path;
	this->_env["SCRIPT_FILENAME"] = this->_cgi_path;
	this->_env["PATH_INFO"] = this->_cgi_path;
	this->_env["QUERY_STRING"] = decode(req.getQuery());
	this->_env["HTTP_COOKIE"] = req.getHeader("Cookie");
	this->_env["HTTP_HOST"] = req.getHeader("host").substr(0, req.getHeader("host").find(":"));
	this->_env["HTTP_PORT"] = req.getHeader("host").substr(req.getHeader("host").find(":") + 1);
	this->_env["HTTP_USER_AGENT"] = req.getHeader("user-agent");
	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["REQUEST_METHOD"] = req.getMethodStr();
	this->_env["REMOTE_ADDR"] = req.getHeader("host").substr(0, req.getHeader("host").find(":"));
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["REQUEST_URI"] = req.getPath() + req.getQuery();
	this->_env["SERVER_SOFTWARE"] = req.getServerName();
	if (req.getMethodStr() == "POST")
	{
		this->_env["CONTENT_LENGTH"] = req.getHeader("content-length");
		this->_env["CONTENT_TYPE"] = req.getHeader("content-type");
	}
	this->_ch_env = (char **)calloc(sizeof(char *), this->_env.size() + 1);
	std::map<std::string, std::string>::const_iterator it = this->_env.begin();
	for (int i = 0; it != this->_env.end(); it++, i++)
	{
		std::string tmp = it->first + "=" + it->second;
		this->_ch_env[i] = strdup(tmp.c_str());
	}
	this->_argv = (char **)malloc(sizeof(char *) * 3);
	this->_argv[0] = strdup(ext_path.c_str());
	this->_argv[1] = strdup(this->_cgi_path.c_str());
	this->_argv[2] = NULL;
}

void Cgi::execute(short &error_code)
{
	if (this->_argv[0] == NULL || this->_argv[1] == NULL)
	{
		error_code = INTERNAL_SERVER_ERROR;
		return;
	}

	int stdin_pipe[2];
	int stdout_pipe[2];

	if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1)
	{
		std::cerr << RED_BOLD << "pipe() failed" << RESET << std::endl;
		error_code = INTERNAL_SERVER_ERROR;
		return;
	}

	this->_cgi_pid = fork();

	if (this->_cgi_pid == 0)
	{
		// Close the write end of the stdin pipe and the read end of the stdout pipe
		close(stdin_pipe[1]);
		close(stdout_pipe[0]);

		// Redirect stdin and stdout to the pipes
		dup2(stdin_pipe[0], STDIN_FILENO);
		dup2(stdout_pipe[1], STDOUT_FILENO);

		// Execute the CGI script
		this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);

		std::cerr << "EXECVE FAILED" << std::endl;
		exit(this->_exit_status);
	}
	else if (this->_cgi_pid > 0)
	{
		// Close the read end of the stdin pipe and the write end of the stdout pipe
		close(stdin_pipe[0]);
		close(stdout_pipe[1]);

		// Write the request body to the stdin of the CGI script
		write(stdin_pipe[1], req.getBody().c_str(), req.getBody().length());
		close(stdin_pipe[1]);

		// Read the output from the stdout of the CGI script
		char buffer[BUFSIZ];
		ssize_t read_len;

		filex = open("./vasper.cgi", O_CREAT | O_TRUNC | O_RDWR, 0777);
		while ((read_len = read(stdout_pipe[0], buffer, BUFSIZ)) > 0)
		{
			write(filex, buffer, read_len); // Write to the file or stdout as needed
		}

		close(stdout_pipe[0]);

		// Wait for the CGI script to complete
		waitpid(this->_cgi_pid, &this->_exit_status, 0);
		error_code = 200;
	}
	else
	{
		std::cout << "Fork failed" << std::endl;
		error_code = INTERNAL_SERVER_ERROR;
	}
}

int Cgi::findStart(const std::string path, const std::string delim)
{
	if (path.empty())
		return (-1);
	size_t poz = path.find(delim);
	if (poz != std::string::npos)
		return (poz);
	else
		return (-1);
}

/* Translation of parameters for QUERY_STRING environment variable */
std::string Cgi::decode(std::string &path)
{
	size_t token = path.find("%");
	while (token != std::string::npos)
	{
		if (path.length() < token + 2)
			break;
		char decimal = fromHexToDec(path.substr(token + 1, 2));
		path.replace(token, 3, to_string(decimal));
		token = path.find("%");
	}
	return (path);
}

void Cgi::clear()
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
	this->_env.clear();
}

std::string Cgi::getResponse()
{
	std::string response;

	close(filex);
	std::ifstream file("./vasper.cgi");
	if (file.is_open())
	{
		std::string line;
		while (getline(file, line, '\0'))
			response += line;
		file.close();
	}
	else if (!file.good())
		std::cout << "Unable to open CGI file" << std::endl;
	response += "\0";
	return (response);
}
