#ifndef CONFIGBUILDERVISITOR_HPP
# define CONFIGBUILDERVISITOR_HPP

# include <stack>
# include "WebServerConfig.hpp"
# include "ParserVisitorBase.hpp"
# include "ParserComposite.hpp"

class ConfigBuilderVisitor : public ParserVisitorBase {
    private:
        ConfigBuilderVisitor(const ConfigBuilderVisitor& other);
        ConfigBuilderVisitor& operator=(const ConfigBuilderVisitor& other);
        std::stack<ConfigNode*> _stack;
        WebServerConfig*        _result;
        void                    _addError(const std::string& msg);
    public:
        ConfigBuilderVisitor(): _result(NULL){};
        ~ConfigBuilderVisitor();

        void visit(Block& block);
        void visit(Directive& directive);

        WebServerConfig* getResult() const;
};

#endif
