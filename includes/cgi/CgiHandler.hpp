#ifndef CGI_HANDLER_HPP
# define CGI_HANDLER_HPP

# include "../libs/Libs.hpp"
# include "../http/HttpRequest.hpp"
# include "../http/HttpResponse.hpp"
# include "../config/block/LocationBlock.hpp"
# include "../config/block/ServerBlock.hpp"

class CgiHandler
{
private:
    const HttpRequest &request;
    const ServerBlock &serverBlock;
    const LocationBlock *locationBlock;
    
    std::string scriptPath;       // CGI 스크립트 파일 경로
    std::string cgiExecutable;    // CGI 인터프리터 경로 (python, php 등)
    std::map<std::string, std::string> envVars;  // CGI 환경 변수
    
    // CGI 실행 관련
    int pipeFd[2];                // 파이프 (CGI 출력 읽기용)
    pid_t childPid;               // CGI 프로세스 PID
    std::string cgiOutput;        // CGI 출력 데이터
    
    // Private helpers
    void setupEnvironment(void);
    void setEnvVar(const std::string &name, const std::string &value);
    bool validateScriptPath(void);
    std::string getFileExtension(const std::string &path) const;
    
public:
    CgiHandler(const HttpRequest &req, const ServerBlock &server, const LocationBlock *location);
    ~CgiHandler(void);
    CgiHandler(const CgiHandler &ref);
    CgiHandler &operator=(const CgiHandler &ref);
    
    // CGI 실행
    bool execute(void);
    bool isCgiRequest(void) const;
    
    // 결과 가져오기
    const std::string &getOutput(void) const;
    void buildResponse(HttpResponse &response);
    
    // 상태
    bool isComplete(void) const;
    int getExitStatus(void) const;
};

#endif /* CGI_HANDLER_HPP */
