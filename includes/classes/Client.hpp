#pragma once

#include "../main.hpp"

class Server;
class Request;
class Response;

class Client
{
    public:
        Client();
        Client(const Client &other);
        Client(Server &server);
		Client &operator=(const Client & other);
        ~Client();

        void                buildResponse();

        Response            response;
        Request         	request;
        Server        		server;
        size_t              bytes;
        std::ifstream       file;
        std::streamsize 	bytesRead;
        size_t      		content_len;
        ssize_t      		bytes_sent;
        bool        		isHeadSent;
        bool        		isFileOpened;
        bool        		flag;
        bool			    _rem;
        int                 _client_socket;
		std::string test;

    private:
        struct sockaddr_in  _client_address;
};
