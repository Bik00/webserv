#ifndef LOCATION_CONFIG_HPP
# define LOCATION_CONFIG_HPP

# include "../libs/Libs.hpp"

class LocationConfig
{
public:
	std::string path;              // location path e.g. "/"
	std::string root;              // filesystem root for this location
	std::vector<std::string> try_files; // try_files list
	std::vector<std::string> methods;   // allowed methods
	bool        autoindex;         // directory listing
	std::string cgi_extension;     // e.g. ".php"
	std::string cgi_path;          // full path to CGI executable
	bool        upload_enabled;
	std::string upload_path;

	LocationConfig(void);
	~LocationConfig(void);
	LocationConfig(const LocationConfig &ref);
	LocationConfig &operator=(const LocationConfig &ref);
};

#endif /* LOCATION_CONFIG_HPP */
