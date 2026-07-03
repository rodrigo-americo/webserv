#include "ParserBuilder.hpp"
#include "ConfigBuilderVisitor.hpp"
#include "WebServerConfig.hpp"
#include "GlobalConfig.hpp"
#include "HttpConfig.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "EventsConfig.hpp"
#include "UpstreamConfig.hpp"
#include "test_utils.hpp"
#include <string>
#include <cstring>

// Deriva o diretório do source file para localizar fixtures ao lado
static std::string fixture_path(const std::string& name) {
    std::string src = __FILE__;
    size_t last_sep = src.find_last_of("/\\");
    std::string dir = (last_sep == std::string::npos) ? "." : src.substr(0, last_sep);
    return dir + "/fixtures/" + name;
}

// Carrega um .conf e retorna o WebServerConfig* (caller responsável por delete)
static WebServerConfig* load(const std::string& conf_file) {
    ParserAst ast = ParserBuilder::defaultBuilder().withFile(conf_file).build();
    ast.validateStructure();
    ConfigBuilderVisitor visitor;
    ast.applyVisitor(visitor);
    return visitor.getResult();
}

// ─── Grupo 1: parse básico (basic.conf) ───────────────────────────────────────

int test_parse_not_null() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    int r = assert_true(cfg != NULL, "basic.conf: parse retorna objeto não nulo", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_workers() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    int r = assert(4, cfg->getGlobal()->getWorkers(), "worker_processes 4", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_error_log() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    int r = assert(std::string("/var/log/nginx/error.log"),
                   cfg->getGlobal()->getErrorLog(), "error_log path", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_worker_connections() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    int r = assert((size_t)1024, cfg->getWorkerConnections(), "worker_connections 1024", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_server_count() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    int r = assert((size_t)1, cfg->getServers().size(), "basic.conf: 1 server", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_server_name() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    int r = assert_true(s->getServerNames().size() >= 1, "server tem server_name", LINE_DATA());
    if (r == 0)
        r += assert(std::string("example.com"), s->getServerNames().front(),
                    "primeiro server_name é example.com", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_listen_port() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    int r = assert_true(s->getListen().size() >= 1, "server tem listen", LINE_DATA());
    if (r == 0)
        r += assert((size_t)8080, s->getListen().front().port, "porta listen é 8080", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_root() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    int r = assert(std::string("/var/www/html"), s->getRoot(), "root /var/www/html", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_client_max_body_size() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    // atoi("10m") == 10
    int r = assert((size_t)10, s->getClientMaxBodySize(), "client_max_body_size 10m → 10", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_location_count_basic() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    int r = assert((size_t)2, s->getLocations().size(), "basic.conf: 2 locations", LINE_DATA());
    delete cfg;
    return r;
}

// ─── Grupo 2: upstream (basic.conf) ───────────────────────────────────────────

int test_parse_upstream_count() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const HttpConfig* http = cfg->getGlobal()->getHttp();
    int r = assert((size_t)1, http->getUpstreams().size(), "1 upstream", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_upstream_servers() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const UpstreamConfig* up = cfg->getGlobal()->getHttp()->getUpstreams().front();
    int r = assert((size_t)3, up->getServers().size(), "upstream tem 3 servers", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_upstream_first_ip() {
    WebServerConfig* cfg = load(fixture_path("basic.conf"));
    if (!cfg) return 1;
    const UpstreamConfig* up = cfg->getGlobal()->getHttp()->getUpstreams().front();
    int r = assert(std::string("127.0.0.1:8080"),
                   up->getServers().front()._ip, "primeiro upstream server ip", LINE_DATA());
    delete cfg;
    return r;
}

// ─── Grupo 3: locations com modificadores (locations.conf) ────────────────────

int test_parse_location_none() {
    WebServerConfig* cfg = load(fixture_path("locations.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    bool found = false;
    const std::list<LocationConfig*>& locs = s->getLocations();
    for (std::list<LocationConfig*>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
        if ((*it)->getPath() == "/" && (*it)->getModifier() == MOD_NONE) { found = true; break; }
    }
    int r = assert_true(found, "location / com MOD_NONE presente", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_location_exact() {
    WebServerConfig* cfg = load(fixture_path("locations.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    bool found = false;
    const std::list<LocationConfig*>& locs = s->getLocations();
    for (std::list<LocationConfig*>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
        if ((*it)->getPath() == "/favicon.ico" && (*it)->getModifier() == MOD_EXACT) { found = true; break; }
    }
    int r = assert_true(found, "location = /favicon.ico com MOD_EXACT presente", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_location_prefix() {
    WebServerConfig* cfg = load(fixture_path("locations.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    bool found = false;
    const std::list<LocationConfig*>& locs = s->getLocations();
    for (std::list<LocationConfig*>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
        if ((*it)->getPath() == "/api" && (*it)->getModifier() == MOD_PREFIX) { found = true; break; }
    }
    int r = assert_true(found, "location ^~ /api com MOD_PREFIX presente", LINE_DATA());
    delete cfg;
    return r;
}

int test_parse_location_regex() {
    WebServerConfig* cfg = load(fixture_path("locations.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    bool found = false;
    const std::list<LocationConfig*>& locs = s->getLocations();
    for (std::list<LocationConfig*>::const_iterator it = locs.begin(); it != locs.end(); ++it) {
        if ((*it)->getModifier() == MOD_REGEX) { found = true; break; }
    }
    int r = assert_true(found, "location ~ .php$ com MOD_REGEX presente", LINE_DATA());
    delete cfg;
    return r;
}

int test_match_location_exact_wins() {
    WebServerConfig* cfg = load(fixture_path("locations.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    const LocationConfig* loc = s->match_location("/favicon.ico");
    int r = 0;
    r += assert_true(loc != NULL, "match_location /favicon.ico não retorna NULL", LINE_DATA());
    if (loc)
        r += assert(MOD_EXACT, loc->getModifier(), "match_location /favicon.ico retorna MOD_EXACT", LINE_DATA());
    delete cfg;
    return r;
}

int test_match_location_prefix_wins() {
    WebServerConfig* cfg = load(fixture_path("locations.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    const LocationConfig* loc = s->match_location("/api/v2/users");
    int r = 0;
    r += assert_true(loc != NULL, "match_location /api/v2/users não retorna NULL", LINE_DATA());
    if (loc)
        r += assert(MOD_PREFIX, loc->getModifier(), "match_location /api/v2/users retorna MOD_PREFIX", LINE_DATA());
    delete cfg;
    return r;
}

int test_match_location_fallback_root() {
    WebServerConfig* cfg = load(fixture_path("locations.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->getServers().front();
    const LocationConfig* loc = s->match_location("/other/path");
    int r = 0;
    r += assert_true(loc != NULL, "match_location /other/path não retorna NULL", LINE_DATA());
    if (loc)
        r += assert(std::string("/"), loc->getPath(), "fallback para location /", LINE_DATA());
    delete cfg;
    return r;
}

// ─── Grupo 4: multi-server (multi_server.conf) ────────────────────────────────

int test_parse_two_servers() {
    WebServerConfig* cfg = load(fixture_path("multi_server.conf"));
    if (!cfg) return 1;
    int r = assert((size_t)2, cfg->getServers().size(), "multi_server.conf: 2 servers", LINE_DATA());
    delete cfg;
    return r;
}

int test_match_server_by_name() {
    WebServerConfig* cfg = load(fixture_path("multi_server.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->match_server(8080, "example.com");
    int r = assert_true(s != NULL, "match_server(8080, example.com) não retorna NULL", LINE_DATA());
    if (s)
        r += assert((size_t)8080, s->getListen().front().port, "servidor retornado escuta na 8080", LINE_DATA());
    delete cfg;
    return r;
}

int test_match_server_by_port_443() {
    WebServerConfig* cfg = load(fixture_path("multi_server.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->match_server(443, "example.com");
    int r = assert_true(s != NULL, "match_server(443, example.com) não retorna NULL", LINE_DATA());
    if (s)
        r += assert((size_t)443, s->getListen().front().port, "servidor retornado escuta na 443", LINE_DATA());
    delete cfg;
    return r;
}

int test_match_server_wrong_port() {
    WebServerConfig* cfg = load(fixture_path("multi_server.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->match_server(9999, "example.com");
    int r = assert_true(s == NULL, "match_server(9999, example.com) retorna NULL", LINE_DATA());
    delete cfg;
    return r;
}

int test_match_server_ssl_has_cert() {
    WebServerConfig* cfg = load(fixture_path("multi_server.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->match_server(443, "example.com");
    int r = 0;
    r += assert_true(s != NULL, "servidor 443 encontrado", LINE_DATA());
    if (s)
        r += assert(std::string("/etc/nginx/ssl/example.crt"),
                    s->getSslCertificate(), "ssl_certificate parseado", LINE_DATA());
    delete cfg;
    return r;
}

// ─── Grupo 5: end-to-end (multi_server.conf) ──────────────────────────────────

int test_end_to_end_server_then_location() {
    WebServerConfig* cfg = load(fixture_path("multi_server.conf"));
    if (!cfg) return 1;
    const ServerConfig* s = cfg->match_server(8080, "example.com");
    int r = assert_true(s != NULL, "end-to-end: servidor encontrado", LINE_DATA());
    if (s) {
        const LocationConfig* loc = s->match_location("/qualquer");
        r += assert_true(loc != NULL, "end-to-end: location fallback / encontrada", LINE_DATA());
    }
    delete cfg;
    return r;
}

int main(int argc, char **argv) {
    messages::settup(argc, argv);
    int failures = 0;

    // Grupo 1: parse básico
    failures += test_parse_not_null();
    failures += test_parse_workers();
    failures += test_parse_error_log();
    failures += test_parse_worker_connections();
    failures += test_parse_server_count();
    failures += test_parse_server_name();
    failures += test_parse_listen_port();
    failures += test_parse_root();
    failures += test_parse_client_max_body_size();
    failures += test_parse_location_count_basic();

    // Grupo 2: upstream
    failures += test_parse_upstream_count();
    failures += test_parse_upstream_servers();
    failures += test_parse_upstream_first_ip();

    // Grupo 3: locations com modificadores
    failures += test_parse_location_none();
    failures += test_parse_location_exact();
    failures += test_parse_location_prefix();
    failures += test_parse_location_regex();
    failures += test_match_location_exact_wins();
    failures += test_match_location_prefix_wins();
    failures += test_match_location_fallback_root();

    // Grupo 4: multi-server
    failures += test_parse_two_servers();
    failures += test_match_server_by_name();
    failures += test_match_server_by_port_443();
    failures += test_match_server_wrong_port();
    failures += test_match_server_ssl_has_cert();

    // Grupo 5: end-to-end
    failures += test_end_to_end_server_then_location();

    messages::print();
    return failures;
}
