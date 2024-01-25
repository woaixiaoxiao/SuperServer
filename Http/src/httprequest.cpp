#include "Http/httprequest.hpp"
#include <sstream>
using namespace std;

const unordered_set<string> HttpRequest::DEFAULT_HTML{
    "/index", "/register", "/login", "/welcome", "/video", "/picture",
};

const unordered_map<string, int> HttpRequest::DEFAULT_HTML_TAG{
    {"/register.html", 0},
    {"/login.html", 1},
};

void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HttpRequest::IsKeepAlive() const {
    if (header_.count("Connection") == 1) {
        return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}

// 解析一整个http请求
bool HttpRequest::parse(Buffer &buff) {
    const char CRLF[] = "\r\n";
    if (buff.ReadableBytes() <= 0) {
        return false;
    }
    while (buff.ReadableBytes() && state_ != FINISH) {
        const char *lineEnd = search(buff.Peek(), buff.BeginWriteConst(), CRLF, CRLF + 2);
        std::string line(buff.Peek(), lineEnd);
        switch (state_) {
            case REQUEST_LINE:
                if (!ParseRequestLine_(line)) {
                    return false;
                }
                ParsePath_();
                break;
            case HEADERS:
                ParseHeader_(line);
                if (buff.ReadableBytes() <= 2) {
                    state_ = FINISH;
                }
                break;
            case BODY:
                ParseBody_(line);
                break;
            default:
                break;
        }
        if (lineEnd == buff.BeginWrite()) {
            break;
        }
        buff.RetrieveUntil(lineEnd + 2);
    }
    LOG_DEBUG("[%s], [%s], [%s]", method_.c_str(), path_.c_str(), version_.c_str());
    return true;
}

// 解析请求的路径，设置主页面为index.html，其他页面则加上.html，比如请求路径为/log，则转为/log.html
void HttpRequest::ParsePath_() {
    if (path_ == "/") {
        path_ = "/index.html";
    } else {
        for (auto &item : DEFAULT_HTML) {
            if (item == path_) {
                path_ += ".html";
                break;
            }
        }
    }
}

// 解析请求行
bool HttpRequest::ParseRequestLine_(const string &line) {
    regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    smatch subMatch;
    if (regex_match(line, subMatch, patten)) {
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    LOG_ERROR("RequestLine Error");
    return false;
}

// 解析请求头
void HttpRequest::ParseHeader_(const string &line) {
    regex patten("^([^:]*): ?(.*)$");
    smatch subMatch;
    if (regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
    } else {
        // 请求头部分全部解析完了，即当前line为\n\0
        state_ = BODY;
    }
}

void HttpRequest::ParseKv() {
    stringstream ss;
    ss << body_;
    std::string temp;
    while (ss >> temp) {
        kvOp.push_back(temp);
    }
    if (kvOp[0] == "set") {
        kv_req->set(kvOp[1], kvOp[2]);
        value = "OK";
    } else if (kvOp[0] == "del") {
        kv_req->del(kvOp[1]);
        value = "OK";
    } else if (kvOp[0] == "get") {
        value = kv_req->get(kvOp[1]);
    }
    kvOp.clear();
}

// 解析请求内容
void HttpRequest::ParseBody_(const string &line) {
    body_ = line;
    // cout << "body: " << body_ << endl;
    ParsePost_();
    ParseKv();
    state_ = FINISH;
    LOG_DEBUG("Body:%s, len:%d", line.c_str(), line.size());
}

// 字符->十六进制
int HttpRequest::ConverHex(char ch) {
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    return ch;
}

// 解析具体的请求部分，这里就是一个登录和注册的逻辑
void HttpRequest::ParsePost_() {
    if (method_ == "POST" && header_["Content-Type"] == "application/x-www-form-urlencoded") {

        ParseFromUrlencoded_();
        if (DEFAULT_HTML_TAG.count(path_)) {
            int tag = DEFAULT_HTML_TAG.find(path_)->second;
            LOG_DEBUG("Tag:%d", tag);
            if (tag == 0 || tag == 1) {
                bool isLogin = (tag == 1);
                if (UserVerify(post_["username"], post_["password"], isLogin)) {
                    path_ = "/welcome.html";
                } else {
                    path_ = "/error.html";
                }
            }
        }
    }
}
/*
等号("=")：用于分隔键值对。当解析到等号时，将之前的字符作为键，之后的字符作为值。

加号("+)"：在URL编码中，空格会被编码为加号("+")。因此，在解析过程中，遇到加号时，将其替换为空格，以便正确还原原始数据。

百分号("%")：在URL编码中，特殊字符会以百分号开头，后面跟着两个十六进制数字表示字符的ASCII码。这个函数中使用百分号进行特殊字符的解码。例如，"%20"表示空格，"%3D"表示等号("=")。

与号("&")：用于分隔多个键值对。当解析到与号时，表示当前键值对的解析结束，可以将之前的键和值存储，并开始解析下一个键值对。
*/
// 解析URL请求
void HttpRequest::ParseFromUrlencoded_() {
    if (body_.size() == 0) {
        return;
    }

    string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;

    for (; i < n; i++) {
        char ch = body_[i];
        switch (ch) {
            case '=':
                key = body_.substr(j, i - j);
                j = i + 1;
                break;
            case '+':
                body_[i] = ' ';
                break;
            case '%':
                num = ConverHex(body_[i + 1]) * 16 + ConverHex(body_[i + 2]);
                body_[i + 2] = num % 10 + '0';
                body_[i + 1] = num / 10 + '0';
                i += 2;
                break;
            case '&':
                value = body_.substr(j, i - j);
                j = i + 1;
                post_[key] = value;
                LOG_DEBUG("%s = %s", key.c_str(), value.c_str());
                break;
            default:
                break;
        }
    }
    assert(j <= i);
    if (post_.count(key) == 0 && j < i) {
        value = body_.substr(j, i - j);
        post_[key] = value;
    }
}

// 根据用户名和密码判断是否有这个用户，如果有则验证，没有则注册
bool HttpRequest::UserVerify(const string &name, const string &pwd, bool isLogin) {
    if (name == "" || pwd == "") {
        return false;
    }
    LOG_INFO("Verify name:%s pwd:%s", name.c_str(), pwd.c_str());
    MYSQL *sql;
    SqlConnRAII(&sql, SqlConnPool::Instance());
    assert(sql);

    bool flag = false;
    unsigned int j = 0;
    char order[256] = {0};
    MYSQL_FIELD *fields = nullptr;
    MYSQL_RES *res = nullptr;
    // 若不是登录，则flag为true，代表注册
    if (!isLogin) {
        flag = true;
    }
    // 根据用户名去数据库查询
    snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1",
             name.c_str());
    LOG_DEBUG("%s", order);

    if (mysql_query(sql, order)) {
        mysql_free_result(res);
        return false;
    }
    res = mysql_store_result(sql);
    j = mysql_num_fields(res);
    fields = mysql_fetch_fields(res);
    // 根据查询结果来判断
    while (MYSQL_ROW row = mysql_fetch_row(res)) {
        LOG_DEBUG("MYSQL ROW: %s %s", row[0], row[1]);
        string password(row[1]);
        // 如果正在登录状态
        if (isLogin) {
            // 如果密码正确，则flag为true，代表通过
            if (pwd == password) {
                flag = true;
            }
            // 密码错误
            else {
                flag = false;
                LOG_DEBUG("pwd error!");
            }
        }
        // 不是登录状态，代表当前请求时注册，但是这个用户名之前已经有了，代表用户名重复
        else {
            flag = false;
            LOG_DEBUG("user used!");
        }
    }
    mysql_free_result(res);

    // 代表注册
    if (!isLogin && flag == true) {
        LOG_DEBUG("regirster!");
        bzero(order, 256);
        snprintf(order, 256, "INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(),
                 pwd.c_str());
        LOG_DEBUG("%s", order);
        if (mysql_query(sql, order)) {
            LOG_DEBUG("Insert error!");
            flag = false;
        }
        flag = true;
    }
    // 这个连接池用的很离谱，先用了RALL，但是马上就销毁，最后又手工free？
    SqlConnPool::Instance()->FreeConn(sql);
    LOG_DEBUG("UserVerify success!!");
    return flag;
}

std::string HttpRequest::path() const { return path_; }

std::string &HttpRequest::path() { return path_; }
std::string HttpRequest::method() const { return method_; }

std::string HttpRequest::version() const { return version_; }

// 根据key得到请求头的value
std::string HttpRequest::GetPost(const std::string &key) const {
    assert(key != "");
    if (post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}

std::string HttpRequest::GetPost(const char *key) const {
    assert(key != nullptr);
    if (post_.count(key) == 1) {
        return post_.find(key)->second;
    }
    return "";
}