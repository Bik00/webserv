#ifndef LOCATION_BLOCK_HPP
# define LOCATION_BLOCK_HPP

# include "../../libs/Libs.hpp"
# include "./BaseBlock.hpp"

class LocationBlock : public BaseBlock
{
private:
    std::string                 path; // 이 location의 경로 패턴 (예: /images/)
    std::vector<std::string>    methods; // 허용된 HTTP 메서드 목록 (예: GET, POST)
    bool                        hasRedirect; // 리디렉션 설정 여부 플래그
    int                         redirectCode; // 리디렉션 상태 코드 (예: 301, 302)
    std::string                 redirectTarget; // 리디렉션 대상 URL
    // root/indexFiles/autoindex/clientMaxBodySize provided by BaseBlock
    std::vector<std::string>    cgiExtensions; // CGI 처리가 필요한 파일 확장자 목록
    std::string                 cgiPath; // CGI 실행 파일(또는 핸들러)의 경로
    bool                        uploadEnable; // 업로드 기능 허용 여부
    std::string                 uploadStore; // 업로드된 파일 저장 경로
    
public:
    LocationBlock(void);
    ~LocationBlock(void);
    LocationBlock(const LocationBlock &ref);
    LocationBlock &operator=(const LocationBlock &ref);

    // path
    void setPath(const std::string &p);
    const std::string &getPath() const;

    // methods (location-level; may be superseded by BaseBlock::allowedMethods usage)
    void setMethods(const std::vector<std::string> &m);
    void addMethod(const std::string &m);
    const std::vector<std::string> &getMethods() const;

    // redirect
    void setRedirect(int code, const std::string &target);
    void clearRedirect();
    bool getHasRedirect() const;
    int getRedirectCode() const;
    const std::string &getRedirectTarget() const;

    // CGI
    void setCgiPath(const std::string &path);
    const std::string &getCgiPath() const;
    void setCgiExtensions(const std::vector<std::string> &exts);
    void addCgiExtension(const std::string &ext);
    const std::vector<std::string> &getCgiExtensions() const;
    bool hasCgi() const;

    // upload
    void enableUpload(bool on);
    bool isUploadEnabled() const;
    void setUploadStore(const std::string &dir);
    const std::string &getUploadStore() const;
};

#endif /* LOCATION_BLOCK_HPP */