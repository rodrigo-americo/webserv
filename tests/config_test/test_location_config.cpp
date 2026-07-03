#include "LocationConfig.hpp"
#include "test_utils.hpp"

// MOD_NONE: prefixo
int test_none_prefix_match() {
    LocationConfig loc;
    loc.setPath("/api");
    loc.setModifier(MOD_NONE);
    return assert_true(loc.matches("/api/users"), "MOD_NONE: prefix match", LINE_DATA());
}

// MOD_NONE: uri igual ao path
int test_none_exact_uri() {
    LocationConfig loc;
    loc.setPath("/api");
    loc.setModifier(MOD_NONE);
    return assert_true(loc.matches("/api"), "MOD_NONE: uri igual ao path", LINE_DATA());
}

// MOD_NONE: sem match
int test_none_no_match() {
    LocationConfig loc;
    loc.setPath("/api");
    loc.setModifier(MOD_NONE);
    return assert_false(loc.matches("/other"), "MOD_NONE: sem match");
}

// MOD_NONE: path raiz "/" bate com tudo
int test_none_root_matches_all() {
    LocationConfig loc;
    loc.setPath("/");
    loc.setModifier(MOD_NONE);
    return assert_true(loc.matches("/qualquer/coisa"), "MOD_NONE: raiz bate com tudo", LINE_DATA());
}

// MOD_PREFIX: match normal
int test_prefix_match() {
    LocationConfig loc;
    loc.setPath("/img");
    loc.setModifier(MOD_PREFIX);
    return assert_true(loc.matches("/img/logo.png"), "MOD_PREFIX: match", LINE_DATA());
}

// MOD_PREFIX: sem match
int test_prefix_no_match() {
    LocationConfig loc;
    loc.setPath("/img");
    loc.setModifier(MOD_PREFIX);
    return assert_false(loc.matches("/other"), "MOD_PREFIX: sem match");
}

// MOD_PREFIX: uri igual ao path
int test_prefix_exact_uri() {
    LocationConfig loc;
    loc.setPath("/img");
    loc.setModifier(MOD_PREFIX);
    return assert_true(loc.matches("/img"), "MOD_PREFIX: uri igual ao path", LINE_DATA());
}

// MOD_EXACT: match exato
int test_exact_match() {
    LocationConfig loc;
    loc.setPath("/login");
    loc.setModifier(MOD_EXACT);
    return assert_true(loc.matches("/login"), "MOD_EXACT: match exato", LINE_DATA());
}

// MOD_EXACT: rejeita prefixo (uri mais longa)
int test_exact_rejects_prefix() {
    LocationConfig loc;
    loc.setPath("/login");
    loc.setModifier(MOD_EXACT);
    return assert_false(loc.matches("/login/extra"), "MOD_EXACT: rejeita uri mais longa");
}

// MOD_EXACT: rejeita uri mais curta
int test_exact_rejects_shorter() {
    LocationConfig loc;
    loc.setPath("/login");
    loc.setModifier(MOD_EXACT);
    return assert_false(loc.matches("/logi"), "MOD_EXACT: rejeita uri mais curta");
}

// MOD_REGEX: não implementado → sempre false
int test_regex_not_implemented() {
    LocationConfig loc;
    loc.setPath("/.*\\.php");
    loc.setModifier(MOD_REGEX);
    return assert_false(loc.matches("/index.php"), "MOD_REGEX: não implementado retorna false");
}

// MOD_REGEX_CI: não implementado → sempre false
int test_regex_ci_not_implemented() {
    LocationConfig loc;
    loc.setPath("/.*\\.php");
    loc.setModifier(MOD_REGEX_CI);
    return assert_false(loc.matches("/INDEX.PHP"), "MOD_REGEX_CI: não implementado retorna false");
}

// Getters: path e modifier preservados
int test_getters_path_modifier() {
    LocationConfig loc;
    loc.setPath("/static");
    loc.setModifier(MOD_PREFIX);
    int r = 0;
    r += assert(std::string("/static"), loc.getPath(), "getPath()", LINE_DATA());
    r += assert(MOD_PREFIX, loc.getModifier(), "getModifier()", LINE_DATA());
    return r;
}

// Getter: autoindex padrão false
int test_autoindex_default() {
    LocationConfig loc;
    return assert_false(loc.getAutoIndex(), "autoindex padrão false");
}

// Getter: redirect padrão {0, ""}
int test_redirect_default() {
    LocationConfig loc;
    int r = 0;
    r += assert(0, loc.getRedirect().first, "redirect code padrão 0", LINE_DATA());
    r += assert(std::string(""), loc.getRedirect().second, "redirect url padrão vazia", LINE_DATA());
    return r;
}

// addMethod / getMethods
int test_add_method() {
    LocationConfig loc;
    loc.addMethod(GET);
    loc.addMethod(POST);
    return assert((size_t)2, loc.getMethods().size(), "addMethod: tamanho da lista", LINE_DATA());
}

// addIndex / getIndex
int test_add_index() {
    LocationConfig loc;
    loc.addIndex("index.html");
    loc.addIndex("index.php");
    return assert((size_t)2, loc.getIndex().size(), "addIndex: tamanho da lista", LINE_DATA());
}

// addCgi / getCgi
int test_add_cgi() {
    LocationConfig loc;
    loc.addCgi(std::make_pair(std::string(".php"), std::string("/usr/bin/php-cgi")));
    return assert((size_t)1, loc.getCgi().size(), "addCgi: tamanho do map", LINE_DATA());
}

// path com trailing slash AINDA bate com uri que começa com ele (substr inclui a barra)
int test_trailing_slash_still_matches() {
    LocationConfig loc;
    loc.setPath("/api/");
    loc.setModifier(MOD_NONE);
    return assert_true(loc.matches("/api/users"), "trailing slash no path bate com uri que começa com '/api/'", LINE_DATA());
}

// uri mais curta que o path não bate (substr pega menos chars)
int test_uri_shorter_than_path() {
    LocationConfig loc;
    loc.setPath("/api/v2");
    loc.setModifier(MOD_NONE);
    return assert_false(loc.matches("/api"), "uri mais curta que o path não bate");
}

// path vazio bate com qualquer uri (substr(0,0) == "")
int test_empty_path_matches_any() {
    LocationConfig loc;
    loc.setPath("");
    loc.setModifier(MOD_NONE);
    return assert_true(loc.matches("/qualquer"), "path vazio bate com qualquer uri", LINE_DATA());
}

// addTryFile / getTryFiles acumula
int test_add_try_file() {
    LocationConfig loc;
    loc.addTryFile("/index.html");
    loc.addTryFile("/index.php");
    return assert((size_t)2, loc.getTryFiles().size(), "addTryFile: tamanho da lista", LINE_DATA());
}

// addAddHeader / getAddHeader acumula pares
int test_add_header() {
    LocationConfig loc;
    loc.addAddHeader("X-Frame-Options", "DENY");
    loc.addAddHeader("X-Content-Type-Options", "nosniff");
    return assert((size_t)2, loc.getAddHeader().size(), "addAddHeader: tamanho da lista", LINE_DATA());
}

// addProxySetHeader / getProxySetHeader acumula pares
int test_proxy_set_header() {
    LocationConfig loc;
    loc.addProxySetHeader("Host", "$host");
    loc.addProxySetHeader("X-Real-IP", "$remote_addr");
    return assert((size_t)2, loc.getProxySetHeader().size(), "addProxySetHeader: tamanho da lista", LINE_DATA());
}

// setters de strings preservam valor
int test_string_setters() {
    LocationConfig loc;
    loc.setRoot("/var/www");
    loc.setUploadDir("/tmp/uploads");
    loc.setProxyPass("http://backend");
    loc.setProxyCacheBypass("$http_pragma");
    loc.setFastcgiIndex("index.php");
    loc.setExpires("1d");
    int r = 0;
    r += assert(std::string("/var/www"),       loc.getRoot(),             "getRoot()",             LINE_DATA());
    r += assert(std::string("/tmp/uploads"),   loc.getUploadDir(),        "getUploadDir()",        LINE_DATA());
    r += assert(std::string("http://backend"), loc.getProxyPass(),        "getProxyPass()",        LINE_DATA());
    r += assert(std::string("$http_pragma"),   loc.getProxyCacheBypass(), "getProxyCacheBypass()", LINE_DATA());
    r += assert(std::string("index.php"),      loc.getFastcgiIndex(),     "getFastcgiIndex()",     LINE_DATA());
    r += assert(std::string("1d"),             loc.getExpires(),          "getExpires()",          LINE_DATA());
    return r;
}

// setAutoindex true preserva valor
int test_autoindex_set_true() {
    LocationConfig loc;
    loc.setAutoindex(true);
    return assert_true(loc.getAutoIndex(), "setAutoindex true", LINE_DATA());
}

// setRedirect preserva code e url
int test_redirect_setter() {
    LocationConfig loc;
    loc.setRedirect(301, "https://example.com");
    int r = 0;
    r += assert(301,                           loc.getRedirect().first,  "redirect code 301",      LINE_DATA());
    r += assert(std::string("https://example.com"), loc.getRedirect().second, "redirect url",      LINE_DATA());
    return r;
}

// MOD_EXACT com path e uri ambos vazios → true
int test_exact_empty_path_empty_uri() {
    LocationConfig loc;
    loc.setPath("");
    loc.setModifier(MOD_EXACT);
    return assert_true(loc.matches(""), "MOD_EXACT: path vazio bate com uri vazia", LINE_DATA());
}

// MOD_EXACT com path vazio não bate com uri não vazia
int test_exact_empty_path_nonempty_uri() {
    LocationConfig loc;
    loc.setPath("");
    loc.setModifier(MOD_EXACT);
    return assert_false(loc.matches("/algo"), "MOD_EXACT: path vazio não bate com uri não vazia");
}

int main(int argc, char **argv) {
    messages::settup(argc, argv);
    int failures = 0;

    failures += test_none_prefix_match();
    failures += test_none_exact_uri();
    failures += test_none_no_match();
    failures += test_none_root_matches_all();
    failures += test_prefix_match();
    failures += test_prefix_no_match();
    failures += test_prefix_exact_uri();
    failures += test_exact_match();
    failures += test_exact_rejects_prefix();
    failures += test_exact_rejects_shorter();
    failures += test_regex_not_implemented();
    failures += test_regex_ci_not_implemented();
    failures += test_getters_path_modifier();
    failures += test_autoindex_default();
    failures += test_redirect_default();
    failures += test_add_method();
    failures += test_add_index();
    failures += test_add_cgi();
    failures += test_trailing_slash_still_matches();
    failures += test_uri_shorter_than_path();
    failures += test_empty_path_matches_any();
    failures += test_add_try_file();
    failures += test_add_header();
    failures += test_proxy_set_header();
    failures += test_string_setters();
    failures += test_autoindex_set_true();
    failures += test_redirect_setter();
    failures += test_exact_empty_path_empty_uri();
    failures += test_exact_empty_path_nonempty_uri();

    messages::print();
    return failures;
}
