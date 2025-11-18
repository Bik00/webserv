#include "../../../includes/config/block/LocationBlock.hpp"

LocationBlock::LocationBlock(void)
{
	path = std::string();
	methods.clear();
	hasRedirect = false;
	redirectCode = 0;
	redirectTarget = std::string();
	// BaseBlock constructor handles root/indexFiles/autoindex/clientMaxBodySize
	cgiExtensions.clear();
	cgiPath = std::string();
	uploadEnable = false;
	uploadStore = std::string();
    
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
	// delegate base fields to BaseBlock
	BaseBlock::operator=(ref);
	cgiExtensions = ref.cgiExtensions;
	cgiPath = ref.cgiPath;
	uploadEnable = ref.uploadEnable;
	uploadStore = ref.uploadStore;
	return *this;
}


// path
void LocationBlock::setPath(const std::string &p) { path = p; }
const std::string &LocationBlock::getPath() const { return path; }

// methods
void LocationBlock::setMethods(const std::vector<std::string> &m) { methods = m; }
void LocationBlock::addMethod(const std::string &m) { methods.push_back(m); }
const std::vector<std::string> &LocationBlock::getMethods() const { return methods; }

// redirect
void LocationBlock::setRedirect(int code, const std::string &target) {
	hasRedirect = true; redirectCode = code; redirectTarget = target;
}
void LocationBlock::clearRedirect() { hasRedirect = false; redirectCode = 0; redirectTarget.clear(); }
bool LocationBlock::getHasRedirect() const { return hasRedirect; }
int LocationBlock::getRedirectCode() const { return redirectCode; }
const std::string &LocationBlock::getRedirectTarget() const { return redirectTarget; }

// CGI
void LocationBlock::setCgiPath(const std::string &path_) { cgiPath = path_; }
const std::string &LocationBlock::getCgiPath() const { return cgiPath; }
void LocationBlock::setCgiExtensions(const std::vector<std::string> &exts) { cgiExtensions = exts; }
void LocationBlock::addCgiExtension(const std::string &ext) { cgiExtensions.push_back(ext); }
const std::vector<std::string> &LocationBlock::getCgiExtensions() const { return cgiExtensions; }
bool LocationBlock::hasCgi() const { return !cgiPath.empty() || !cgiExtensions.empty(); }

// upload
void LocationBlock::enableUpload(bool on) { uploadEnable = on; }
bool LocationBlock::isUploadEnabled() const { return uploadEnable; }
void LocationBlock::setUploadStore(const std::string &dir) { uploadStore = dir; uploadEnable = !dir.empty(); }
const std::string &LocationBlock::getUploadStore() const { return uploadStore; }


