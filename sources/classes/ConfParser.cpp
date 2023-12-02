#include "../../includes/main.hpp"

ConfParser::ConfParser() : nbr_serv(0) {}
ConfParser::~ConfParser() {}
std::vector<Server> ConfParser::getServers() {return servers;}

void ConfParser::parse(const std::string &config_file)
{
	std::string content = Server::fileToString(config_file);
	splitServers(content);
	if (serv_conf.size() != nbr_serv)
		throw std::runtime_error("Error: Server config file is not well formated 100");

	for (size_t i = 0; i < nbr_serv; i++)
	{
		Server server;
		createServer(serv_conf[i], server);
		servers.push_back(server);
	}
	if (servers.empty())
		throw std::runtime_error("Error: Server config file is not well formated 10");
	for (size_t i = 0; i < servers.size(); i++)
		for (size_t j = i + 1; j < servers.size(); j++)
			if (servers[i].getPort() == servers[j].getPort() && servers[i].getHost() == servers[j].getHost() && servers[i].getServerName() == servers[j].getServerName())
				throw std::runtime_error("Error: Server config file is not well formated 11");

	for (size_t i = 0; i < servers.size(); i++)
		for (size_t j = i + 1; j < servers.size();)
		{
			if (servers[i].getPort() == servers[j].getPort() && servers[i].getHost() == servers[j].getHost())
				servers.erase(servers.begin() + j);
			else
				j++;
		}

	nbr_serv = servers.size();
}

void ConfParser::splitServers(std::string &content)
{
	size_t start = 0;
	size_t end = 1;

	while (start != end && start < content.length())
	{
		start = startServer(start, content) + 1;
		end = endServer(start, content);
		if (end == start)
			break;
		serv_conf.push_back(content.substr(start, end - start + 1));
		nbr_serv++;
		start = end;
		start++;
	}
}

size_t ConfParser::startServer(size_t start, std::string &content)
{
	size_t pos = start;

	for (; pos < content.size() && isspace(content[pos]); pos++)
		if (content[pos] == 's')
			break;
	if (!content[pos])
		return start;
	if (content.compare(pos, 6, "server") != 0)
		throw std::runtime_error("Error: Wrong character 1");
	pos += 6;
	while (pos < content.size() && isspace(content[pos]))
		pos++;
	if (content[pos] != '{')
		throw std::runtime_error("Error: Wrong character");
	return (pos);
}

size_t ConfParser::endServer(size_t start, std::string &content)
{
	size_t pos = start;
	int count = 0;

	for (pos = start + 1; pos < content.size(); pos++)
	{
		if (content[pos] == '{')
			count++;
		if (content[pos] == '}')
		{
			if (count == 0)
				return (pos + 1);
			count--;
		}
	}
	return (start);
}

void ConfParser::createServer(std::string &config, Server &server)
{
	std::vector<std::string> line = splitParametrs(config, "\n");
	std::vector<std::string> error_page;

	for (size_t i = 0; i < line.size(); i++)
	{
		std::string str = line[i];
		Server::checkToken(str);
		std::vector<std::string> param = ft_split(str);
		if (param.size() > 1)
		{
			std::string key = param[0];
			std::vector<std::string> value;
			for (size_t i = 1; i < param.size(); i++)
				value.push_back(param[i]);
			if (key == "listen")
			{
				if (value.size() != 1)
					throw std::runtime_error("Error: Server config file is not well formated 4");
				server.setListen(value[0]);
			}
			else if (key == "server_name")
			{
				if (value.size() != 1)
					throw std::runtime_error("Error: Server config file is not well formated 5");
				server.setServerName(value[0]);
			}
			else if (key == "error_page")
				server.setErrorPages(value);
			else if (key == "location")
			{
				if (value.size() != 2)
					throw std::runtime_error("Error: Server config file is not well formated 6");
				std::string name = value[0];
				if (value[1] != "{")
					throw std::runtime_error("Error: Server config file is not well formated 7");
				std::vector<std::string> location;
				while (i < line.size() && strtrim(line[i]) != "}")
				{
					Server::checkToken(line[i]);
					location.push_back(line[i++]);
				}
				if (strtrim(line[i]) != "}")
					throw std::runtime_error("Error: Server config file is not well formated 8");
				server.setLocation(name, location);
			}
			else if (key == "host")
				server.setHost(value[0]);
			else if (key == "client_max_body_size")
				server.setClientMaxBodySize(value[0]);
			else if (key == "root")
				server.setRoot(value[0]);
			else if (key == "index")
				server.setIndex(value[0]);
			else if (key == "autoindex")
				server.setAutoindex(value[0]);
			else if (key == "upload_path")
				server.setUploadPath(value[0]);
		}
		else
		{
			if (param.empty() || param[0] == "}" || param[0] == "{")
				continue;
			else
				throw std::runtime_error("Error: Server config file is not well formated 9");
		}
	}
	if (Server::isReadableAndExist(server.getUploadPath(), "") < 0)
		throw std::runtime_error("Error: upload folder not exist 2");
	if (!server.checkLocations())
		throw std::runtime_error("Error: in Location from config file");
}
/* define is path is file(1), folder(2) or something else(3) */
int ConfParser::getTypePath(std::string const path)
{
	struct stat buffer;
	int result;

	result = stat(path.c_str(), &buffer);
	if (result == 0)
	{
		if (buffer.st_mode & S_IFREG)
			return (1);
		else if (buffer.st_mode & S_IFDIR)
			return (2);
		else
			return (3);
	}
	else
		return (-1);
}