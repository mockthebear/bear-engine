#pragma once
#include "animationscript.hpp"
class GameFile;



class ScriptLoader{
    public:
        static bool LoadScript(std::string scriptFile, AnimationScript &anim);
        static bool LoadCompiledScript(std::string scriptFile, AnimationScript &anim);
        static bool CompileScript(std::string scriptFile, std::vector<uint8_t> &bytecode);
        static bool CompileScriptToFile(std::string scriptFile, std::string out);
        static bool LoadBytecode(AnimationScript &anim, std::vector<uint8_t> &bytecode);

    private:
        static void ParseFirstCharacter(char c,GameFile &script, std::vector<uint8_t> &bytecode, std::map<std::string, int> &labelList);
        static void CheckKeyword(std::string &kw,GameFile &script);
        static void CheckEmptyness(std::string &kw,GameFile &script);
        static bool SeekInstructions(GameFile &script, std::vector<uint8_t> &bytecode, std::map<std::string, int> &labelList);
        static bool FoundHeader(GameFile &script, std::string &identifier, std::vector<uint8_t> &bytecode);

        static bool IsNumber(char c);
        static bool IsLetter(char c);
        static bool IsSpacing(char c);

        static bool CheckKeyword(const char *kw, GameFile &script);

        static bool CheckNextInstruction( GameFile &script,  std::map<std::string, int> &labelList);

        static void PullError(std::string, GameFile &script);

        //Builder
        static void ParseCallBack(GameFile &script, std::vector<uint8_t> &bytecode, std::map<std::string, int> &labelList);
        static void ParseJump(GameFile &script, std::vector<uint8_t> &bytecode, std::map<std::string, int> &labelList);

        static void checkSymbols(std::map<std::string, int> &labelList);
        static void skipSpaces(GameFile &script);

        static bool IsHeader(std::string str);
        static void ParseSymbols(GameFile &script, std::map<std::string, int> &labelList);
        static std::string FormatIdentifier(std::string identifier);
};
