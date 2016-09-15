#pragma once

#include "gamefile.hpp"
#include "utils.hpp"
#include <map>

class XmlNode{
    public:
        XmlNode(){nodeCount=0;nodePtr=0;index=0;};
        std::string GetName(){return Name;};
        std::string GetValue(){return Value;};
        std::string Info(std::string Info){return Data[Info];};
        XmlNode * Next();
        XmlNode * GetNode(int n);
        int GetCount(){return nodeCount;};
        int GetIndex(){return index;};
        /**
            Used to be the iterator on c++11
        */
        XmlNode **begin(){
            return &Nodes[0];
        }
        /**
            Used to be the iterator on c++11
        */
        XmlNode **end(){
            return &Nodes[nodeCount];
        }

    private:
        std::map<std::string,std::string> Data;
        std::string Name;
        std::string Value;
        friend class Xml;
        XmlNode **Nodes;
        int index;
        int nodeCount;
        int nodePtr;
};

class Xml{
    public:
        Xml();
        XmlNode *Parse(std::string fileName);

    private:
        void ParsePartial(XmlNode *d,std::string str);
        bool IsXmlString(std::string str);
        friend class XmlNode;
        GameFile f;
};
