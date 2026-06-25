#include "ConfigBuilderVisitor.hpp"
#include <cstdlib>
#include "ConfigServer.hpp"
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
            {
                lc->setPath(block.values[0].getContent());
                lc->setModifier(MOD_NONE);
            }
            else if (block.values.size() >= 2)
            {
                ParserTokenType::type mod = block.values[0].getType();
                if (mod == ParserTokenType::PT_MOD_EXACT)        lc->setModifier(MOD_EXACT);
                else if (mod == ParserTokenType::PT_MOD_PREFIX)  lc->setModifier(MOD_PREFIX);
                else if (mod == ParserTokenType::PT_MOD_REGEX)   lc->setModifier(MOD_REGEX);
                else if (mod == ParserTokenType::PT_MOD_REGEX_CI) lc->setModifier(MOD_REGEX_CI);
                else                                             lc->setModifier(MOD_NONE);
                lc->setPath(block.values[1].getContent());
            }
            node = lc;
            break;
        }
        case ParserTokenType::PT_SERVER:
            node = _createChild<ServerConfig>("server");
            break;
        case ParserTokenType::PT_UPSTREAM:
            node = _createChild<UpstreamConfig>("upstream");
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

    if (GlobalConfig* gc = dynamic_cast<GlobalConfig*>(top))
        _handleGlobalDirective(directive, gc);
    else if (EventsConfig* ec = dynamic_cast<EventsConfig*>(top))
        _handleEventsDirective(directive, ec);
    else if (HttpConfig* hc = dynamic_cast<HttpConfig*>(top))
        _handleHttpDirective(directive, hc);
    else if (ServerConfig* sc = dynamic_cast<ServerConfig*>(top))
        _handleServerDirective(directive, sc);
    else if (LocationConfig* lc = dynamic_cast<LocationConfig*>(top))
        _handleLocationDirective(directive, lc);
    else if (UpstreamConfig* uc = dynamic_cast<UpstreamConfig*>(top))
        _handleUpstreamDirective(directive, uc);
    else
        _addError("ConfigBuilderVisitor: tipo de contexto desconhecido");
}

void ConfigBuilderVisitor::_handleGlobalDirective(Directive& d, GlobalConfig* gc)
{
    switch (d.name.getType())
    {
        case ParserTokenType::PT_WORKER_PROCESSES:
            gc->setWorkers(std::atoi(d.values[0].getContent().c_str()));
            break;
        case ParserTokenType::PT_ERROR_LOG:
            gc->setErrorLog(d.values[0].getContent());
            break;
        case ParserTokenType::PT_PID:
            gc->setPid(std::atoi(d.values[0].getContent().c_str()));
            break;
        default:
            break;
    }
}

void ConfigBuilderVisitor::_handleEventsDirective(Directive& d, EventsConfig* ec)
{
    switch (d.name.getType())
    {
        case ParserTokenType::PT_WORKER_CONNECTIONS:
            ec->setWorkerConnections(std::atoi(d.values[0].getContent().c_str()));
            break;
        case ParserTokenType::MULTI_ACCEPT:
            ec->setMultiAccept(d.values[0].getContent() == "on");
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
        case ParserTokenType::PT_INCLUDE:
            hc->setInclude(d.values[0].getContent());
            break;
        case ParserTokenType::KEEPALIVE_TIMEOUT:
            hc->setKeepaliveTimeout(std::atoi(d.values[0].getContent().c_str()));
            break;
        case ParserTokenType::SENDFILE:
            hc->setSendfile(d.values[0].getContent() == "on");
            break;
        case ParserTokenType::DEFAULT_TYPE:
            hc->setDefaultType(d.values[0].getContent());
            break;
        case ParserTokenType::PT_LOG_FORMAT:
            hc->setLogFormat(d.values[0].getContent());
            break;
        case ParserTokenType::PT_ACCESS_LOG:
            hc->setAccessLog(d.values[0].getContent());
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
            sc->setRoot(d.values[0].getContent());
            break;
        case ParserTokenType::PT_RETURN:
   			 if (d.values.size() == 2)
				sc->setRedirect(std::atoi(d.values[0].getContent().c_str()), d.values[1].getContent());
			else
				sc->setRedirect(0, d.values[0].getContent());
			break;
        case ParserTokenType::PT_ACCESS_LOG:
            sc->setAccessLog(d.values[0].getContent());
            break;
        case ParserTokenType::PT_SSL_CERTIFICATE:
            sc->setSslCertificate(d.values[0].getContent());
            break;
        case ParserTokenType::PT_SSL_CERTIFICATE_KEY:
            sc->setSslCertificateKey(d.values[0].getContent());
            break;
        case ParserTokenType::PT_SSL_PROTOCOLS:
            sc->setSslProtocols(d.values[0].getContent());
            break;
        case ParserTokenType::PT_SSL_CIPHERS:
            sc->setSslCiphers(d.values[0].getContent());
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
            lc->setRoot(d.values[0].getContent());
            break;
        case ParserTokenType::PT_AUTOINDEX:
            lc->setAutoindex(d.values[0].getContent() == "on");
            break;
        case ParserTokenType::PT_INDEX:
            for (size_t i = 0; i < d.values.size(); ++i)
                lc->addIndex(d.values[i].getContent());
            break;
        case ParserTokenType::PT_FASTCGI_PASS:
            lc->addCgi(std::make_pair(std::string("fastcgi_pass"), d.values[0].getContent()));
            break;
        case ParserTokenType::PT_FASTCGI_PARAM:
            if (d.values.size() >= 2)
                lc->addCgi(std::make_pair(d.values[0].getContent(), 
                                        d.values[1].getContent()));
            break;
        case ParserTokenType::PT_FASTCGI_INDEX:
            lc->setFastcgiIndex(d.values[0].getContent());
            break;
        case ParserTokenType::PT_TRY_FILES:
            for (size_t i = 0; i < d.values.size(); ++i)
                lc->addTryFile(d.values[i].getContent());
            break;
        case ParserTokenType::PT_ADD_HEADER:
            if (d.values.size() >= 2)
                lc->addAddHeader(d.values[0].getContent(), 
                               d.values[1].getContent());
            break;
        case ParserTokenType::PT_PROXY_PASS:
            lc->setProxyPass(d.values[0].getContent());
            break;
        case ParserTokenType::PT_PROXY_SET_HEADER:
            if (d.values.size() >= 2)
                lc->addProxySetHeader(d.values[0].getContent(), 
                                     d.values[1].getContent());
            break;
        case ParserTokenType::PT_PROXY_CACHE_BYPASS:
            lc->setProxyCacheBypass(d.values[0].getContent());
            break;
        case ParserTokenType::PT_EXPIRES:
            lc->setExpires(d.values[0].getContent());
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
        default:
            break;
    }
}

void ConfigBuilderVisitor::_handleUpstreamDirective(Directive& d, UpstreamConfig* uc)
{
    switch (d.name.getType())
    {
        case ParserTokenType::PT_SERVER_DIRECTIVE:
            if (!d.values.empty())
            {
                UpstreamServer us;
                us._ip = d.values[0].getContent();
                if (d.values.size() > 1)
                    us._weight = std::atoi(d.values[1].getContent().c_str());
                uc->addServer(us);
            }
            break;
        default:
            break;
    }
}

WebServerConfig* ConfigBuilderVisitor::getResult() const
{
    return _result;
}
