#include "../../includes/main.hpp"

Client::Client() {}
Client::~Client() {}
Client::Client(const Client &other) { *this = other; }
Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		this->_client_socket = other._client_socket;
		this->_client_address = other._client_address;
		this->request = other.request;
		this->response = other.response;
		this->server = other.server;
		this->isHeadSent = other.isHeadSent;
		this->isFileOpened = other.isFileOpened;
		this->_rem = other._rem;
		this->bytesRead = other.bytesRead;
		this->content_len = other.content_len;
		this->bytes_sent = other.bytes_sent;
		this->flag = other.flag;
	}
	return (*this);
}

Client::Client(Server &server)
{
	isHeadSent = false;
	isFileOpened = false;
	_rem = false;
	bytesRead = 0;
	content_len = 0;
	bytes_sent = 0;
	flag = false;
	response.setServer(server);
	request.setMaxBodySize(server.getClientMaxBodySize());
}

void Client::buildResponse()
{
	response.setRequest(this->request);
	response.respond();
}