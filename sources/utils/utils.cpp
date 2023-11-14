#include "../../includes/main.hpp"

// TAG: Converts a hexadecimal number to a decimal number
char fromHexToDec(std::string hex)
{
	char decimal = 0;
	for (int i = 0; i < 2; i++)
	{
		if (hex[i] >= '0' && hex[i] <= '9')
			decimal += (hex[i] - '0') * pow(16, 1 - i);
		else if (hex[i] >= 'A' && hex[i] <= 'F')
			decimal += (hex[i] - 'A' + 10) * pow(16, 1 - i);
		else if (hex[i] >= 'a' && hex[i] <= 'f')
			decimal += (hex[i] - 'a' + 10) * pow(16, 1 - i);
		else
			return (0);
	}
	return (decimal);
}

// TAG: Split a string into a vector of strings
std::vector<std::string> ft_split(const std::string &s)
{
	std::vector<std::string> tokens;
	std::string token;
	std::stringstream ss(s);
	while (ss >> token)
		tokens.push_back(token);
	return tokens;
}

// TAG: Splits a string into a vector of strings (separated by a separator)
std::vector<std::string> splitParametrs(std::string line, std::string sep)
{
	std::vector<std::string> str;
	size_t start, end;
	start = end = 0;
	while (1)
	{
		end = line.find_first_of(sep, start);
		if (end == std::string::npos)
			break;
		std::string tmp = line.substr(start, end - start);
		str.push_back(tmp);
		start = line.find_first_not_of(sep, end);
		if (start == std::string::npos)
			break;
	}
	return (str);
}

// TAG: Generates all the sub-uris of a given uri
std::vector<std::string> generateSubUris(const std::string &input)
{
	std::vector<std::string> tokens;
	std::string token;
	std::stringstream ss(input);
	std::string cumulativePath;

	while (std::getline(ss, token, '/'))
	{
		cumulativePath += "/" + token;
		tokens.push_back(cumulativePath);
	}
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].size() > 1)
			tokens[i].erase(0, 1);
	}
	return tokens;
}

// TAG: Checks if a path is a directory
bool is_dir(const std::string &path)
{
	struct stat file_info;
	if (stat(path.c_str(), &file_info) != 0)
		return false;
	return S_ISDIR(file_info.st_mode);
}

// TAG: Trims white spaces from both ends of the string
std::string strtrim(const std::string &input)
{
	size_t start = 0;
	size_t end = input.length() - 1;

	// Find the position of the first non-white space character from the beginning
	while (start <= end && std::isspace(input[start]))
		start++;
	// Find the position of the first non-white space character from the end
	while (end >= start && std::isspace(input[end]))
		end--;
	return input.substr(start, end - start + 1);
}

// TAG: Checks if a file exists
bool file_exists(const std::string &path)
{
	struct stat file_info;
	if (stat(path.c_str(), &file_info) != 0)
		return false;
	return true;
}