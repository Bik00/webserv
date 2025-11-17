#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include "../../libs/Libs.hpp"
# include "./LocationBlock.hpp"

class ServerBlock
{
private:
    std::vector<LocationBlock>  locationBlocks; // 서버 내의 location 블록들

    std::string                 listenHost; // 서버가 바인드할 호스트(예: 0.0.0.0)
    int                         listenPort; // 서버가 바인드할 포트
    bool                        defaultServer; // 기본 서버로 동작할지 여부
    std::vector<std::string>    serverNames; // 이 서버가 처리할 도메인 이름들
    std::string                 root; // 기본 문서 루트 경로
    std::vector<std::string>    indexFiles; // 디렉터리 인덱스 파일 리스트
    std::map<int, std::string>  errorPages; // 에러 코드 -> 에러 페이지 경로 매핑
    size_t                      clientMaxBodySize; // 허용되는 최대 요청 본문 크기(bytes)
    bool                        autoindex; // 디렉터리 자동 인덱싱 허용 여부
public:
    ServerBlock(void);
    ~ServerBlock(void);
    ServerBlock(const ServerBlock &ref);
    ServerBlock &operator=(const ServerBlock &ref);

    void addLocationBlock(const LocationBlock &lb);
    void addServerName(const std::string &name);
    void addIndexFile(const std::string &f);
    void addErrorPage(int code, const std::string &path);

    void setListenHost(const std::string &host);
    void setListenPort(int port);
    void setDefaultServer(bool def);
    void setRoot(const std::string &r);
    void setIndexFiles(const std::vector<std::string> &files);
    void setClientMaxBodySize(size_t size);
    void setAutoindex(bool on);
};

#endif /* SERVER_BLOCK_HPP */