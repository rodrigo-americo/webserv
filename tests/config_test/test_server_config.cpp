#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "ConfigServerListen.hpp"
#include "test_utils.hpp"

static ConfigServerListen makeListen(size_t port) {
    ConfigServerListen l;
    l.port = port;
    l.address = "0.0.0.0";
    return l;
}

// match_location: MOD_EXACT tem prioridade sobre MOD_NONE com mesmo path
int test_exact_beats_none() {
    ServerConfig s;

    LocationConfig* none = new LocationConfig();
    none->setPath("/api");
    none->setModifier(MOD_NONE);
    s.addChild(none);

    LocationConfig* exact = new LocationConfig();
    exact->setPath("/api");
    exact->setModifier(MOD_EXACT);
    s.addChild(exact);

    const LocationConfig* result = s.match_location("/api");
    return assert_true(result == exact, "EXACT tem prioridade sobre NONE no mesmo path", LINE_DATA());
}

// match_location: prefix mais longo vence
int test_longest_prefix_wins() {
    ServerConfig s;

    LocationConfig* root = new LocationConfig();
    root->setPath("/");
    root->setModifier(MOD_NONE);
    s.addChild(root);

    LocationConfig* api = new LocationConfig();
    api->setPath("/api");
    api->setModifier(MOD_NONE);
    s.addChild(api);

    LocationConfig* apiv2 = new LocationConfig();
    apiv2->setPath("/api/v2");
    apiv2->setModifier(MOD_NONE);
    s.addChild(apiv2);

    const LocationConfig* result = s.match_location("/api/v2/users");
    return assert_true(result == apiv2, "prefix mais longo vence", LINE_DATA());
}

// match_location: fallback para raiz "/"
int test_fallback_root() {
    ServerConfig s;

    LocationConfig* root = new LocationConfig();
    root->setPath("/");
    root->setModifier(MOD_NONE);
    s.addChild(root);

    const LocationConfig* result = s.match_location("/qualquer/coisa");
    return assert_true(result == root, "fallback para location raiz", LINE_DATA());
}

// match_location: sem match retorna NULL
int test_no_match_returns_null() {
    ServerConfig s;

    LocationConfig* api = new LocationConfig();
    api->setPath("/api");
    api->setModifier(MOD_NONE);
    s.addChild(api);

    const LocationConfig* result = s.match_location("/other");
    return assert_true(result == NULL, "sem match retorna NULL", LINE_DATA());
}

// match_location: MOD_PREFIX mais longo vence entre vários prefix
int test_longest_prefix_modifier_wins() {
    ServerConfig s;

    LocationConfig* img = new LocationConfig();
    img->setPath("/img");
    img->setModifier(MOD_PREFIX);
    s.addChild(img);

    LocationConfig* imgthumb = new LocationConfig();
    imgthumb->setPath("/img/thumb");
    imgthumb->setModifier(MOD_PREFIX);
    s.addChild(imgthumb);

    const LocationConfig* result = s.match_location("/img/thumb/photo.jpg");
    return assert_true(result == imgthumb, "MOD_PREFIX mais longo vence", LINE_DATA());
}

// match_location: MOD_EXACT em path diferente não interfere
int test_exact_different_path_no_interference() {
    ServerConfig s;

    LocationConfig* root = new LocationConfig();
    root->setPath("/");
    root->setModifier(MOD_NONE);
    s.addChild(root);

    LocationConfig* exact = new LocationConfig();
    exact->setPath("/login");
    exact->setModifier(MOD_EXACT);
    s.addChild(exact);

    const LocationConfig* result = s.match_location("/home");
    return assert_true(result == root, "MOD_EXACT em path diferente não interfere", LINE_DATA());
}

// addChild com tipo inválido gera erro
int test_addchild_invalid_type() {
    ServerConfig s;

    // HttpConfig não é LocationConfig — passa um LocationConfig como teste de tipo errado
    // Aqui usamos um ConfigNode genérico via subclasse LocationConfig passada como GlobalConfig
    // O test direto: passar um ServerConfig dentro de outro ServerConfig (tipo errado)
    ServerConfig* wrong = new ServerConfig();
    s.addChild(wrong);
    return assert_true(s.getErrors().size() > 0, "addChild tipo inválido gera erro", LINE_DATA());
}

// Getters básicos
int test_server_getters_defaults() {
    ServerConfig s;
    int r = 0;
    r += assert((size_t)0, s.getClientMaxBodySize(), "client_max_body_size padrão 0", LINE_DATA());
    r += assert(0, s.getRedirect().first, "redirect code padrão 0", LINE_DATA());
    r += assert(std::string(""), s.getRedirect().second, "redirect url padrão vazia", LINE_DATA());
    return r;
}

// addListen / getListen
int test_add_listen() {
    ServerConfig s;
    s.addListen(makeListen(80));
    s.addListen(makeListen(443));
    return assert((size_t)2, s.getListen().size(), "addListen: tamanho da lista", LINE_DATA());
}

// addServerName / getServerNames
int test_add_server_name() {
    ServerConfig s;
    s.addServerName("example.com");
    s.addServerName("www.example.com");
    return assert((size_t)2, s.getServerNames().size(), "addServerName: tamanho da lista", LINE_DATA());
}

// addErrorPage / getErrorPages
int test_add_error_page() {
    ServerConfig s;
    s.addErrorPage(404, "/404.html");
    s.addErrorPage(500, "/500.html");
    return assert((size_t)2, s.getErrorPages().size(), "addErrorPage: tamanho do map", LINE_DATA());
}

// match_location: MOD_PREFIX vence MOD_NONE mesmo sendo mais curto
int test_prefix_beats_longer_none() {
    ServerConfig s;

    LocationConfig* none = new LocationConfig();
    none->setPath("/api/v2");
    none->setModifier(MOD_NONE);
    s.addChild(none);

    LocationConfig* prefix = new LocationConfig();
    prefix->setPath("/api");
    prefix->setModifier(MOD_PREFIX);
    s.addChild(prefix);

    const LocationConfig* result = s.match_location("/api/v2/users");
    return assert_true(result == prefix, "MOD_PREFIX mais curto vence MOD_NONE mais longo", LINE_DATA());
}

// match_location: lista de locations vazia retorna NULL
int test_empty_locations_returns_null() {
    ServerConfig s;
    const LocationConfig* result = s.match_location("/qualquer");
    return assert_true(result == NULL, "lista vazia: match_location retorna NULL", LINE_DATA());
}

// match_location: uri vazia com location "/" não bate (substr(0,1) != "" para path "/")
int test_empty_uri_root_location() {
    ServerConfig s;

    LocationConfig* root = new LocationConfig();
    root->setPath("/");
    root->setModifier(MOD_NONE);
    s.addChild(root);

    const LocationConfig* result = s.match_location("");
    return assert_true(result == NULL, "uri vazia não bate com location '/'", LINE_DATA());
}

// addChild com NULL gera erro (dynamic_cast de NULL → NULL → entra no else)
int test_addchild_null() {
    ServerConfig s;
    s.addChild(NULL);
    return assert_true(s.getErrors().size() > 0, "addChild NULL gera erro", LINE_DATA());
}

// setters de string preservam valor
int test_server_string_setters() {
    ServerConfig s;
    s.setRoot("/var/www/html");
    s.setAccessLog("/var/log/access.log");
    s.setSslCertificate("/etc/ssl/cert.pem");
    s.setSslCertificateKey("/etc/ssl/key.pem");
    s.setSslProtocols("TLSv1.2 TLSv1.3");
    s.setSslCiphers("HIGH:!aNULL");
    int r = 0;
    r += assert(std::string("/var/www/html"),        s.getRoot(),               "getRoot()",               LINE_DATA());
    r += assert(std::string("/var/log/access.log"),  s.getAccessLog(),          "getAccessLog()",          LINE_DATA());
    r += assert(std::string("/etc/ssl/cert.pem"),    s.getSslCertificate(),     "getSslCertificate()",     LINE_DATA());
    r += assert(std::string("/etc/ssl/key.pem"),     s.getSslCertificateKey(),  "getSslCertificateKey()",  LINE_DATA());
    r += assert(std::string("TLSv1.2 TLSv1.3"),     s.getSslProtocols(),       "getSslProtocols()",       LINE_DATA());
    r += assert(std::string("HIGH:!aNULL"),          s.getSslCiphers(),         "getSslCiphers()",         LINE_DATA());
    return r;
}

// setRedirect preserva code e url
int test_server_redirect_setter() {
    ServerConfig s;
    s.setRedirect(301, "https://example.com");
    int r = 0;
    r += assert(301,                                s.getRedirect().first,  "redirect code 301", LINE_DATA());
    r += assert(std::string("https://example.com"), s.getRedirect().second, "redirect url",      LINE_DATA());
    return r;
}

// setClientMaxBodySize preserva valor
int test_client_max_body_size_setter() {
    ServerConfig s;
    s.setClientMaxBodySize(10485760);
    return assert((size_t)10485760, s.getClientMaxBodySize(), "setClientMaxBodySize 10MB", LINE_DATA());
}

// MOD_EXACT ignorado quando uri não bate exatamente — NONE ainda é retornado
int test_exact_skipped_when_uri_not_exact() {
    ServerConfig s;

    LocationConfig* exact = new LocationConfig();
    exact->setPath("/login");
    exact->setModifier(MOD_EXACT);
    s.addChild(exact);

    LocationConfig* root = new LocationConfig();
    root->setPath("/");
    root->setModifier(MOD_NONE);
    s.addChild(root);

    const LocationConfig* result = s.match_location("/login/callback");
    return assert_true(result == root, "EXACT ignorado quando uri não é exata, NONE retornado", LINE_DATA());
}

// getLocations() vazia por padrão
int test_get_locations_default_empty() {
    ServerConfig s;
    return assert((size_t)0, s.getLocations().size(), "getLocations() padrão: lista vazia", LINE_DATA());
}

// getLocations() retorna todas as locations adicionadas
int test_get_locations_accumulates() {
    ServerConfig s;
    LocationConfig* loc1 = new LocationConfig();
    loc1->setPath("/api");
    loc1->setModifier(MOD_NONE);
    LocationConfig* loc2 = new LocationConfig();
    loc2->setPath("/static");
    loc2->setModifier(MOD_PREFIX);
    s.addChild(loc1);
    s.addChild(loc2);
    return assert((size_t)2, s.getLocations().size(), "getLocations() acumula 2 locations", LINE_DATA());
}

// getLocations() preserva identidade dos ponteiros
int test_get_locations_pointer_identity() {
    ServerConfig s;
    LocationConfig* loc1 = new LocationConfig();
    loc1->setPath("/a");
    loc1->setModifier(MOD_NONE);
    LocationConfig* loc2 = new LocationConfig();
    loc2->setPath("/b");
    loc2->setModifier(MOD_NONE);
    s.addChild(loc1);
    s.addChild(loc2);
    const std::list<LocationConfig*>& locs = s.getLocations();
    int r = 0;
    r += assert_true(locs.front() == loc1, "getLocations(): primeiro ponteiro preservado", LINE_DATA());
    r += assert_true(locs.back()  == loc2, "getLocations(): segundo ponteiro preservado",  LINE_DATA());
    return r;
}

// Empate de tamanho em MOD_NONE — o que bate vence, o que não bate é ignorado
int test_none_same_size_only_matching_wins() {
    ServerConfig s;

    LocationConfig* api = new LocationConfig();
    api->setPath("/api");
    api->setModifier(MOD_NONE);
    s.addChild(api);

    LocationConfig* img = new LocationConfig();
    img->setPath("/img");
    img->setModifier(MOD_NONE);
    s.addChild(img);

    const LocationConfig* result = s.match_location("/api/users");
    return assert_true(result == api, "empate de tamanho: apenas o que bate é retornado", LINE_DATA());
}

int main(int argc, char **argv) {
    messages::settup(argc, argv);
    int failures = 0;

    failures += test_exact_beats_none();
    failures += test_longest_prefix_wins();
    failures += test_fallback_root();
    failures += test_no_match_returns_null();
    failures += test_longest_prefix_modifier_wins();
    failures += test_exact_different_path_no_interference();
    failures += test_addchild_invalid_type();
    failures += test_server_getters_defaults();
    failures += test_add_listen();
    failures += test_add_server_name();
    failures += test_add_error_page();
    failures += test_prefix_beats_longer_none();
    failures += test_empty_locations_returns_null();
    failures += test_empty_uri_root_location();
    failures += test_addchild_null();
    failures += test_server_string_setters();
    failures += test_server_redirect_setter();
    failures += test_client_max_body_size_setter();
    failures += test_exact_skipped_when_uri_not_exact();
    failures += test_none_same_size_only_matching_wins();
    failures += test_get_locations_default_empty();
    failures += test_get_locations_accumulates();
    failures += test_get_locations_pointer_identity();

    messages::print();
    return failures;
}
