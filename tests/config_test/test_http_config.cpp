#include "HttpConfig.hpp"
#include "ServerConfig.hpp"
// #include "UpstreamConfig.hpp"
#include "EventsConfig.hpp"
#include "test_utils.hpp"

// --- HttpConfig ---

// defaults
int test_http_defaults() {
    HttpConfig h;
    int r = 0;
    r += assert((size_t)0,       h.getKeepaliveTimeout(), "keepalive_timeout padrão 0",  LINE_DATA());
    // r += assert_false(h.getSendfile(), "sendfile padrão false");
    // r += assert(std::string(""), h.getInclude(),     "include padrão ''",      LINE_DATA());
    // r += assert(std::string(""), h.getDefaultType(), "default_type padrão ''", LINE_DATA());
    // r += assert(std::string(""), h.getLogFormat(),   "log_format padrão ''",   LINE_DATA());
    // r += assert(std::string(""), h.getAccessLog(),   "access_log padrão ''",   LINE_DATA());
    // r += assert(std::string(""), h.getExpires(),     "expires padrão ''",      LINE_DATA());
    // r += assert((size_t)0,       h.getServers().size(),   "servers vazio",     LINE_DATA());
    // r += assert((size_t)0,       h.getUpstreams().size(), "upstreams vazio",   LINE_DATA());
    return r;
}

// setters de string e scalar
int test_http_setters() {
    HttpConfig h;
    // h.setInclude("mime.types");
    h.setKeepaliveTimeout(75);
    // h.setSendfile(true);
    // h.setDefaultType("application/octet-stream");
    // h.setLogFormat("combined");
    // h.setAccessLog("/var/log/access.log");
    // h.setExpires("7d");
    int r = 0;
    // r += assert(std::string("mime.types"),              h.getInclude(),          "getInclude()",          LINE_DATA());
    r += assert((size_t)75,                             h.getKeepaliveTimeout(), "getKeepaliveTimeout()", LINE_DATA());
    // r += assert_true(h.getSendfile(), "getSendfile() true", LINE_DATA());
    // r += assert(std::string("application/octet-stream"), h.getDefaultType(),     "getDefaultType()",      LINE_DATA());
    // r += assert(std::string("combined"),                h.getLogFormat(),        "getLogFormat()",        LINE_DATA());
    // r += assert(std::string("/var/log/access.log"),     h.getAccessLog(),        "getAccessLog()",        LINE_DATA());
    // r += assert(std::string("7d"),                      h.getExpires(),          "getExpires()",          LINE_DATA());
    return r;
}

// addChild(ServerConfig) acumula na lista
int test_http_addchild_server() {
    HttpConfig h;
    ServerConfig* s1 = new ServerConfig();
    ServerConfig* s2 = new ServerConfig();
    h.addChild(s1);
    h.addChild(s2);
    return assert((size_t)2, h.getServers().size(), "addChild ServerConfig: acumula 2", LINE_DATA());
}

// addChild(UpstreamConfig) acumula na lista
// int test_http_addchild_upstream() {
//     HttpConfig h;
//     UpstreamConfig* u1 = new UpstreamConfig();
//     UpstreamConfig* u2 = new UpstreamConfig();
//     h.addChild(u1);
//     h.addChild(u2);
//     return assert((size_t)2, h.getUpstreams().size(), "addChild UpstreamConfig: acumula 2", LINE_DATA());
// }

// addChild com tipo inválido gera erro
int test_http_addchild_invalid() {
    HttpConfig h;
    EventsConfig* wrong = new EventsConfig();
    h.addChild(wrong);
    int r = assert_true(h.getErrors().size() > 0, "addChild tipo inválido gera erro", LINE_DATA());
    delete wrong;
    return r;
}

// addChild(NULL) gera erro
int test_http_addchild_null() {
    HttpConfig h;
    h.addChild(NULL);
    return assert_true(h.getErrors().size() > 0, "addChild NULL gera erro", LINE_DATA());
}

// getServers() retorna os mesmos ponteiros adicionados (identidade)
int test_http_getservers_identity() {
    HttpConfig h;
    ServerConfig* s1 = new ServerConfig();
    ServerConfig* s2 = new ServerConfig();
    h.addChild(s1);
    h.addChild(s2);
    const std::list<ServerConfig*>& servers = h.getServers();
    int r = 0;
    r += assert_true(servers.front() == s1, "primeiro servidor: identidade preservada", LINE_DATA());
    r += assert_true(servers.back()  == s2, "segundo servidor: identidade preservada",  LINE_DATA());
    return r;
}

// addChild aceita Server e Upstream misturados sem erro
// int test_http_addchild_mixed() {
//     HttpConfig h;
//     ServerConfig*   s = new ServerConfig();
//     UpstreamConfig* u = new UpstreamConfig();
//     h.addChild(s);
//     h.addChild(u);
//     int r = 0;
//     r += assert((size_t)1, h.getServers().size(),   "1 server",   LINE_DATA());
//     r += assert((size_t)1, h.getUpstreams().size(), "1 upstream", LINE_DATA());
//     r += assert_true(h.getErrors().size() == 0, "sem erros", LINE_DATA());
//     return r;
// }

// --- EventsConfig ---

// defaults
// int test_events_defaults() {
//     EventsConfig e;
//     int r = 0;
//     r += assert_false(e.getMultiAccept(), "multi_accept padrão false");
//     r += assert(IO_SELECT, e.getUse(),               "use padrão IO_SELECT",        LINE_DATA());
//     r += assert((size_t)0, e.getWorkerConnections(), "worker_connections padrão 0", LINE_DATA());
//     return r;
// }

// // setters
// int test_events_setters() {
//     EventsConfig e;
//     e.setMultiAccept(true);
//     e.setUse(IO_EPOLL);
//     e.setWorkerConnections(1024);
//     int r = 0;
//     r += assert_true(e.getMultiAccept(), "setMultiAccept(true)", LINE_DATA());
//     r += assert(IO_EPOLL, e.getUse(),               "setUse(IO_EPOLL)",          LINE_DATA());
//     r += assert((size_t)1024, e.getWorkerConnections(), "setWorkerConnections(1024)", LINE_DATA());
//     return r;
// }

// todos os valores de IOMultiplexer
int test_events_io_multiplexer_values() {
    EventsConfig e;
    int r = 0;
    e.setUse(IO_SELECT); r += assert(IO_SELECT, e.getUse(), "IO_SELECT", LINE_DATA());
    e.setUse(IO_POLL);   r += assert(IO_POLL,   e.getUse(), "IO_POLL",   LINE_DATA());
    e.setUse(IO_EPOLL);  r += assert(IO_EPOLL,  e.getUse(), "IO_EPOLL",  LINE_DATA());
    e.setUse(IO_KQUEUE); r += assert(IO_KQUEUE, e.getUse(), "IO_KQUEUE", LINE_DATA());
    return r;
}

// --- UpstreamConfig ---

// lista vazia por padrão
// int test_upstream_default_empty() {
//     UpstreamConfig u;
//     return assert((size_t)0, u.getServers().size(), "upstream: lista vazia por padrão", LINE_DATA());
// }

// // addServer acumula
// int test_upstream_add_server() {
//     UpstreamConfig u;
//     UpstreamServer s1;
//     s1._ip = "10.0.0.1";
//     s1._weight = 5;
//     UpstreamServer s2;
//     s2._ip = "10.0.0.2";
//     s2._weight = 3;
//     u.addServer(s1);
//     u.addServer(s2);
//     return assert((size_t)2, u.getServers().size(), "upstream: addServer acumula 2", LINE_DATA());
// }

// // UpstreamServer defaults
// int test_upstream_server_defaults() {
//     UpstreamServer s;
//     int r = 0;
//     r += assert(std::string(""), s._ip,     "UpstreamServer: ip padrão ''",    LINE_DATA());
//     r += assert((size_t)0,       s._weight, "UpstreamServer: weight padrão 0", LINE_DATA());
//     r += assert_false(s._flag, "UpstreamServer: flag padrão false");
//     return r;
// }

// // ordem de inserção preservada em UpstreamConfig
// int test_upstream_insertion_order() {
//     UpstreamConfig u;
//     UpstreamServer s1; s1._ip = "10.0.0.1";
//     UpstreamServer s2; s2._ip = "10.0.0.2";
//     UpstreamServer s3; s3._ip = "10.0.0.3";
//     u.addServer(s1);
//     u.addServer(s2);
//     u.addServer(s3);
//     const std::list<UpstreamServer>& servers = u.getServers();
//     std::list<UpstreamServer>::const_iterator it = servers.begin();
//     int r = 0;
//     r += assert(std::string("10.0.0.1"), it->_ip, "primeiro inserido é front()", LINE_DATA()); ++it;
//     r += assert(std::string("10.0.0.2"), it->_ip, "segundo inserido é middle",   LINE_DATA()); ++it;
//     r += assert(std::string("10.0.0.3"), it->_ip, "terceiro inserido é back()",  LINE_DATA());
//     return r;
// }

// // valores de um UpstreamServer são preservados após addServer
// int test_upstream_server_values_preserved() {
//     UpstreamConfig u;
//     UpstreamServer s;
//     s._ip     = "192.168.1.10:8080";
//     s._weight = 10;
//     s._flag   = true;
//     u.addServer(s);
//     const UpstreamServer& stored = u.getServers().front();
//     int r = 0;
//     r += assert(std::string("192.168.1.10:8080"), stored._ip,     "ip preservado",     LINE_DATA());
//     r += assert((size_t)10,                       stored._weight, "weight preservado",  LINE_DATA());
//     r += assert_true(stored._flag, "flag preservada", LINE_DATA());
//     return r;
// }

int main(int argc, char **argv) {
    messages::settup(argc, argv);
    int failures = 0;

    failures += test_http_defaults();
    failures += test_http_setters();
    failures += test_http_addchild_server();
    // failures += test_http_addchild_upstream();
    failures += test_http_addchild_invalid();
    failures += test_http_addchild_null();
    failures += test_http_getservers_identity();
    // failures += test_http_addchild_mixed();
    // failures += test_events_defaults();
    // failures += test_events_setters();
    failures += test_events_io_multiplexer_values();
    // failures += test_upstream_default_empty();
    // failures += test_upstream_add_server();
    // failures += test_upstream_server_defaults();
    // failures += test_upstream_insertion_order();
    // failures += test_upstream_server_values_preserved();

    messages::print();
    return failures;
}
