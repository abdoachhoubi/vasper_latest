#pragma once

#include "../main.hpp"

class Location
{
	public:
		Location();
		Location(const Location &other);
		Location &operator=(const Location &other);
		~Location();

		void setPath(std::string parametr);
		const std::string &getPath() const;

		void setRootLocation(std::string parametr);
		const std::string &getRootLocation() const;

		void setMethods(std::vector<std::string> methods);
		const std::vector<bool> &getMethods() const;

		void setAutoindex(std::string parametr);
		const bool &getAutoindex() const;

		void setIndexLocation(std::string parametr);
		const std::string &getIndexLocation() const;

		void setReturn(std::string parametr);
		const std::string &getReturn() const;

		void setCgiPath(std::vector<std::string> path);
		const std::vector<std::string> &getCgiPath() const;

		void setCgiExtension(std::vector<std::string> extension);
		const std::vector<std::string> &getCgiExtension() const;

		const std::map<std::string, std::string> &getExtensionPath() const;



		void setAllowedMethods(std::vector<std::string> methods);
		std::vector<std::string> getAllowedMethods() const;

		void setCGI(std::string parametr);
		bool getCGI() const;

		std::map<std::string, std::string> _ext_path;

	private:
		std::string					path;
		std::string					root;
		bool						autoindex;
		std::string					index;
		std::vector<bool>			_methods;
		std::string					_return;
		std::string					_alias;
		std::vector<std::string>	cgi_path;
		std::vector<std::string>	allowed_methods;
		std::vector<std::string>	cgi_extension;
		unsigned long				clientMaxBodySize;
		bool						cgi;
};
