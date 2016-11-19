#include "xml.hpp"
#include <iostream>
#include <string>


XmlNode * XmlNode::Next(){
   XmlNode * node = GetNode(nodePtr);
   nodePtr++;
   return node;
}
XmlNode * XmlNode::GetNode(int n){
    if (n < 0 || n >= nodeCount){
        return nullptr;
    }
    return Nodes[n];
}

XmlNode * XmlNode::Get(std::string name){
    for (int i=0;i<nodeCount;i++){
        if (Nodes[i] && Nodes[i]->GetName() == name){
            return Nodes[i];
        }
    }
    return nullptr;
}
Xml::Xml(){

}



XmlNode *Xml::Parse(std::string fileName){
    GameFile f;
    if (not f.Open(fileName)){
        return nullptr;
    }
    f.Cache();

    std::string data = f.GetCache();


    utils::ReadUntil(data,"<?");
    if (data.length() <= 0){
        return nullptr;
    }
    std::string Header = utils::ReadUntil(data,"?>");

    //Dispensavel
    utils::ReadUntil(data,"<");

    if (data.length() <= 0){
        return nullptr;
    }

    /*
        First parse
    */

    std::string content = utils::ReadUntil(data,">");
    std::string mouduleName = utils::ReadWord(content,0,' ');
    XmlNode *d = new XmlNode();
    d->Nodes = new XmlNode*[1];
    d->Nodes[0] = nullptr;
    d->nodeCount = 0;
    d->Header = Header;
    while (content.length() > 0){
        std::string baseName = utils::ReadUntil(content,'=');
        utils::TrimString(baseName,' ');
        utils::ReadUntil(content,'"');
        std::string nodeContent = utils::ReadUntil(content,'"');
        utils::ReadUntil(content,' ');
        d->Data[baseName] = nodeContent;
    }
    d->Name = mouduleName;
    std::string dataFromWithin = utils::ReadUntil(data,"</"+mouduleName+">");
    ParsePartial(d,dataFromWithin);

    return d;
}

bool Xml::IsXmlString(std::string str){
    for (uint32_t i=0;i<str.length();i++){
        char c = str[i];
        if (c == '<'){
            if (str[i+1] != '!'){
                return true;
            }else{
                return false;
            }
        }
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')){
            return false;
        }
    }
    return false;
}

void Xml::ParsePartial(XmlNode *d,std::string content){
    while (content.length() > 0){
        XmlNode *aux = new XmlNode();
        aux->Nodes = new XmlNode*[1];
        aux->Nodes[0] = nullptr;
        aux->nodeCount = 0;
        utils::ReadUntil(content,"<");
        std::string mouduleName = utils::ReadWord(content,0,' ');
        std::string internalData = utils::ReadUntil(content,">");
        bool justATag = false;
        if (internalData[internalData.length()-1] == '/' || mouduleName.length() == 0){
            justATag = true;
        }
        while (internalData.length() > 0){
            std::string baseName = utils::ReadUntil(internalData,'=');
            utils::TrimString(baseName,' ');
            utils::ReadUntil(internalData,'"');
            std::string nodeContent = utils::ReadUntil(internalData,'"');
            utils::ReadUntil(internalData,' ');
            if (baseName.length() > 0){
                aux->Data[baseName] = nodeContent;

            }
        }
        if (!justATag){
            std::string dataFromWithin = utils::ReadUntil(content,"</"+mouduleName+">");
            if (IsXmlString(dataFromWithin)){
                ParsePartial(aux,dataFromWithin);
            }else{
                aux->Value = dataFromWithin;
            }
        }
        if (mouduleName.length() > 0){
            aux->Name =  mouduleName;
            XmlNode **reallocAux = new XmlNode*[d->nodeCount+1];
            for (int i=0;i<d->nodeCount;i++){
                reallocAux[i] = d->Nodes[i];
            }
            reallocAux[d->nodeCount] = aux;
            aux->index = d->nodeCount;
            d->nodeCount++;
            delete d->Nodes;
            d->Nodes = reallocAux;
        }else{
            delete aux;
            break;
        }
    }
}
