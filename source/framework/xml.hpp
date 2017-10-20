#pragma once

#include "gamefile.hpp"
#include "utils.hpp"
#include <map>

/** \brief Basic XML reader
 * Simple reading of this XML:
 @code
 <?xml version="1.0" encoding="UTF-8"?>
<species name="bear">
	<leg value="huge"/>
	<adorable value="1"/>
</species>
 @endcode
 * This code get all this data:
 @code
    XmlNode *file = Xml::Parse("file.xml"); //Or  asset:file.xml
    if (file){
        std::string species = file->GetName(); // species
        std::string tagName = file->Info("name"); // bear
        XmlNode * node = file->Get("leg"); //<leg size="huge"/>
        if (node){
            std::string tag = file->GetName(); // leg
            std::string size = file->Info("value"); // huge
        }

        for (auto &it : *file){
            //Iteration for each member
            bear::out << it->GetName(); << "\n";
            bear::out << it->Info("value"); << "\n";
        }
        //Output:
        // leg - huge
        // adorable - 1

        // This will delete all nodes. So becareful
        delete file;
    }
 @endcode
 */


class XmlNode{
    public:
        ~XmlNode();
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
