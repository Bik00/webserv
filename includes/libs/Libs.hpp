#ifndef LIBS_HPP
# define LIBS_HPP

/* ************************************************************************** */
/* ----------------------------- Library FILES ------------------------------ */
/* ************************************************************************** */

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <set>
# include <fstream>
# include <cstdlib>
# include <cctype>
# include <algorithm>
# include <sstream>
# include <stdexcept>

# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/epoll.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/wait.h>
# include <signal.h>
# include <set>
# include <cstring>

# include "../struct/ListenAddr.hpp"

/* ************************************************************************** */
/* ----------------------------- CONSTRUCTOR -------------------------------- */
/* ************************************************************************** */

# define DEFAULT_CONFIG_PATH "./config/default.conf"

# define DEFAULT_WORKER_PROCESSES 1
# define DEFAULT_WORKER_CONNECTIONS 1024
# define DEFAULT_LISTEN_HOST "0.0.0.0"
# define DEFAULT_LISTEN_PORT 80
# define DEFAULT_ROOT "./www"
# define DEFAULT_INDEX_FILE "index.html"
# define DEFAULT_CLIENT_MAX_BODY_SIZE (1 * 1024 * 1024) // 1MB
# define DEFAULT_ALLOWED_METHODS std::vector<std::string>({"GET", "POST", "DELETE"})

// 기본 에러 페이지 경로 (프로젝트의 ./www/errors/*.html 사용)
// 예: ./www/errors/404.html, ./www/errors/500.html 등
# define DEFAULT_ERROR_PAGES_DIR "./www/errors"

# define MAX_CLIENT_MAX_BODY_SIZE (10 * 1024 * 1024) // 10MB

#endif /* LIBS_HPP */