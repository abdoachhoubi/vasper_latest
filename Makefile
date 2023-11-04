MAIN_SRCS = ${addprefix sources/, main.cpp}

CLASSES_SRCS = ${addprefix sources/classes/, Cgi.cpp Client.cpp ConfParser.cpp \
				Location.cpp Multiplexer.cpp Request.cpp Response.cpp Server.cpp}

UTILS_SRCS = ${addprefix sources/utils/, autoindex.cpp utils.cpp}

CC = c++

CFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address

NAME = webserv

all: ${NAME}

${NAME}: ${MAIN_SRCS} ${CLASSES_SRCS} ${UTILS_SRCS}
	# ./update_conf.sh
	${CC} ${CFLAGS} ${MAIN_SRCS} ${CLASSES_SRCS} ${UTILS_SRCS} -o ${NAME}

clean:
	rm -rf ${NAME}

fclean: clean

re: fclean all
