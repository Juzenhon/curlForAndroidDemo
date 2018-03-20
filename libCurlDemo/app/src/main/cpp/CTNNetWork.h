//
// Created by juzenhon on 2017/8/28.
//

#ifndef TMTP_CTNHTTPCLIENT_H
#define TMTP_CTNHTTPCLIENT_H

#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;


enum HttpMethod {
    Get,
    Post,
};

class HttpResponse {

public:

    string _url;
    string _serverIp;

    map<string, string> _headers;

    string _errorStr;

    double _dns_time;

    double _connect_time;

    double _ssl_connect_time;

    double _transfer_time;

    double _http_version;

    int _httpCode;
};

class HttpRequest {

    friend class CTNNetWork;
public:

    HttpRequest();

    virtual ~HttpRequest();

    map<string, string> &getHeaders();

    HttpMethod getMethod();

    string &getUrl();

    void setUrl(const char *url);

    void cancel();

    /**
     * 入队开始执行
     */
    void enqueue();

    /**
     * 获取请求body中的数据
     * @param size 数据长度
     * @return  bufPtr body数据
     */
    virtual const char *getPostBody(int64_t &size);

    /**
     * 数据写到本地
     * @param dataPtr
     * @param len
     * @return 实际处理的长度
     */
    virtual int32_t writeTo(const char* dataPtr,int32_t len);

protected:

    /**
   * 处理response
   * @param code 响应代码
   */
    virtual void handResponse(HttpResponse& response) = 0;
protected:

    map<string, string> _headers;

    HttpMethod _method = Get;

    string _url;

    string _result;

    bool _cancel;

    HttpResponse _response;

private:
    bool _running;
    void *_userP1;
    void *_requestHandle;

};

class CTNNetWork {

public:

    static CTNNetWork *getClient() {
        static CTNNetWork sInstance;
        return &sInstance;
    }

    ~CTNNetWork();

public:

    void setTimeout(int seconds);

    void setCerPath(const char* cerPath);

private:

    friend class HttpRequest;

    void enqueue(HttpRequest *request);

    void *initReqForHandle(HttpRequest *request);

    void loopRequests();

    /**
     * 把队列中的请求加入client执行
     * @param multi_handle
     * @return
     */
    int joinRequestForExec(void* multi_handle);

    void readResponseInfo(void* cm_handle);

    void cancelRequest(HttpRequest * request);

private:
    CTNNetWork();

    CTNNetWork(const CTNNetWork &rhs);

    const CTNNetWork operator=(const CTNNetWork &rhs);

    int _timeout;

    std::string _cerPath;

    queue<HttpRequest *> _requestQueue;

    mutex _queueMutex;

    mutex _conditionMutex;

    std::condition_variable _condition;

    bool _net_running;

    int _exec_count;

    void* _multi_handle;

    void* _shareHandle;

    const static int MAX_EXEC_COUNT = 4;
};


#endif //TMTP_CTNHTTPCLIENT_H
