#include "../settings/definitions.hpp"
#ifdef RUN_TESTS


#include "../engine/genericstate.hpp"
#include "../socket/tcp.hpp"
#pragma once

#ifndef DISABLE_SOCKET


class Test_Socket: public State{
    public:
        Test_Socket(){
            requestQuit = requestDelete = false;

        };
        ~Test_Socket(){

        };
        void Begin(){
            bear::out << "Test\n";
            duration = 100.0f;
            TcpClient tc;
            getchar();
            bear::out << tc.Connect("127.0.0.1",1337) << "\n";

            SocketMessage sk;
            //sk.SetStream("owo noticed\n");

            tc.Send(&sk);
            bear::out << "Sending\n";

            sk.Clear();
            bear::out << tc.ReceiveBytes(&sk,1024) << "\n";

            std::cout << sk.GetStream() << "\n";
            sk.Clear();
            bear::out << tc.Receive(&sk) << "\n";

            std::cout << sk.GetStream() << "\n";

            getchar();
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


#endif // DISABLE_SOCKET
#endif // RUN_TESTS
