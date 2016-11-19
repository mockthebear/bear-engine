#pragma once

#include "gamefile.hpp"
#include "utils.hpp"
#include <map>

class XmlNode{
    public:
        XmlNode(){nodeCount=0;nodePtr=0;index=0;Nodes=nullptr;};
        std::string GetName(){return Name;};
        std::string GetValue(){return Value;};
        std::string Info(std::string Info){return Data[Info];};
        int InfoInt(std::string Info){
            return utils::GetNumber(Data[Info]);
        };
        XmlNode * Get(std::string name);

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
        std::string Header;
        friend class Xml;
        XmlNode **Nodes;
        int index;
        int nodeCount;
        int nodePtr;
};

class Xml{
    public:
        Xml();
        static XmlNode *Parse(std::string fileName);

    private:
        static void ParsePartial(XmlNode *d,std::string str);
        static bool IsXmlString(std::string str);
        friend class XmlNode;
};
