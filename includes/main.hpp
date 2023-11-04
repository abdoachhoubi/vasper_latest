#pragma once

#include <netdb.h>
#include <exception>
# include <iostream>
# include <fcntl.h>
# include <cstring>
# include <string>
# include <unistd.h>
# include <dirent.h>
# include <sstream>
# include <cstdlib>
# include <fstream>
# include <sstream>
# include <cctype>
# include <climits>
# include <ctime>
# include <cstdarg>
#include <csignal>

/* STL Containers */
# include <map>
# include <set>
# include <vector>
# include <algorithm>
# include <iterator>
# include <list>

/* System */
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <unistd.h>
# include <signal.h>

# include <sys/socket.h>
# include <netinet/in.h>
# include <sys/select.h>
# include <arpa/inet.h>
# include <math.h>

#define RED_BOLD "\033[1;31m"
#define GREEN_BOLD "\033[1;32m"
#define YELLOW_BOLD "\033[1;33m"
#define BLUE_BOLD "\033[1;34m"
#define MAGENTA_BOLD "\033[1;35m"
#define ORANGE_BOLD "\033[1;36m"
#define WHITE_BOLD "\033[1;37m"
#define CYAN_BOLD "\033[1;38m"
#define RESET "\033[0m"

#define BUFFER_SIZE 65536 // 2^16
#define CONNECTION_TIMEOUT 60
#define MAX_CONNECTIONS 1000
#define MAX_CONTENT_LENGTH 1000000
#define MAX_URI_LENGTH 2048

#define TEMP_ROOT_PATH "/home/astro/Desktop/webserv_enhanced"


enum REQ_METHOD
{
    GET,
    POST,
    DELETE,
    NONE
};

#include "utils.hpp"

#include "classes/ConfParser.hpp"
#include "classes/Server.hpp"
#include "classes/Location.hpp"
#include "classes/Request.hpp"
#include "classes/Cgi.hpp"
#include "classes/Multiplexer.hpp"
#include "classes/Response.hpp"
#include "classes/Client.hpp"
