#include "../../../includes/config/block/LocationBlock.hpp"

LocationBlock::LocationBlock(void)
{
	path = std::string();
	methods.clear();
	hasRedirect = false;
	redirectCode = 0;
	redirectTarget = std::string();
	root = std::string();
	autoindex = false;
	indexFiles.clear();
	cgiExtensions.clear();
	cgiPath = std::string();
	uploadEnable = false;
	uploadStore = std::string();
	clientMaxBodySize = 0;
}

LocationBlock::~LocationBlock(void)
{
}

LocationBlock::LocationBlock(const LocationBlock &ref)
{
	*this = ref;
}

LocationBlock &LocationBlock::operator=(const LocationBlock &ref)
{
	if (this == &ref)
		return *this;
	path = ref.path;
	methods = ref.methods;
	hasRedirect = ref.hasRedirect;
	redirectCode = ref.redirectCode;
	redirectTarget = ref.redirectTarget;
	root = ref.root;
	autoindex = ref.autoindex;
	indexFiles = ref.indexFiles;
	cgiExtensions = ref.cgiExtensions;
	cgiPath = ref.cgiPath;
	uploadEnable = ref.uploadEnable;
	uploadStore = ref.uploadStore;
	clientMaxBodySize = ref.clientMaxBodySize;
	return *this;
}


