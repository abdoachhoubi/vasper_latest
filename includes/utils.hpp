#pragma once

#include "main.hpp"

template <typename T>
std::string to_string(const T &value)
{
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

std::string autoindex_style();
std::string create_autoindex_string(std::string dir, std::string dir_str, const std::vector<std::string> &files);
std::string autoindex_body(char *path, std::string req_path);

char fromHexToDec(std::string hex);
std::vector<std::string> ft_split(const std::string &s);
std::vector<std::string> splitParametrs(std::string line, std::string sep);
std::vector<std::string> generateSubUris(const std::string &input);
std::string strtrim(const std::string &input);
bool is_dir(const std::string &path);
bool file_exists(const std::string &path);