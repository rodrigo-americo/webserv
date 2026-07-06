#include "ConfigBuilderVisitor.hpp"
#include <cstdlib>
#include "ConfigServer.hpp"
#include "Path.hpp"
#include "ServerListenParser.hpp"


ConfigBuilderVisitor::~ConfigBuilderVisitor() {}

void ConfigBuilderVisitor::_addError(const std::string& msg)
{
    if (_result)
        _result->addError(msg);
    else
        _pendingErrors.push_back(msg);
}

void ConfigBuilderVisitor::visit(Block& block)
{
    ConfigNode* node = NULL;

    switch (block.name.getType())
    {
        case ParserTokenType::PT_MAIN:
        {
            WebServerConfig* ws = new WebServerConfig();
            GlobalConfig* gc = new GlobalConfig();
            ws->addChild(gc);
            _result = ws;
            for (size_t i = 0; i < _pendingErrors.size(); ++i)
                _result->addError(_pendingErrors[i]);
            _pendingErrors.clear();
            node = gc;
            break;
        }
        case ParserTokenType::PT_HTTP:
            node = _createChild<HttpConfig>("http");
            break;
        case ParserTokenType::PT_EVENTS:
            node = _createChild<EventsConfig>("events");
            break;
        case ParserTokenType::PT_LOCATION:
        {
            LocationConfig* lc = _createChild<LocationConfig>("location");
            if (block.values.size() == 1)
                lc->setPath(Path(block.values[0].getContent()));
            else
                _addError("location: esperado exatamente um caminho");
            node = lc;
            break;
        }
        case ParserTokenType::PT_SERVER:
            node = _createChild<ServerConfig>("server");
            break;
        default:
            _addError("ConfigBuilderVisitor: bloco desconhecido");
            return;
    }

    _stack.push(node);

    for (size_t i = 0; i < block.children.size(); ++i)
        block.children[i]->accept(*this);

    _stack.pop();
}

void ConfigBuilderVisitor::visit(Directive& directive)
{
    if (_stack.empty())
    {
        _addError("ConfigBuilderVisitor: diretiva fora de contexto");
        return;
    }

    ConfigNode* top = _stack.top();

    if (dynamic_cast<GlobalConfig*>(top))
    {
        // contexto global (main): nao possui diretivas proprias no nosso subset
    }
    else if (EventsConfig* ec = dynamic_cast<EventsConfig*>(top))
        _handleEventsDirective(directive, ec);
    else if (HttpConfig* hc = dynamic_cast<HttpConfig*>(top))
        _handleHttpDirective(directive, hc);
    else if (ServerConfig* sc = dynamic_cast<ServerConfig*>(top))
        _handleServerDirective(directive, sc);
    else if (LocationConfig* lc = dynamic_cast<LocationConfig*>(top))
        _handleLocationDirective(directive, lc);
    else
        _addError("ConfigBuilderVisitor: diretiva '" + directive.name.getContent()
                  + "' em contexto nao reconhecido");
}


void ConfigBuilderVisitor::_handleEventsDirective(Directive& d, EventsConfig* ec)
{
    switch (d.name.getType())
    {
        case ParserTokenType::PT_WORKER_CONNECTIONS:
            ec->setWorkerConnections(std::atoi(d.values[0].getContent().c_str()));
            break;
        case ParserTokenType::PT_USE:
        {
            const std::string& val = d.values[0].getContent();
            if (val == "epoll")        ec->setUse(IO_EPOLL);
            else if (val == "kqueue")  ec->setUse(IO_KQUEUE);
            else if (val == "poll")    ec->setUse(IO_POLL);
            else                       ec->setUse(IO_SELECT);
            break;
        }
        default:
            break;
    }
}

void ConfigBuilderVisitor::_handleHttpDirective(Directive& d, HttpConfig* hc)
{
    switch (d.name.getType())
    {
        case ParserTokenType::KEEPALIVE_TIMEOUT:
            hc->setKeepaliveTimeout(std::atoi(d.values[0].getContent().c_str()));
            break;
        default:
            break;
    }
}

void ConfigBuilderVisitor::_handleServerDirective(Directive& d, ServerConfig* sc)
{
    switch (d.name.getType())
    {
        case ParserTokenType::PT_LISTEN:
        {
            ConfigServer tmp;
            ServerListenParser p(d);
            p.toConfig(tmp);
            for (size_t i = 0; i < tmp.listeners.size(); i++)
                sc->addListen(tmp.listeners[i]);
            break;
        }
        case ParserTokenType::PT_ERROR_PAGE:
            if (d.values.size() >= 2)
                sc->addErrorPage(std::atoi(d.values[0].getContent().c_str()), 
                               d.values[1].getContent());
            break;
        case ParserTokenType::PT_CLIENT_MAX_BODY_SIZE:
            sc->setClientMaxBodySize(std::atoi(d.values[0].getContent().c_str()));
            break;
        case ParserTokenType::PT_SERVER_NAME:
            for (size_t i = 0; i < d.values.size(); ++i)
                sc->addServerName(d.values[i].getContent());
            break;
        case ParserTokenType::PT_ROOT:
            sc->setRoot(Path(d.values[0].getContent()));
            break;
        default:
            break;
    }
}

void ConfigBuilderVisitor::_handleLocationDirective(Directive& d, LocationConfig* lc)
{
    switch (d.name.getType())
    {
        case ParserTokenType::PT_ROOT:
            lc->setRoot(Path(d.values[0].getContent()));
            break;
        case ParserTokenType::PT_AUTOINDEX:
            lc->setAutoindex(d.values[0].getContent() == "on");
            break;
        case ParserTokenType::PT_INDEX:
            for (size_t i = 0; i < d.values.size(); ++i)
                lc->addIndex(d.values[i].getContent());
            break;
        case ParserTokenType::PT_CGI_EXTENSION:
            lc->addCgiExtension(std::make_pair(d.values[0].getContent(), 
                                        d.values[1].getContent()));
            break;
		case ParserTokenType::PT_RETURN:
   			 if (d.values.size() == 2)
				lc->setRedirect(std::atoi(d.values[0].getContent().c_str()), d.values[1].getContent());
			else
				lc->setRedirect(0, d.values[0].getContent());
			break;
        case ParserTokenType::PT_UPLOAD_DIR:
            lc->setUploadDir(d.values[0].getContent());
            break;
        case ParserTokenType::PT_ALLOW_METHODS:
            for (size_t i = 0; i < d.values.size(); ++i)
            {
                const std::string& m = d.values[i].getContent();
                if (m == "GET")         lc->addMethod(GET);
                else if (m == "POST")   lc->addMethod(POST);
                else if (m == "DELETE") lc->addMethod(DELETE);
            }
            break;
        case ParserTokenType::PT_REQUIRE_AUTH:
            lc->setRequireAuth(d.values[0].getContent() == "on");
            break;
        default:
            break;
    }
}

WebServerConfig* ConfigBuilderVisitor::getResult() const
{
    return _result;
}
