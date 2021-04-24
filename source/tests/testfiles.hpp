#include "../settings/definitions.hpp"
#ifndef DISABLE_UNIT_TESTS



#include "../engine/genericstate.hpp"
#pragma once

#include "../game/state.hpp"
#include "../performance/console.hpp"

#include "../engine/timer.hpp"
#include "../framework/resourcemanager.hpp"
#include "../framework/gamefile.hpp"
#include "../framework/userfile.hpp"
#include "../framework/debughelper.hpp"




class Test_Files: public State{
    public:
        Test_Files(){
            ScreenManager::GetInstance().SetScreenName("Test file");
            requestQuit = requestDelete = false;

        };
        ~Test_Files(){

        };
        void Begin(){

            bear::out << "Loading a simple file.\n";
            duration = 10.0f;
            GameFile f;
            f.Open("teste.txt",true);
            if (f.IsOpen()){
                bear::out << "File open.\n";
                uint32_t size = f.GetSize();
                bear::out << "File size: " << size << "\n";
                std::string line;
                f.GetLine(line);
                Bearssertc(line == "Hello text","Expected 'Hello text'");
                f.GetLine(line);
                Bearssert(line == "message");
                f.GetLine(line);
                Bearssert(line == "end");
                f.Close();
            }else{
                bear::out << "Failed.\n";

            }
            bear::out << "Loading a simple binary file.\n";
            f.Open("teste.bin",true);
            if (f.IsOpen()){
                uint32_t count = f.Read8();
                Bearssertc(count == 255,utils::format("Expected 255 got %d",count));
                count = f.Read16();
                Bearssertc(count == 25087,utils::format("Expected 25087 got %d",count));
                count = f.Read32();
                Bearssertc(count == 1671979617,utils::format("Expected 1671979617 got %d",count));
                int n = f.ReadByte();
                Bearssertc(n == -1,utils::format("Expected -1 got %d",n));
                char ch = f.ReadByte();
                Bearssertc(ch == 'a',utils::format("Expected char a got %c",ch));
                f.Close();
            }else{
                bear::out << "Failed.\n";

            }

            bear::out << "Loading a .burr file\n";
            ResourceFile rf;
            rf.Open("test.burr");
            for (auto &it : rf.ListFiles()){
                bear::out << "--> "<< it << "\n";
            }

            bear::out << "Requesting an file from test.burr:\n";
            SDL_RWops *fp = rf.GetFile("jaaj.txt");
            if (fp){
                f.SetFilePointer(fp);
                std::string msg = f.ReadWord(1);
                Bearssertc(msg == "esse texto",msg);
                f.Close();
            }else{
                bear::out << "Failed.\n";

            }
            rf.Close();
            bear::out << "Opening basefile enginedata.burr\n";
            if (!ResourceManager::GetInstance().Load("engine/enginedata.burr","engine")){
                Console::GetInstance().AddTextInfo("engine/enginedata.burr missing!!!");

            }
            bear::out << "Opening basefile ui.burr\n";
            if (!ResourceManager::GetInstance().Load("engine/ui.burr","ui")){
                Console::GetInstance().AddTextInfo("engine/ui.burr missing!!!");

            }


            bear::out << "Writing an file.\n";

            UserFile wrt;
            if (wrt.Open("writeTest.txt")){
                bear::out << "Writing on : " << wrt.GetFilePath() << "\n";
                wrt.Printf("Hi. Test :D");
                wrt.Close();
                bear::out << "Opening our file.\n";
                if (f.Open("writeTest.txt",true)){
                    bear::out <<"Our file contains:"<< f.Read(-1) << "\n";
                    f.Close();

                }else{
                    bear::out << "Failed to write the file.\n";

                }

            }else{
                bear::out << "Error on writing an file.\n";

            }

            bear::out << "Completed.\n";


        };



        void Update(float dt){
            duration -= dt;
            if( InputManager::GetInstance().IsAnyKeyPressed() != -1 || duration <= 0 ) {
                requestDelete = true;
            }

        };
        void Render(){

        };
        void Input();
        void Resume(){};
        void End(){};
    private:
        float duration;
};

#endif // DISABLE_UNIT_TESTS

