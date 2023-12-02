#include "../../includes/main.hpp"

Cgi::Cgi()
{
	this->_cgi_pid = -1;
	this->inputpip = 0;
	this->outputpip = 1;
	this->know = 0;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
}

Cgi::Cgi(std::string path)
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->inputpip = 0;
	this->outputpip = 1;
	this->know = 0;
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
	*this = other;
}

Cgi &Cgi::operator=(const Cgi &other)
{
	if (this != &other)
	{
		this->_env = other._env;
		this->_ch_env = other._ch_env;
		this->_argv = other._argv;
		this->know = other.know;
		this->_cgi_path = other._cgi_path;
		this->inputpip = other.inputpip;
		this->outputpip = other.outputpip;
		this->_cgi_pid = other._cgi_pid;
		this->_exit_status = other._exit_status;
	}
	return (*this);
}

// TAG - Setters
void Cgi::setCgiPath(const std::string &cgi_path) { this->_cgi_path = cgi_path; }

// TAG - Getters
const std::string &Cgi::getCgiPath() const { return (this->_cgi_path); }

// TAG - Initialize the environment variables
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

// TAG - The main function that executes the CGI script
void Cgi::execute(short &error_code)
{
	if (this->know == 0)
	{
		this->know = 1;
		if (this->_argv[0] == NULL || this->_argv[1] == NULL)
		{
			error_code = INTERNAL_SERVER_ERROR;
			return;
		}

		// TAG - Create the files (out and in)
		if (req.getMethodStr() == "POST")
			this->outputpip = open("./outputpip", O_RDWR | O_CREAT | O_TRUNC, 0666);
		write(this->outputpip, req.getBody().c_str(), req.getBody().length());
		this->inputpip = open("./inputpip", O_RDWR | O_CREAT | O_TRUNC, 0666);
		if (this->inputpip == -1)
			throw std::runtime_error("open error");

		this->_cgi_pid = fork();

		if (this->_cgi_pid == 0)
		{
			// TAG - Configuring redirection of stdin and stdout
			if (req.getMethodStr() == "POST")
			{
				close(this->outputpip);
				outputpip = open("./outputpip", O_RDONLY);
				dup2(outputpip, STDIN_FILENO);
				close(this->outputpip);
			}
			dup2(inputpip, STDOUT_FILENO);
			close(this->inputpip);

			// TAG - Execute the CGI script
			this->_exit_status = execve(this->_argv[0], this->_argv, this->_ch_env);

			std::cerr << "EXECVE FAILED" << std::endl;
			exit(this->_exit_status);
		}
		else if (this->_cgi_pid > 0)
		{
			// TAG - Close the files (out and in)
			close(inputpip);
			if (req.getMethodStr() == "POST")
				close(outputpip);
		}
		else
		{
			std::cerr << "Fork failed" << std::endl;
			error_code = INTERNAL_SERVER_ERROR;
		}
	}
	else
	{
		int res = waitpid(this->_cgi_pid, &this->_exit_status, WNOHANG);
		if (res == 0)
		{
			return;
		}
		else
		{
			// TAG - Check the status of the CGI script
			if (WIFEXITED(this->_exit_status))
			{
				this->know = 2;
				this->inputpip = open("./inputpip", O_RDONLY);
				// TAG - Read the response from the CGI script
				char buffer[BUFSIZ];
				ssize_t read_len;

				filex = open("./vasper.cgi", O_CREAT | O_TRUNC | O_RDWR, 0777);
				while ((read_len = read(this->inputpip, buffer, BUFSIZ)) > 0)
					write(filex, buffer, read_len);
				close(this->inputpip);
				error_code = 200;
			}
		}
	}
}

// TAG - Decode the path for query environment variable
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

// TAG - Clear the CGI class
void Cgi::clear()
{
	this->_cgi_pid = -1;
	this->_exit_status = 0;
	this->_cgi_path = "";
	this->_ch_env = NULL;
	this->_argv = NULL;
	this->_env.clear();
}

// TAG - Get the response from the CGI script
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
