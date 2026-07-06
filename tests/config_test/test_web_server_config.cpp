#include "WebServerConfig.hpp"
#include "GlobalConfig.hpp"
#include "HttpConfig.hpp"
#include "ServerConfig.hpp"
#include "EventsConfig.hpp"
#include "ConfigServerListen.hpp"
#include "test_utils.hpp"

static ConfigServerListen makeListen(size_t port) {
    ConfigServerListen l;
    l.port = port;
    l.address = "0.0.0.0";
    return l;
}

// Monta WebServerConfig → GlobalConfig → HttpConfig → ServerConfig
// O caller é responsável por criar os objetos; addChild transfere ownership.
static WebServerConfig* buildConfig(ServerConfig** out_servers, size_t count) {
    WebServerConfig* wsc = new WebServerConfig();
    GlobalConfig* global = new GlobalConfig();
    HttpConfig* http = new HttpConfig();

    for (size_t i = 0; i < count; ++i)
        http->addChild(out_servers[i]);

    global->addChild(http);
    wsc->addChild(global);
    return wsc;
}

// match_server: match exato por server_name
int test_match_by_server_name() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addServerName("api.com");

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    const ServerConfig* result = wsc->match_server(80, "api.com");
    int r = assert_true(result == s1, "match por server_name", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: fallback para primeiro servidor na porta (sem server_name)
int test_fallback_first_on_port() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    const ServerConfig* result = wsc->match_server(80, "anything.com");
    int r = assert_true(result == s1, "fallback: primeiro servidor na porta", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: sem match na porta retorna NULL
int test_no_match_wrong_port() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addServerName("api.com");

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    const ServerConfig* result = wsc->match_server(443, "api.com");
    int r = assert_true(result == NULL, "porta errada retorna NULL", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: host header com ":port" é ignorado na comparação
int test_host_header_strips_port() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addServerName("api.com");

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    const ServerConfig* result = wsc->match_server(80, "api.com:80");
    int r = assert_true(result == s1, "host header com :port é ignorado", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: dois servidores na mesma porta, retorna o com server_name correto
int test_two_servers_same_port() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addServerName("first.com");

    ServerConfig* s2 = new ServerConfig();
    s2->addListen(makeListen(80));
    s2->addServerName("second.com");

    ServerConfig* servers[] = { s1, s2 };
    WebServerConfig* wsc = buildConfig(servers, 2);

    const ServerConfig* result = wsc->match_server(80, "second.com");
    int r = assert_true(result == s2, "dois servidores: retorna o com server_name correto", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: fallback é o primeiro da porta quando nenhum server_name bate
int test_two_servers_fallback_is_first() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addServerName("first.com");

    ServerConfig* s2 = new ServerConfig();
    s2->addListen(makeListen(80));
    s2->addServerName("second.com");

    ServerConfig* servers[] = { s1, s2 };
    WebServerConfig* wsc = buildConfig(servers, 2);

    const ServerConfig* result = wsc->match_server(80, "unknown.com");
    int r = assert_true(result == s1, "fallback: primeiro servidor na porta quando nenhum server_name bate", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: servidor em porta diferente não é fallback
int test_server_different_port_not_fallback() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(8080));
    s1->addServerName("api.com");

    ServerConfig* s2 = new ServerConfig();
    s2->addListen(makeListen(80));
    s2->addServerName("web.com");

    ServerConfig* servers[] = { s1, s2 };
    WebServerConfig* wsc = buildConfig(servers, 2);

    const ServerConfig* result = wsc->match_server(80, "unknown.com");
    int r = assert_true(result == s2, "servidor em porta diferente não é fallback", LINE_DATA());
    delete wsc;
    return r;
}

// getServers: sem global retorna lista vazia
int test_get_servers_no_global() {
    WebServerConfig wsc;
    return assert((size_t)0, wsc.getServers().size(), "getServers sem global: lista vazia", LINE_DATA());
}

// getWorkerConnections: sem events retorna 1024
int test_worker_connections_default() {
    WebServerConfig wsc;
    return assert((size_t)1024, wsc.getWorkerConnections(), "getWorkerConnections padrão 1024", LINE_DATA());
}

// addChild com tipo inválido gera erro
int test_webserver_addchild_invalid() {
    WebServerConfig wsc;
    HttpConfig* wrong = new HttpConfig();
    wsc.addChild(wrong);
    int r = assert_true(wsc.getErrors().size() > 0, "addChild tipo inválido gera erro", LINE_DATA());
    delete wrong;
    return r;
}

// match_server: servidor sem listen não é selecionado nem como fallback
int test_server_without_listen_not_selected() {
    ServerConfig* s1 = new ServerConfig();
    // sem addListen — não escuta em nenhuma porta

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    const ServerConfig* result = wsc->match_server(80, "api.com");
    int r = assert_true(result == NULL, "servidor sem listen não é selecionado", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: global sem http → getServers vazio → NULL
int test_match_server_no_http() {
    WebServerConfig* wsc = new WebServerConfig();
    GlobalConfig* global = new GlobalConfig();
    // não adiciona HttpConfig ao global
    wsc->addChild(global);

    const ServerConfig* result = wsc->match_server(80, "api.com");
    int r = assert_true(result == NULL, "global sem http: match_server retorna NULL", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: host_header vazio → nenhum server_name bate → fallback
int test_empty_host_header_fallback() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addServerName("api.com");

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    const ServerConfig* result = wsc->match_server(80, "");
    int r = assert_true(result == s1, "host_header vazio cai no fallback", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: servidor com múltiplos server_names — segundo nome bate
int test_multiple_server_names_second_matches() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addServerName("primary.com");
    s1->addServerName("alias.com");

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    const ServerConfig* result = wsc->match_server(80, "alias.com");
    int r = assert_true(result == s1, "segundo server_name também bate", LINE_DATA());
    delete wsc;
    return r;
}

// getWorkerConnections: com events configurado retorna o valor real
int test_worker_connections_from_events() {
    WebServerConfig* wsc = new WebServerConfig();
    GlobalConfig* global = new GlobalConfig();
    HttpConfig* http = new HttpConfig();
    EventsConfig* events = new EventsConfig();
    events->setWorkerConnections(2048);

    global->addChild(http);
    global->addChild(events);
    wsc->addChild(global);

    int r = assert((size_t)2048, wsc->getWorkerConnections(), "getWorkerConnections com events: retorna 2048", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: servidor com múltiplos listens em portas diferentes
int test_server_multiple_listens() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(80));
    s1->addListen(makeListen(443));
    s1->addServerName("site.com");

    ServerConfig* servers[] = { s1 };
    WebServerConfig* wsc = buildConfig(servers, 1);

    int r = 0;
    r += assert_true(wsc->match_server(80,  "site.com") == s1, "match na porta 80",  LINE_DATA());
    r += assert_true(wsc->match_server(443, "site.com") == s1, "match na porta 443", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: fallback respeita ordem — primeiro servidor que tem aquela porta
int test_fallback_order_respects_first_port_match() {
    ServerConfig* s1 = new ServerConfig();
    s1->addListen(makeListen(8080));

    ServerConfig* s2 = new ServerConfig();
    s2->addListen(makeListen(80));
    s2->addListen(makeListen(8080));

    ServerConfig* servers[] = { s1, s2 };
    WebServerConfig* wsc = buildConfig(servers, 2);

    const ServerConfig* result = wsc->match_server(8080, "unknown.com");
    int r = assert_true(result == s1, "fallback: s1 é o primeiro com porta 8080", LINE_DATA());
    delete wsc;
    return r;
}

// match_server: lista de servidores vazia → NULL
int test_match_server_empty_server_list() {
    WebServerConfig* wsc = new WebServerConfig();
    GlobalConfig* global = new GlobalConfig();
    HttpConfig* http = new HttpConfig();
    // http sem servidores
    global->addChild(http);
    wsc->addChild(global);

    const ServerConfig* result = wsc->match_server(80, "api.com");
    int r = assert_true(result == NULL, "lista de servidores vazia: match_server retorna NULL", LINE_DATA());
    delete wsc;
    return r;
}

// getGlobal() retorna o objeto adicionado via addChild
int test_get_global() {
    WebServerConfig wsc;
    GlobalConfig* global = new GlobalConfig();
    wsc.addChild(global);
    return assert_true(wsc.getGlobal() == global, "getGlobal() retorna o objeto adicionado", LINE_DATA());
}

// cadeia completa: match_server → match_location end-to-end
// int test_end_to_end_server_then_location() {
//     ServerConfig* s1 = new ServerConfig();
//     s1->addListen(makeListen(80));
//     s1->addServerName("api.com");

//     LocationConfig* root = new LocationConfig();
//     root->setPath("/");
//     root->setModifier(MOD_NONE);
//     s1->addChild(root);

//     LocationConfig* api = new LocationConfig();
//     api->setPath("/api");
//     api->setModifier(MOD_NONE);
//     s1->addChild(api);

//     ServerConfig* servers[] = { s1 };
//     WebServerConfig* wsc = buildConfig(servers, 1);

//     const ServerConfig* server = wsc->match_server(80, "api.com");
//     int r = 0;
//     r += assert_true(server == s1, "end-to-end: servidor correto", LINE_DATA());
//     if (server) {
//         const LocationConfig* loc = server->match_location("/api/users");
//         r += assert_true(loc == api, "end-to-end: location correta", LINE_DATA());
//     }
//     delete wsc;
//     return r;
// }

// getWorkerConnections: events com worker_connections=0 → fallback 1024
int test_worker_connections_zero_fallback() {
    WebServerConfig* wsc = new WebServerConfig();
    GlobalConfig* global = new GlobalConfig();
    HttpConfig* http = new HttpConfig();
    EventsConfig* events = new EventsConfig();
    // worker_connections padrão é 0

    global->addChild(http);
    global->addChild(events);
    wsc->addChild(global);

    int r = assert((size_t)1024, wsc->getWorkerConnections(), "worker_connections=0 usa fallback 1024", LINE_DATA());
    delete wsc;
    return r;
}

int main(int argc, char **argv) {
    messages::settup(argc, argv);
    int failures = 0;

    failures += test_match_by_server_name();
    failures += test_fallback_first_on_port();
    failures += test_no_match_wrong_port();
    failures += test_host_header_strips_port();
    failures += test_two_servers_same_port();
    failures += test_two_servers_fallback_is_first();
    failures += test_server_different_port_not_fallback();
    failures += test_get_servers_no_global();
    failures += test_worker_connections_default();
    failures += test_webserver_addchild_invalid();
    failures += test_server_without_listen_not_selected();
    failures += test_match_server_no_http();
    failures += test_empty_host_header_fallback();
    failures += test_multiple_server_names_second_matches();
    failures += test_worker_connections_from_events();
    failures += test_worker_connections_zero_fallback();
    failures += test_server_multiple_listens();
    failures += test_fallback_order_respects_first_port_match();
    failures += test_match_server_empty_server_list();
    failures += test_get_global();
    // failures += test_end_to_end_server_then_location();

    messages::print();
    return failures;
}
