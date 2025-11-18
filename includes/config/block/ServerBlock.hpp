#ifndef SERVER_BLOCK_HPP
# define SERVER_BLOCK_HPP

# include "../../libs/Libs.hpp"
# include "./LocationBlock.hpp"
# include "../../struct/ListenAddr.hpp"
# include "./BaseBlock.hpp"

class ServerBlock : public BaseBlock
{
private:
    std::vector<LocationBlock>  locationBlocks;     // 서버 내의 location 블록들
    std::vector<ListenAddr>    listenAddrs; // 여러 listen 주소 저장
    std::vector<std::string>    serverNames; // 이 서버가 처리할 도메인 이름들
    

public:
    ServerBlock(void);
    ~ServerBlock(void);
    ServerBlock(const ServerBlock &ref);
    ServerBlock &operator=(const ServerBlock &ref);

    void addLocationBlock(const LocationBlock &lb);
    void addServerName(const std::string &name);
    // BaseBlock provides addIndexFile/addErrorPage/setRoot/setIndexFiles/setClientMaxBodySize/setAutoindex

    // listen management
    void addListen(const std::string &host, int port, bool def=false);
    const std::vector<ListenAddr> &getListenAddrs() const;
    void clearListenAddrs();
    void ensureDefaultListen();
    void setDefaultServer(bool def);
    // use BaseBlock::setRoot/setIndexFiles/setClientMaxBodySize/setAutoindex
};

#endif /* SERVER_BLOCK_HPP */