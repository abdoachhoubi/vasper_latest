#include "../../includes/main.hpp"

Multiplexer::Multiplexer()
{
	fdmax = 0;
	_recv_fds = fd_set();
	_write_fds = fd_set();
}

Multiplexer::~Multiplexer() {}

void Multiplexer::createServers(std::vector<Server> servers)
{
	_servers = servers;
	std::cout << RED_BOLD << "Creating Servers" << RESET << std::endl;
	char buf[INET_ADDRSTRLEN];
	for (std::vector<Server>::iterator it = _servers.begin(); it != _servers.end(); ++it)
	{
		it->createServer();
		std::cout << BLUE_BOLD << "Server " << it->getServerName().c_str() << " Created and listening on: http://" << inet_ntop(AF_INET, &it->getHost(), buf, INET_ADDRSTRLEN) << ":" << it->getPort() << RESET << std::endl;
	}
}

void Multiplexer::init_fds()
{
	FD_ZERO(&_recv_fds);
	FD_ZERO(&_write_fds);

	for (size_t i = 0; i < _servers.size(); i++)
	{
		addToSet(_servers[i].getFd(), _recv_fds);
		_servers_map[_servers[i].getFd()] = _servers[i];
	}
}

void Multiplexer::addToSet(const int i, fd_set &set)
{
	FD_SET(i, &set);
	fdmax = (fdmax < i) ? i : fdmax;
}

void Multiplexer::removeFromSet(const int i, fd_set &set)
{
	FD_CLR(i, &set);
	if (i == fdmax)
		fdmax--;
}

void Multiplexer::closeConnection(const int i)
{
	if (FD_ISSET(i, &_write_fds))
		removeFromSet(i, _write_fds);
	if (FD_ISSET(i, &_recv_fds))
		removeFromSet(i, _recv_fds);
	close(i);
	_clients_map.erase(i);
}

void Multiplexer::readRequest(const int &i, Client &client)
{
	char buffer[BUFFER_SIZE];
	int bytes_read = 0;
	bytes_read = read(i, buffer, BUFFER_SIZE);
	if (bytes_read < 0)
	{
		std::cerr << "webserv: fd " << i << "Read failed: Unable to read data from the file." << std::endl;
		closeConnection(i);
		return;
	}
	else if (bytes_read == 0)
		return ;
	else
	{
		client.request.parse(buffer, bytes_read);
		memset(buffer, 0, sizeof(buffer));
		if (client.request.parsingCompleted() || client.request.errorCode())
		{
			client.buildResponse();
			removeFromSet(i, _recv_fds);
			addToSet(i, _write_fds);
		}
	}
}


void Multiplexer::buildTheResponse(Client &client)
{
	if (client.response.isCGI == true)
	{
		Location loc;
		client.response.handleCgi(loc);
		if (client.response._cgi_obj.know != 2)
			client.response._response = "";
		return;
	}
	if (client.isHeadSent == false)
	{
		client.flag = false;
		client.response._response = client.response.get_headers();
		client.content_len = client.response.fileSize;
		client.isHeadSent = true;
	}
	else
	{
		char buffer[BUFFER_SIZE];
		memset(buffer, 0, sizeof(buffer));
		if (client.isFileOpened == false)
		{
			client.file.close();
			client.file.open(client.response.getPath().c_str(), std::ios::in | std::ios::binary);
			client.isFileOpened = true;
		}
		client.file.readsome(buffer, BUFFER_SIZE);
		client.bytesRead = client.file.gcount();
		if (client.bytesRead < 0)
			std::cerr << "ERROR HERE : IN READ BYTES " << std::endl;
		client.response._response = std::string(buffer, client.bytesRead);
	}
}

void Multiplexer::sendResponse(const int &i, Client &client)
{
	std::string response;
	if (client._rem == false)
		buildTheResponse(client);
	if (client.response._cgi_obj.know == 1)
		return;
	ssize_t result = write(i, client.response._response.c_str(), client.response._response.size());
	if (client.flag == true)
		client.bytes_sent += result;
	if (result == -1 || client.response._cgi_obj.know == 2)
		closeConnection(i);
	else if (result != (ssize_t)client.response._response.size())
	{
		client._rem = true;
		client.response._response = client.response._response.substr(result, client.response._response.size());
	}
	else if (client.bytes_sent >= (ssize_t)client.content_len || client.response._response.empty() || result == 0)
		closeConnection(i);
	else
		client._rem = false; // send just a buffer size not all the response
}

void Multiplexer::acceptNewConnection(Server &serv)
{
	struct sockaddr_in client_address;
	long client_address_size = sizeof(client_address);
	int client_sock;
	Client new_client(serv);
	char buf[INET_ADDRSTRLEN];

	if ((client_sock = accept(serv.getFd(), (struct sockaddr *)&client_address,
							  (socklen_t *)&client_address_size)) == -1)
	{
		std::cerr << "Accept failed: Unable to accept the client connection." << std::endl;
		return;
	}
	std::cout << YELLOW_BOLD << "Assigned Socket " << client_sock << " to connection from " << inet_ntop(AF_INET, &client_address, buf, INET_ADDRSTRLEN) << RESET << std::endl;
	addToSet(client_sock, _recv_fds);
	if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
	{
		std::cerr << "Fcntl failed: Unable to set file descriptor options." << std::endl;
		removeFromSet(client_sock, _recv_fds);
		close(client_sock);
		return;
	}
	_clients_map[client_sock] = new_client;
}

void Multiplexer::runServers()
{
	fd_set _recv_temp;
	fd_set _write_temp;

	init_fds();

	while (true)
	{
		_recv_temp = _recv_fds;
		_write_temp = _write_fds;
		if (select(fdmax + 1, &_recv_temp, &_write_temp, NULL, NULL) < 0)
		{
			std::cerr << "Select failed: Unable to monitor file descriptor." << std::endl;
			continue;
		}
		for (int i = 2; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &_write_temp))
				sendResponse(i, _clients_map[i]);
			else if (FD_ISSET(i, &_recv_temp))
			{
				if (_servers_map.count(i))
					acceptNewConnection(_servers_map.find(i)->second);
				else
					readRequest(i, _clients_map[i]);
			}
		}
	}
}