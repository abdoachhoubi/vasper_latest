#include "../../includes/main.hpp"

// TAG: Returns the CSS style for the autoindex
std::string autoindex_style()
{
	return "<style>\n"
		   "    * {\n"
		   "        padding: 0;\n"
		   "        margin: 0;\n"
		   "        box-sizing: border-box;\n"
		   "    }\n"
		   "    body {\n"
		   "        width: 100%;\n"
		   "        min-height: 100dvh;\n"
		   "        padding: 10%;\n"
		   "        font-family: 'Courier New', Courier, monospace;\n"
		   "        background-color: #000000;\n"
		   "        background-repeat: no-repeat;\n"
		   "        background-size: cover;\n"
		   "    }\n"
		   "    .table {\n"
		   "        width: 100%;\n"
		   "        display: grid;\n"
		   "        grid-template-columns: repeat(3, 1fr);\n"
		   "        grid-gap: 2rem;\n"
		   "    }\n"
		   "    h1 {\n"
		   "        font-size: 2rem;\n"
		   "        margin-bottom: 1rem;\n"
		   "		color: #fff;\n"
		   "    }\n"
		   "    .element\n"
		   "    {\n"
		   "        width: 100%;\n"
		   "        height: 100%;\n"
		   "        display: flex;\n"
		   "        flex-direction: column;\n"
		   "        justify-content: center;\n"
		   "        align-items: center;\n"
		   "        border-radius: 10px;\n"
		   "        border: 1px solid #fff;\n"
		   "        padding: 1rem;\n"
		   "    }\n"
		   "    .directory { /* Add this CSS rule for directories */\n"
		   "        background-color: #fff;\n"
		   "        border: 1px solid #fff;\n"
		   "    }\n"
		   "    .directory a {\n"
		   "        color: cornflowerblue;\n"
		   "    }\n"
		   "    a {\n"
		   "        text-decoration: none;\n"
		   "        color: #fff;\n"
		   "    }\n"
		   "</style>\n";
}

// TAG: Creates the HTML string for the autoindex
std::string create_autoindex_string(std::string dir, std::string dir_str, const std::vector<std::string> &files)
{
	std::string autoindex_string;
	autoindex_string += "<!DOCTYPE html>\n"
						"<html>\n"
						"<head>\n" +
						autoindex_style() + "<title>Index of /" + dir_str.substr(1) + "</title>\n"
																					  "</head>\n"
																					  "<body>\n"
																					  "<h1>Index of /" +
						dir_str.substr(1) + "</h1>\n"
											"<div class=\"table\">\n"
											"<span class=\"element directory\">\n"
											"<a class=\"link\" href=\"../\">../</a>\n"
											"</span>\n";
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		std::string path_to_file = dir + *it;
		bool is_directory = is_dir(path_to_file);
		autoindex_string += "<span class=\"element" + std::string(is_directory ? " directory" : "") + "\">\n";
		std::string file_path = dir + *it;
		if (file_exists(file_path))
			autoindex_string += "<a class=\"link\" href=\"" + std::string(dir_str[dir_str.length() - 1] == '/' ? dir_str : dir_str + "/") + *it + std::string(is_directory ? "/\">" : "\">") + *it + "</a>\n" + "</span>\n";
		else
			autoindex_string += "<a class=\"link\" href=\"" + *it + std::string(is_directory ? "/\">" : "\">") + *it + "</a>\n" + "</span>\n";
	}

	autoindex_string += "</div>\n"
						"</body>\n"
						"</html>\n";
	return autoindex_string;
}

// TAG: Returns the HTML body for the autoindex
std::string autoindex_body(char *path, std::string req_path)
{
	DIR *dir;
	std::string autoindex_body;
	struct dirent *ent;
	std::vector<std::string> files;

	if ((dir = opendir(path)) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))
				files.push_back(ent->d_name);
		}
		std::sort(files.begin(), files.end());
		autoindex_body = create_autoindex_string(path, req_path, files);
		closedir(dir);
		return autoindex_body;
	}
	else
	{
		std::cerr << "Could not open directory" << std::endl;
		return std::string(); // Return an empty string on error
	}
}
