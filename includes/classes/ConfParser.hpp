#pragma once

#include "../main.hpp"

class Server;

class ConfParser {

	private:
		std::vector<Server>			servers;
		std::vector<std::string>	serv_conf;
		size_t						nbr_serv;
	
	public:
		ConfParser();
		~ConfParser();

		std::vector<Server>		getServers();
		void					parse(const std::string &config_file);
		void					splitServers(std::string &content);
		size_t					startServer(size_t start, std::string &content);
		size_t					endServer(size_t start, std::string &content);
		void					createServer(std::string &config, Server &server);

		static int getTypePath(std::string const path);
};
