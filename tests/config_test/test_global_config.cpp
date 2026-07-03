#include "GlobalConfig.hpp"
#include "HttpConfig.hpp"
#include "EventsConfig.hpp"
#include "test_utils.hpp"

// defaults: workers=0, pid=0, error_log vazio, http e events nulos
int test_global_defaults() {
    GlobalConfig g;
    int r = 0;
    r += assert(0,               g.getWorkers(),  "workers padrão 0",    LINE_DATA());
    r += assert(0,               g.getPid(),      "pid padrão 0",        LINE_DATA());
    r += assert(std::string(""), g.getErrorLog(), "error_log padrão ''", LINE_DATA());
    r += assert_true(g.getHttp()   == NULL, "http padrão NULL",   LINE_DATA());
    r += assert_true(g.getEvents() == NULL, "events padrão NULL", LINE_DATA());
    return r;
}

// setters de campos escalares
int test_global_scalar_setters() {
    GlobalConfig g;
    g.setWorkers(4);
    g.setPid(1234);
    g.setErrorLog("/var/log/error.log");
    int r = 0;
    r += assert(4,                              g.getWorkers(),  "setWorkers(4)",          LINE_DATA());
    r += assert(1234,                           g.getPid(),      "setPid(1234)",           LINE_DATA());
    r += assert(std::string("/var/log/error.log"), g.getErrorLog(), "setErrorLog(path)",   LINE_DATA());
    return r;
}

// addChild com HttpConfig → getHttp retorna o objeto
int test_global_addchild_http() {
    GlobalConfig g;
    HttpConfig* http = new HttpConfig();
    g.addChild(http);
    return assert_true(g.getHttp() == http, "addChild HttpConfig: getHttp() retorna o objeto", LINE_DATA());
}

// addChild com EventsConfig → getEvents retorna o objeto
int test_global_addchild_events() {
    GlobalConfig g;
    EventsConfig* events = new EventsConfig();
    g.addChild(events);
    return assert_true(g.getEvents() == events, "addChild EventsConfig: getEvents() retorna o objeto", LINE_DATA());
}

// addChild com tipo inválido gera erro
int test_global_addchild_invalid() {
    GlobalConfig g;
    GlobalConfig* wrong = new GlobalConfig();
    g.addChild(wrong);
    int r = assert_true(g.getErrors().size() > 0, "addChild tipo inválido gera erro", LINE_DATA());
    delete wrong;
    return r;
}

// segundo addChild(HttpConfig) sobrescreve o primeiro (sem erro)
int test_global_addchild_http_overwrite() {
    GlobalConfig g;
    HttpConfig* first  = new HttpConfig();
    HttpConfig* second = new HttpConfig();
    g.addChild(first);
    g.addChild(second);
    int r = 0;
    r += assert_true(g.getHttp() == second, "segundo HttpConfig sobrescreve o primeiro", LINE_DATA());
    r += assert_true(g.getErrors().size() == 0, "sobrescrita não gera erro", LINE_DATA());
    // first vazou — destrutor de g deleta second; first deve ser deletado aqui
    delete first;
    return r;
}

// segundo addChild(EventsConfig) sobrescreve o primeiro (sem erro)
int test_global_addchild_events_overwrite() {
    GlobalConfig g;
    EventsConfig* first  = new EventsConfig();
    EventsConfig* second = new EventsConfig();
    g.addChild(first);
    g.addChild(second);
    int r = 0;
    r += assert_true(g.getEvents() == second, "segundo EventsConfig sobrescreve o primeiro", LINE_DATA());
    r += assert_true(g.getErrors().size() == 0, "sobrescrita não gera erro", LINE_DATA());
    delete first;
    return r;
}

// addChild aceita tanto HttpConfig quanto EventsConfig sem erro
int test_global_addchild_http_and_events() {
    GlobalConfig g;
    HttpConfig*   http   = new HttpConfig();
    EventsConfig* events = new EventsConfig();
    g.addChild(http);
    g.addChild(events);
    int r = 0;
    r += assert_true(g.getHttp()   == http,   "http configurado corretamente",   LINE_DATA());
    r += assert_true(g.getEvents() == events, "events configurado corretamente", LINE_DATA());
    r += assert_true(g.getErrors().size() == 0, "sem erros ao adicionar ambos",  LINE_DATA());
    return r;
}

int main(int argc, char **argv) {
    messages::settup(argc, argv);
    int failures = 0;

    failures += test_global_defaults();
    failures += test_global_scalar_setters();
    failures += test_global_addchild_http();
    failures += test_global_addchild_events();
    failures += test_global_addchild_invalid();
    failures += test_global_addchild_http_overwrite();
    failures += test_global_addchild_events_overwrite();
    failures += test_global_addchild_http_and_events();

    messages::print();
    return failures;
}
