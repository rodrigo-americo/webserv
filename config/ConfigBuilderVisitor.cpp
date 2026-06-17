#include "ConfigBuilderVisitor.hpp"
#include <cstdlib>
#include "ConfigServer.hpp"
#include "ServerListenParser.hpp"


ConfigBuilderVisitor::~ConfigBuilderVisitor() {}

void ConfigBuilderVisitor::_addError(const std::string& msg)
{
    if (_result)
        _result->addError(msg);
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
            node = gc;
            break;
        }
        case ParserTokenType::PT_HTTP:
        {
            HttpConfig* http = new HttpConfig();
            if (!_stack.empty())
                dynamic_cast<ConfigContainer*>(_stack.top())->addChild(http);
            node = http;
            break;
        }
        case ParserTokenType::PT_EVENTS:
        {
            EventsConfig* ec = new EventsConfig();
            if (!_stack.empty())
                dynamic_cast<ConfigContainer*>(_stack.top())->addChild(ec);
            node = ec;
            break;
        }
        case ParserTokenType::PT_LOCATION:
        {
            LocationConfig* lc = new LocationConfig();
            // values[0] may be a modifier token or the path; values[1] is path if modifier present
            if (block.values.size() == 1)
            {
                lc->setPath(block.values[0].getContent());
                lc->setModifier(MOD_NONE);
            }
            else if (block.values.size() >= 2)
            {
                ParserTokenType::type mod = block.values[0].getType();
                if (mod == ParserTokenType::PT_MOD_EXACT)       lc->setModifier(MOD_EXACT);
                else if (mod == ParserTokenType::PT_MOD_PREFIX) lc->setModifier(MOD_PREFIX);
                else if (mod == ParserTokenType::PT_MOD_REGEX)    lc->setModifier(MOD_REGEX);
                else if (mod == ParserTokenType::PT_MOD_REGEX_CI) lc->setModifier(MOD_REGEX_CI);
                else                           lc->setModifier(MOD_NONE);
                lc->setPath(block.values[1].getContent());
            }
            if (!_stack.empty())
                dynamic_cast<ConfigContainer*>(_stack.top())->addChild(lc);
            node = lc;
            break;
        }
        case ParserTokenType::PT_SERVER:
        {
            ServerConfig* serconfig = new ServerConfig();
            if (!_stack.empty())
                dynamic_cast<ConfigContainer*>(_stack.top())->addChild(serconfig);
            node = serconfig;
            break;
        }
        case ParserTokenType::PT_UPSTREAM:
        {
            UpstreamConfig* upconfig = new UpstreamConfig();
            if (!_stack.empty())
                dynamic_cast<ConfigContainer*>(_stack.top())->addChild(upconfig);
            node = upconfig;
            break;
        }
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

    switch (directive.name.getType())
    {
        // GlobalConfig
        case ParserTokenType::PT_WORKER_PROCESSES:
        {
            GlobalConfig* gc = dynamic_cast<GlobalConfig*>(top);
            if (gc)
                gc->setWorkers(std::atoi(directive.values[0].getContent().c_str()));
            break;
        }
        case ParserTokenType::PT_ERROR_LOG:
        {
            GlobalConfig* gc = dynamic_cast<GlobalConfig*>(top);
            if (gc)
                gc->setErrorLog(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_PID:
        {
            GlobalConfig* gc = dynamic_cast<GlobalConfig*>(top);
            if (gc)
                gc->setPid(std::atoi(directive.values[0].getContent().c_str()));
            break;
        }

        // EventsConfig
        case ParserTokenType::PT_WORKER_CONNECTIONS:
        {
            EventsConfig* ec = dynamic_cast<EventsConfig*>(top);
            if (ec)
                ec->setWorkerConnections(std::atoi(directive.values[0].getContent().c_str()));
            break;
        }
        case ParserTokenType::MULTI_ACCEPT:
        {
            EventsConfig* ec = dynamic_cast<EventsConfig*>(top);
            if (ec)
                ec->setMultiAccept(directive.values[0].getType() == ParserTokenType::PT_BOOL_ON);
            break;
        }
        case ParserTokenType::PT_USE:
        {
            EventsConfig* ec = dynamic_cast<EventsConfig*>(top);
            if (ec)
            {
                const std::string& val = directive.values[0].getContent();
                if (val == "epoll")        ec->setUse(IO_EPOLL);
                else if (val == "kqueue")  ec->setUse(IO_KQUEUE);
                else if (val == "poll")    ec->setUse(IO_POLL);
                else                       ec->setUse(IO_SELECT);
            }
            break;
        }

        // HttpConfig
        case ParserTokenType::PT_INCLUDE:
        {
            HttpConfig* hc = dynamic_cast<HttpConfig*>(top);
            if (hc)
                hc->setInclude(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::KEEPALIVE_TIMEOUT:
        {
            HttpConfig* hc = dynamic_cast<HttpConfig*>(top);
            if (hc)
                hc->setKeepaliveTimeout(std::atoi(directive.values[0].getContent().c_str()));
            break;
        }

        // ServerConfig
        case ParserTokenType::PT_LISTEN: {
		ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
		if (sc) {
			ConfigServer tmp;
			ServerListenParser p(directive);
			p.toConfig(tmp);
			for (size_t i = 0; i < tmp.listeners.size(); i++)
				sc->addListen(tmp.listeners[i]);
		}
		break;
		}
        case ParserTokenType::PT_ERROR_PAGE:
        {
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc && directive.values.size() >= 2)
                sc->addErrorPage(std::atoi(directive.values[0].getContent().c_str()), directive.values[1].getContent());
            break;
        }
        case ParserTokenType::PT_CLIENT_MAX_BODY_SIZE:
        {
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc)
                sc->setClientMaxBodySize(std::atoi(directive.values[0].getContent().c_str()));
            break;
        }
        case ParserTokenType::PT_SERVER_NAME:
        {
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc)
            {
                for (size_t i = 0; i < directive.values.size(); ++i)
                    sc->addServerName(directive.values[i].getContent());
            }
            break;
        }

        // LocationConfig
        case ParserTokenType::PT_ROOT:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
                lc->setRoot(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_AUTOINDEX:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
                lc->setAutoindex(directive.values[0].getType() == ParserTokenType::PT_BOOL_ON);
            break;
        }
        case ParserTokenType::PT_INDEX:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
            {
                for (size_t i = 0; i < directive.values.size(); ++i)
                    lc->addIndex(directive.values[i].getContent());
            }
            break;
        }
        case ParserTokenType::PT_RETURN:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
                lc->setRedirect(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_FASTCGI_PASS:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
                lc->addCgi(std::make_pair(std::string("fastcgi_pass"), directive.values[0].getContent()));
            break;
        }
        case ParserTokenType::PT_FASTCGI_PARAM:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc && directive.values.size() >= 2)
                lc->addCgi(std::make_pair(directive.values[0].getContent(), directive.values[1].getContent()));
            break;
        }
        case ParserTokenType::PT_FASTCGI_INDEX:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
                lc->setFastcgiIndex(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_TRY_FILES:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
            {
                for (size_t i = 0; i < directive.values.size(); ++i)
                    lc->addTryFile(directive.values[i].getContent());
            }
            break;
        }
        case ParserTokenType::PT_ADD_HEADER:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc && directive.values.size() >= 2)
                lc->addAddHeader(directive.values[0].getContent(), directive.values[1].getContent());
            break;
        }
        case ParserTokenType::PT_PROXY_PASS:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
                lc->setProxyPass(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_PROXY_SET_HEADER:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc && directive.values.size() >= 2)
                lc->addProxySetHeader(directive.values[0].getContent(), directive.values[1].getContent());
            break;
        }
        case ParserTokenType::PT_PROXY_CACHE_BYPASS:
        {
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc)
                lc->setProxyCacheBypass(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_EXPIRES:
        {
            // pode aparecer tanto em http quanto em location
            LocationConfig* lc = dynamic_cast<LocationConfig*>(top);
            if (lc) { lc->setExpires(directive.values[0].getContent()); break; }
            HttpConfig* hc = dynamic_cast<HttpConfig*>(top);
            if (hc) hc->setExpires(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::SENDFILE:
        {
            HttpConfig* hc = dynamic_cast<HttpConfig*>(top);
            if (hc)
                hc->setSendfile(directive.values[0].getType() == ParserTokenType::PT_BOOL_ON);
            break;
        }
        case ParserTokenType::DEFAULT_TYPE:
        {
            HttpConfig* hc = dynamic_cast<HttpConfig*>(top);
            if (hc)
                hc->setDefaultType(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_LOG_FORMAT:
        {
            HttpConfig* hc = dynamic_cast<HttpConfig*>(top);
            if (hc)
                hc->setLogFormat(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_ACCESS_LOG:
        {
            // pode aparecer em http ou server
            HttpConfig* hc = dynamic_cast<HttpConfig*>(top);
            if (hc) { hc->setAccessLog(directive.values[0].getContent()); break; }
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc) sc->setAccessLog(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_SSL_CERTIFICATE:
        {
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc)
                sc->setSslCertificate(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_SSL_CERTIFICATE_KEY:
        {
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc)
                sc->setSslCertificateKey(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_SSL_PROTOCOLS:
        {
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc)
                sc->setSslProtocols(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_SSL_CIPHERS:
        {
            ServerConfig* sc = dynamic_cast<ServerConfig*>(top);
            if (sc)
                sc->setSslCiphers(directive.values[0].getContent());
            break;
        }
        case ParserTokenType::PT_SERVER_DIRECTIVE:
        {
            UpstreamConfig* uc = dynamic_cast<UpstreamConfig*>(top);
            if (uc && !directive.values.empty())
            {
                UpstreamServer us;
                us._ip = directive.values[0].getContent();
                if (directive.values.size() > 1)
                    us._weight = std::atoi(directive.values[1].getContent().c_str());
                uc->addServer(us);
            }
            break;
        }

        default:
            break;
    }
}

WebServerConfig* ConfigBuilderVisitor::getResult() const
{
    return _result;
}
