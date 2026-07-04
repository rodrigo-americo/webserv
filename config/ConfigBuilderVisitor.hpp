#ifndef CONFIGBUILDERVISITOR_HPP
# define CONFIGBUILDERVISITOR_HPP

# include <stack>
# include <vector>
# include "WebServerConfig.hpp"
# include "ParserVisitorBase.hpp"
# include "ParserComposite.hpp"

class ConfigBuilderVisitor : public ParserVisitorBase {
	private:
		ConfigBuilderVisitor(const ConfigBuilderVisitor& other);
		ConfigBuilderVisitor& operator=(const ConfigBuilderVisitor& other);
		std::stack<ConfigNode*>  _stack;
		WebServerConfig*         _result;
		std::vector<std::string> _pendingErrors;
		void				_addError(const std::string& msg);
		template<typename T>
		T* _createChild(const std::string& blockName) {
			T* child = new T();
			if (!_stack.empty()) {
				ConfigContainer* parent = dynamic_cast<ConfigContainer*>(_stack.top());
				if (parent)
					parent->addChild(child);
				else
					_addError("bloco " + blockName + " em contexto invalido");
			}
			return child;
		}
		void _handleEventsDirective(Directive& d, EventsConfig* ec);
		void _handleHttpDirective(Directive& d, HttpConfig* hc);
		void _handleServerDirective(Directive& d, ServerConfig* sc);
		void _handleLocationDirective(Directive& d, LocationConfig* lc);
	public:
		ConfigBuilderVisitor(): _result(NULL){};
		~ConfigBuilderVisitor();

		void visit(Block& block);
		void visit(Directive& directive);

		WebServerConfig* getResult() const;
};

#endif
