# Campos faltando por escopo no ConfigBuilderVisitor

O ScopeValidator permite certas diretivas em múltiplos escopos, mas as config classes
só têm os campos/setters no escopo onde já são usados. Adicionar os campos abaixo
quando o servidor precisar consultar esses valores em http ou server level.

---

## HttpConfig

| Campo | Tipo | Setter | Diretiva | Motivo para adicionar |
|---|---|---|---|---|
| `_root` | `std::string` | `setRoot` | `PT_ROOT` | default de root para todos os servers |
| `_autoindex` | `bool` | `setAutoindex` | `PT_AUTOINDEX` | default de autoindex para todos os servers |
| `_index` | `std::list<std::string>` | `addIndex` | `PT_INDEX` | default de index para todos os servers |
| `_add_header` | `std::list<pair<string,string>>` | `addAddHeader` | `PT_ADD_HEADER` | headers globais |
| `_proxy_set_header` | `std::list<pair<string,string>>` | `addProxySetHeader` | `PT_PROXY_SET_HEADER` | proxy headers globais |
| `_fastcgi_index` | `std::string` | `setFastcgiIndex` | `PT_FASTCGI_INDEX` | fastcgi default |
| `_cgi` | `std::map<string,string>` | `addCgi` | `PT_FASTCGI_PARAM` | fastcgi params globais |
| `_proxy_cache_bypass` | `std::string` | `setProxyCacheBypass` | `PT_PROXY_CACHE_BYPASS` | proxy cache global |
| `_client_max_body_size` | `size_t` | `setClientMaxBodySize` | `PT_CLIENT_MAX_BODY_SIZE` | default para todos os servers |
| `_error_log` | `std::string` | `setErrorLog` | `PT_ERROR_LOG` | log de erro por http block |
| `_error_page` | `std::map<int,std::string>` | `addErrorPage` | `PT_ERROR_PAGE` | error pages globais |

## ServerConfig

| Campo | Tipo | Setter | Diretiva | Motivo para adicionar |
|---|---|---|---|---|
| `_autoindex` | `bool` | `setAutoindex` | `PT_AUTOINDEX` | autoindex default para locations |
| `_index` | `std::list<std::string>` | `addIndex` | `PT_INDEX` | index default para locations |
| `_add_header` | `std::list<pair<string,string>>` | `addAddHeader` | `PT_ADD_HEADER` | headers por server |
| `_proxy_set_header` | `std::list<pair<string,string>>` | `addProxySetHeader` | `PT_PROXY_SET_HEADER` | proxy headers por server |
| `_fastcgi_index` | `std::string` | `setFastcgiIndex` | `PT_FASTCGI_INDEX` | fastcgi default por server |
| `_cgi` | `std::map<string,string>` | `addCgi` | `PT_FASTCGI_PARAM` | fastcgi params por server |
| `_proxy_cache_bypass` | `std::string` | `setProxyCacheBypass` | `PT_PROXY_CACHE_BYPASS` | proxy cache por server |
| `_error_log` | `std::string` | `setErrorLog` | `PT_ERROR_LOG` | log de erro por server |
| `_try_files` | `std::list<std::string>` | `addTryFile` | `PT_TRY_FILES` | try_files default para locations |

## LocationConfig

| Campo | Tipo | Setter | Diretiva | Motivo para adicionar |
|---|---|---|---|---|
| `_access_log` | `std::string` | `setAccessLog` | `PT_ACCESS_LOG` | log de acesso por location |
| `_error_page` | `std::map<int,std::string>` | `addErrorPage` | `PT_ERROR_PAGE` | error pages por location |

---

## Nota sobre herança

Quando esses campos forem adicionados, será necessário implementar resolução por precedência:
location > server > http. Exemplo para `root`:

```
location /  → lc->getRoot() se não vazio
server      → sc->getRoot() se não vazio
http        → hc->getRoot() como fallback
```
