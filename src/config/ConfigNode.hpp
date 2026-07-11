#ifndef CONFIGNODE_HPP
# define CONFIGNODE_HPP

# include <string>
# include "has_name.hpp"
# include "has_errors.hpp"

class ConfigNode: public segregation::has_name, public segregation::has_errors{
    private:
        ConfigNode(const ConfigNode &other);
        ConfigNode &operator=(const ConfigNode &other);
    public:
        ConfigNode() {}
        virtual ~ConfigNode(){};
        void addError(const std::string& msg) { _collect(segregation::error("", msg)); }
};

class ConfigContainer : public ConfigNode
{
    private:
        ConfigContainer(const ConfigContainer &other);
        ConfigContainer &operator=(const ConfigContainer &other);
    public:
        ConfigContainer() {}
        virtual void addChild(ConfigNode* child) = 0;
        virtual ~ConfigContainer(){};
};

class ConfigLeaf: public ConfigNode{
    private:
        ConfigLeaf(const ConfigLeaf &other);
        ConfigLeaf &operator=(const ConfigLeaf &other);
    public:
        ConfigLeaf() {}
        virtual ~ConfigLeaf(){};
};

#endif
