#include "scriptloader.hpp"
#include "../../framework/gamefile.hpp"
#include "../../framework/userfile.hpp"
#include <iostream>

void ScriptLoader::PullError(std::string err, GameFile &script){
    uint32_t line = script.GetLineNumber(script.Tell());
    throw BearException(utils::format("Syntax error at line %d: %s", line, err.c_str()));
}

bool ScriptLoader::CompileScript(std::string scriptFile, std::vector<uint8_t> &bytecode){
    GameFile script;
    if (!script.Open(scriptFile)){
        throw BearException("File not found.");
        return false;
    }
    script.Cache();

    while (script.Tell() < script.GetSize()){
        char c = script.ReadByte();
        ParseFirstCharacter(c, script, bytecode);
    }

    script.Close();
    return true;
}


bool ScriptLoader::CompileScriptToFile(std::string scriptFile, std::string out){
    std::vector<uint8_t> bytecode;
    GameFile script;
    if (!script.Open(scriptFile)){
        throw BearException("File not found.");
        return false;
    }
    script.Cache();

    while (script.Tell() < script.GetSize()){
        char c = script.ReadByte();
        ParseFirstCharacter(c, script, bytecode);
    }

    script.Close();

    UserFile f;

    f.Open(out);
    f.Write((char*)&bytecode[0], bytecode.size());
    f.Close();
    bytecode.clear();
    return true;
}

bool ScriptLoader::LoadBytecode(AnimationLoader &anim, std::vector<uint8_t> &bytecode){
    if (bytecode.size() == 0){
        throw BearException("Empty bytecode");
        return false;
    }
    uint32_t sz = bytecode.size();
    uint32_t strSize;
    for (uint32_t i=0;i<sz;i++){
        uint8_t byte = static_cast<uint8_t>(bytecode[i]);
        ++i;
        strSize = static_cast<uint32_t>(bytecode[i]);
        ++i;
        std::string label;
        for (uint32_t b = 0;b<strSize;b++){
            label += bytecode[i];
            ++i;
        }
        uint32_t localLabelId = anim.QuerySymbol(label);
        InstructionSet iset;

        if (byte == 0xba){

            char op = 0;
            do{
                op = bytecode[i];
                std::cout << "Its a instruction ["<<op<<"]\n";
                switch (op){
                    case 'l':{
                        if (bytecode[++i] == 0x01){
                            iset.emplace_back(AnimInstruction(OPCODE_LINE, static_cast<uint32_t>(bytecode[++i]) ));
                        }else{
                            throw BearException("Expected 0x01 in l");
                        }
                        break;
                    }
                    case 'j':{
                        uint8_t size1 = bytecode[++i];
                        std::string innerLabel1;
                        for (uint32_t b = 0;b<size1;b++){
                            innerLabel1 += (char)bytecode[++i];
                        }
                        uint32_t symb1 = anim.QuerySymbol(innerLabel1);
                        iset.emplace_back(AnimInstruction(OPCODE_JUMP, symb1));

                        break;
                    }
                    case 'n':{
                        char mode = bytecode[++i];
                        if (mode == 0x01){
                            iset.emplace_back(AnimInstruction(OPCODE_LINE, static_cast<uint32_t>(bytecode[++i]) ));
                        }else if (mode == 0x02){
                            iset.emplace_back(AnimInstruction(OPCODE_RANGE,  bytecode[i+1], bytecode[i+2] ) );
                            i+=2;
                        }else{
                            throw BearException("Expected 0x01/0x02 in n");
                        }
                        break;
                    }
                    case 'c':{
                        char mode = bytecode[++i];

                        uint8_t size1 = bytecode[++i];

                        std::string innerLabel1;
                        for (uint32_t b = 0;b<size1;b++){
                            innerLabel1 += (char)bytecode[++i];
                        }
                        uint32_t symb1 = anim.QuerySymbol(innerLabel1);

                        if (mode == 0x01){
                            iset.emplace_back(AnimInstruction(OPCODE_CALLBACK, symb1));
                        }else if (mode == 0x02){
                            uint8_t size2 = bytecode[++i];
                            std::string innerLabel2;
                            for (uint32_t b = 0;b<size2;b++){
                                innerLabel2 += (char)bytecode[++i];
                            }

                            uint32_t symb2 = anim.QuerySymbol(innerLabel2);
                            iset.emplace_back(AnimInstruction(OPCODE_CONDITIONCALLBACK, symb1));
                            iset.emplace_back(AnimInstruction(OPCODE_TOEND, symb2));
                        }else{
                            throw BearException("Expected 0x01/0x02 in c");
                        }
                        break;
                    }
                    case 'w':{
                        char mode = bytecode[++i];
                        if (mode == 0x01){
                            union __aux{
                                uint8_t c[4];
                                float f;
                            } conv;
                            conv.c[0] = bytecode[++i];
                            conv.c[1] = bytecode[++i];
                            conv.c[2] = bytecode[++i];
                            conv.c[3] = bytecode[++i];
                            iset.emplace_back(AnimInstruction(OPCODE_WAIT, conv.f ) );
                        }else{
                            throw BearException("Expected 0x01 in t");
                        }
                        break;
                    }
                    case 't':{
                        char mode = bytecode[++i];
                        if (mode == 0x01){
                            union __aux{
                                uint8_t c[4];
                                float f;
                            } conv;
                            conv.c[0] = bytecode[++i];
                            conv.c[1] = bytecode[++i];
                            conv.c[2] = bytecode[++i];
                            conv.c[3] = bytecode[++i];
                            iset.emplace_back(AnimInstruction(OPCODE_TIME, conv.f ) );
                        }else{
                            throw BearException("Expected 0x01 in t");
                        }
                        break;
                    }
                    default:{
                        throw BearException(std::string("Unknow bytecode [") + op + "]");
                        break;
                    }
                }
                i++;
                op = bytecode[i];
            }while (bytecode[i] != 0xff);
            if (bytecode[i] != 0xff){
                throw BearException(std::string("Expected 0xff at the end of set"));
            }else{
                anim.AddInstructionSet(localLabelId, iset);
            }

        }
    }
    return true;
}

bool ScriptLoader::LoadScript(std::string scriptFile, AnimationLoader &anim){
    std::vector<uint8_t> bytecode;
    if (!CompileScript(scriptFile,bytecode) || bytecode.size() == 0){
        return false;
    }
    return LoadBytecode(anim, bytecode);
}

bool ScriptLoader::SeekInstructions(GameFile &script, std::vector<uint8_t> &bytecode){
    uint16_t start = script.Tell();
    if (start == script.GetSize()){
        return false;
    }
    std::string emptyData = script.ReadUntil({'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'l', 'L', 'j', 'J', 't', 'T', 'w', 'W', 'C', 'c'});
    //std::cout << "Em["<<emptyData<<"]\n";
    CheckEmptyness(emptyData, script);
    script.Seek(script.Tell()-1);
    char op = script.ReadByte();


    switch (op){
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':{
            bytecode.emplace_back('n');
            //std::cout << "is num instruction["<<op<<"]\n";
            script.Seek(script.Tell()-1);
            uint32_t FirstNumber = script.GetNumber(false);
            script.Seek(script.Tell()-1);
            uint32_t SecondNumber = 255;
            char ck = script.ReadByte();
            //std::cout << "F number is " << FirstNumber << "and ck is" << ck << "\n";
            if (ck == '.'){
                ck = script.ReadByte();
                if (ck == '.'){
                    ck = script.ReadByte();
                    //std::cout << "Finally found "<<ck<<"\n";
                    if (IsNumber(ck)){
                        script.Seek(script.Tell()-1);
                        SecondNumber = script.GetNumber(false);

                        std::cout << "INSTRUCTION: Animate from  "<<FirstNumber<<" to "<<SecondNumber<<"\n";
                        bytecode.emplace_back(0x02);
                        bytecode.emplace_back((uint8_t)FirstNumber);
                        bytecode.emplace_back((uint8_t)SecondNumber);
                        script.Seek(script.Tell()-1);
                        return CheckNextInstruction(script);
                    }else if(ck == '.'){
                        std::cout << "INSTRUCTION: Animate from  "<<FirstNumber<<" to "<<SecondNumber<<"\n";
                        bytecode.emplace_back(0x02);
                        bytecode.emplace_back((uint8_t)FirstNumber);
                        bytecode.emplace_back((uint8_t)SecondNumber);
                        script.Seek(script.Tell()-1);
                        return CheckNextInstruction(script);
                    }else{
                        PullError(std::string("Expected Number... or Number..Number got Number..[")+ck+"]", script);
                    }
                }else{
                     PullError(std::string("Expected Number.."), script);
                }
            }else{
                script.Seek(script.Tell()-2);
                uint32_t num = script.GetNumber(false);
                //std::cout << "INSTRUCTION: Jump to frame "<<num<<"\n";
                bytecode.emplace_back(0x01);
                bytecode.emplace_back((char)num);
                script.Seek(script.Tell()-2);
                return CheckNextInstruction(script);
            }
            break;
        }
        case 'l':
        case 'L':{
            bytecode.emplace_back('l');
            char ck = script.ReadByte();
            //std::cout << "is line instruction["<<ck<<"]\n";

            if (IsNumber(ck)){
                script.Seek(script.Tell()-1);
                uint32_t num = script.GetNumber(false);
                //std::cout << "INSTRUCTION: L"<<num<<"\n";
                bytecode.emplace_back(0x01);
                bytecode.emplace_back((char)num);
                script.Seek(script.Tell()-1);
                return CheckNextInstruction(script);
            }else{
                script.Seek(script.Tell()-2);
                CheckKeyword("line", script);
                ck = script.ReadByte();
                if (IsNumber(ck)){
                    script.Seek(script.Tell()-1);
                    uint32_t num = script.GetNumber(false);
                    //std::cout << "INSTRUCTION: L"<<num<<"\n";
                    bytecode.emplace_back(0x01);
                    bytecode.emplace_back((char)num);
                    script.Seek(script.Tell()-1);
                    return CheckNextInstruction(script);
                }else{
                    PullError(std::string("Expected a number after 'line'" ), script);
                }
            }
            break;
        }
        case 'w':
        case 'W':{
            bytecode.emplace_back('w');
            char ck = script.ReadByte();
            if (IsNumber(ck)){
                script.Seek(script.Tell()-1);
                float num = script.GetFloat(false);
                bytecode.emplace_back(0x01);
                union __aux{
                    uint8_t c[4];
                    float f;
                } conv;
                conv.f = num;
                bytecode.emplace_back(conv.c[0]);
                bytecode.emplace_back(conv.c[1]);
                bytecode.emplace_back(conv.c[2]);
                bytecode.emplace_back(conv.c[3]);
                //std::cout << "INSTRUCTION: W"<<num<<"\n";
                script.Seek(script.Tell()-1);
                return CheckNextInstruction(script);
            }else{
                script.Seek(script.Tell()-2);
                CheckKeyword("wait", script);
                ck = script.ReadByte();
                if (IsNumber(ck)){
                    script.Seek(script.Tell()-1);
                    float num = script.GetFloat(false);
                    //std::cout << "INSTRUCTION: W"<<num<<"\n";
                    bytecode.emplace_back(0x01);
                    union __aux{
                        uint8_t c[4];
                        float f;
                    } conv;
                    conv.f = num;
                    bytecode.emplace_back(conv.c[0]);
                    bytecode.emplace_back(conv.c[1]);
                    bytecode.emplace_back(conv.c[2]);
                    bytecode.emplace_back(conv.c[3]);
                    script.Seek(script.Tell()-1);

                    return CheckNextInstruction(script);
                }else{
                    PullError(std::string("Expected a number after 'wait'" ), script);
                }
            }
            break;
        }
        case 't':
        case 'T':{
            bytecode.emplace_back('t');
            char ck = script.ReadByte();
            if (IsNumber(ck)){
                script.Seek(script.Tell()-1);
                float num = script.GetFloat(false);
                //std::cout << "INSTRUCTION: T"<<num<<"\n";
                bytecode.emplace_back(0x01);
                union __aux{
                    uint8_t c[4];
                    float f;
                } conv;
                conv.f = num;
                bytecode.emplace_back(conv.c[0]);
                bytecode.emplace_back(conv.c[1]);
                bytecode.emplace_back(conv.c[2]);
                bytecode.emplace_back(conv.c[3]);
                script.Seek(script.Tell()-1);
                return CheckNextInstruction(script);
            }else{
                script.Seek(script.Tell()-2);
                CheckKeyword("time", script);
                ck = script.ReadByte();
                if (IsNumber(ck)){
                    script.Seek(script.Tell()-1);
                    float num = script.GetFloat(false);
                    //std::cout << "INSTRUCTION: T"<<num<<"\n";
                    bytecode.emplace_back(0x01);
                    union __aux{
                        uint8_t c[4];
                        float f;
                    } conv;
                    conv.f = num;
                    bytecode.emplace_back(conv.c[0]);
                    bytecode.emplace_back(conv.c[1]);
                    bytecode.emplace_back(conv.c[2]);
                    bytecode.emplace_back(conv.c[3]);
                    script.Seek(script.Tell()-1);
                    return CheckNextInstruction(script);
                }else{
                    PullError(std::string("Expected a number after 'time'" ), script);
                }
            }
            break;
        }
        case 'c':
        case 'C':{
            bytecode.emplace_back('c');
            char ck = script.ReadByte();
            if (ck != '{'){
                script.Seek(script.Tell()-2);
                CheckKeyword("callback", script);
                ck = script.ReadByte();
            }
            if (ck == '{'){
                std::string identifier = script.ReadUntil('}');
                CheckKeyword(identifier, script);
                ck = script.ReadByte();
                if (ck == ':'){
                    //std::cout << "CONDITION "<<identifier<<"\n";
                    ck = script.ReadByte();
                    if (ck == '{'){
                        std::string jumpIdent = script.ReadUntil('}');
                        CheckKeyword(jumpIdent, script);
                        script.Seek(script.Tell()-1);
                        //std::cout << "INSTRUCTION: CALLBACK to "<<identifier<<" jump conditionally to "<<jumpIdent<<"\n";
                        bytecode.emplace_back(0x02);
                        bytecode.emplace_back(identifier.size());
                        bytecode.insert(bytecode.end(), identifier.begin(), identifier.end());
                        bytecode.emplace_back(jumpIdent.size());
                        bytecode.insert(bytecode.end(), jumpIdent.begin(), jumpIdent.end());
                        return CheckNextInstruction(script);

                    }else{
                        throw BearException(std::string("Expected { after :, instead got [")+ck+"]");
                        PullError(std::string("Expected {  after :, instead got [")+ck+"]", script);
                    }
                }else{
                    script.Seek(script.Tell()-1);
                    bytecode.emplace_back(0x01);
                    bytecode.emplace_back(identifier.size());
                    bytecode.insert(bytecode.end(), identifier.begin(), identifier.end());
                    //std::cout << "INSTRUCTION: CALLBACK to "<<identifier<<"\n";
                    return CheckNextInstruction(script);
                }
            }
            break;
        }
        case 'j':
        case 'J':{
            bytecode.emplace_back('j');
            char ck = script.ReadByte();
            if (ck != '{' && ck != ' ' && ck != '\t'){
                script.Seek(script.Tell()-2);
                CheckKeyword("jump", script);
                ck = script.ReadByte();
            }
            if (ck == '{' || ck == ' ' || ck == '\t'){
                std::string identifier = script.ReadUntil({'}',',','\n',' ', '\r', '\t', '\0'});
                CheckKeyword(identifier, script);
                script.Seek(script.Tell()-1);
                //std::cout << "INSTRUCTION: Jump to "<<identifier<<"\n";
                bytecode.emplace_back(identifier.size());
                bytecode.insert(bytecode.end(), identifier.begin(), identifier.end());
                return CheckNextInstruction(script);
            }else{
                PullError(std::string("Exception after jump instruction. Expected space or {, instead got [")+ck+"]", script);
            }
            break;
        }
        default:
            PullError(std::string("Unknow operator [") + op + "]", script);
            break;
    }

    return false;
}

bool ScriptLoader::CheckNextInstruction(GameFile &script){
    uint16_t start = script.Tell();

    std::string untilNext = script.ReadUntil(',');
    uint32_t commentaryEnd = 0;
    for (uint32_t i=0; i < untilNext.size(); i++){
        char c = untilNext[i];
        if (c == '#'){
            for (;untilNext[i] != '\n' && i <  untilNext.size();++i){}
            commentaryEnd = i;
            //std::cout << "Ends at["<<i<<"]\n";
        }else if (untilNext[i] == ':' && untilNext[i+1] != '{'){
            for (uint32_t ofst=commentaryEnd; ofst < untilNext.size(); ofst++){
                  if  (IsLetter(untilNext[ofst]))  {
                        script.Seek(start+ofst);
                        return false;
                  }
            }

        }
    }
    return true;
}

bool ScriptLoader::CheckKeyword(const char *check, GameFile &script){
    std::string foundId;
    char it = check[0];
    for (int i=0;it != 0; i++){
        it = check[i];
        if (it == 0)
            break;
        char c = script.ReadByte();
        foundId += c;
        if (c != it){
            PullError(std::string("Expected [")+check+"] command got instead ["+foundId+"] "+c+"!="+it, script);
        }
    }
    return true;
}
//
void ScriptLoader::ParseFirstCharacter(char c,GameFile &script, std::vector<uint8_t> &bytecode){
    if (c == '#'){
        //Comentary
        script.ReadUntil('\n');
        //std::cout << "Next is["<<script.ReadByte()<<"]";
    }else if ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
        script.Seek(script.Tell()-1);
        std::string identifier = script.ReadUntil({' ', '\n', '\t', ':'});

        //std::cout << "Instruction label: ["<<identifier<<"]\n";
        script.Seek(script.Tell()-1);
        std::string emptyData = script.ReadUntil(':');
        CheckEmptyness(emptyData, script);

        if (identifier == "ftime"){
            float num = script.GetFloat(false);
            script.Seek(script.Tell()-1);
            //std::cout << "Header ftime = " << num << "\n";
            script.ReadUntil('\n');
            bytecode.emplace_back(0xaa);
            bytecode.emplace_back(identifier.size());
            bytecode.insert(bytecode.end(), identifier.begin(), identifier.end());
            union __aux{
                uint8_t c[4];
                float f;
            } conv;
            conv.f = num;
            bytecode.emplace_back(conv.c[0]);
            bytecode.emplace_back(conv.c[1]);
            bytecode.emplace_back(conv.c[2]);
            bytecode.emplace_back(conv.c[3]);
            bytecode.emplace_back(0xff);
            return;
        }
        bytecode.emplace_back(0xba);
        bytecode.emplace_back(identifier.size());
        bytecode.insert(bytecode.end(), identifier.begin(), identifier.end());

        while (SeekInstructions(script, bytecode)){}
        bytecode.emplace_back(0xff);
        //getchar();

    }
}

bool ScriptLoader::IsLetter(char c){

    if ( (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ){
        return true;
    }
    return false;
}

bool ScriptLoader::IsNumber(char c){

    if (c >= '0' && c <= '9' ){
        return true;
    }
    return false;
}


void ScriptLoader::CheckKeyword(std::string &kw,GameFile &script){
    for (auto &c : kw){
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))){
            PullError(std::string("Invalid keyword '") + kw + "'", script);
        }
    }
}

void ScriptLoader::CheckEmptyness(std::string &kw,GameFile &script){
    for (auto &c : kw){
        if (c != ' ' && c != '\n' && c != '\t'){
            PullError(std::string("Invalid empty space on '") + kw + "'", script);
        }
    }
}
