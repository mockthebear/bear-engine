#include "../settings/definitions.hpp"

/*
    Code from otclient
*/
#ifdef __linux__

#include "../engine/gamebase.hpp"
#include <execinfo.h>
#include <ucontext.h>
#include <signal.h>

#define MAX_BACKTRACE_DEPTH 128
#define DEMANGLE_BACKTRACE_SYMBOLS


void crashHandler(int signum, siginfo_t* info, void* secret)
{
    Game::Crashed = true;
    bear::out << "Application crashed\n";

    std::stringstream ss;
    ss << utils::format("build date: %s\n", __DATE__);
    ss.flags(std::ios::hex | std::ios::showbase);

    ucontext_t context = *(ucontext_t*)secret;
#if __WORDSIZE == 64
    ss << "  at rip = " << context.uc_mcontext.gregs[REG_RIP] << std::endl;
    ss << "     rax = " << context.uc_mcontext.gregs[REG_RAX] << std::endl;
    ss << "     rbx = " << context.uc_mcontext.gregs[REG_RBX] << std::endl;
    ss << "     rcx = " << context.uc_mcontext.gregs[REG_RCX] << std::endl;
    ss << "     rdx = " << context.uc_mcontext.gregs[REG_RDX] << std::endl;
    ss << "     rsi = " << context.uc_mcontext.gregs[REG_RSI] << std::endl;
    ss << "     rdi = " << context.uc_mcontext.gregs[REG_RDI] << std::endl;
    ss << "     rbp = " << context.uc_mcontext.gregs[REG_RBP] << std::endl;
    ss << "     rsp = " << context.uc_mcontext.gregs[REG_RSP] << std::endl;
    ss << "     efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
    ss << std::endl;
#elif defined(REG_EIP)
    ss << "  at eip = " << context.uc_mcontext.gregs[REG_EIP] << std::endl;
    ss << "     eax = " << context.uc_mcontext.gregs[REG_EAX] << std::endl;
    ss << "     ebx = " << context.uc_mcontext.gregs[REG_EBX] << std::endl;
    ss << "     ecx = " << context.uc_mcontext.gregs[REG_ECX] << std::endl;
    ss << "     edx = " << context.uc_mcontext.gregs[REG_EDX] << std::endl;
    ss << "     esi = " << context.uc_mcontext.gregs[REG_ESI] << std::endl;
    ss << "     edi = " << context.uc_mcontext.gregs[REG_EDI] << std::endl;
    ss << "     ebp = " << context.uc_mcontext.gregs[REG_EBP] << std::endl;
    ss << "     esp = " << context.uc_mcontext.gregs[REG_ESP] << std::endl;
    ss << "     efl = " << context.uc_mcontext.gregs[REG_EFL] << std::endl;
    ss << std::endl;
#endif

    ss.flags(std::ios::dec);
    ss << "  backtrace:" << std::endl;

    void* buffer[MAX_BACKTRACE_DEPTH];
    int numLevels = backtrace(buffer, MAX_BACKTRACE_DEPTH);
    char **tracebackBuffer = backtrace_symbols(buffer, numLevels);
    if(tracebackBuffer) {
        for(int i = 2; i < numLevels; i++) {
            std::string line = tracebackBuffer[i];
            if(line.find("__libc_start_main") != std::string::npos)
                break;
#ifdef DEMANGLE_BACKTRACE_SYMBOLS
            std::size_t demanglePos = line.find("(_Z");
            if(demanglePos != std::string::npos) {
                demanglePos++;
                int len = std::min(line.find_first_of("+", demanglePos), line.find_first_of(")", demanglePos)) - demanglePos;
                std::string funcName = line.substr(demanglePos, len);
                line.replace(demanglePos, len, funcName.c_str());
            }
#endif
            ss << "    " << i-1 << ": " << line << std::endl;
        }
        free(tracebackBuffer);
    }

    std::cout << ss.str() << "\n";

    std::string fileName = "crash_report.log";
    std::ofstream fout(fileName.c_str(), std::ios::out | std::ios::app);
    if(fout.is_open() && fout.good()) {
        fout << "== application crashed\n";
        fout << ss.str();
        fout << "\n";
        fout.close();
        bear::out << "Crash report saved at : " <<fileName<< "\n";
    } else
        bear::out << "Error on save crash report\n";

    signal(SIGILL, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
}

void installCrashHandler()
{
    struct sigaction sa;
    sa.sa_sigaction = &crashHandler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    sigaction(SIGILL, &sa, NULL);   // illegal instruction
    sigaction(SIGSEGV, &sa, NULL);  // segmentation fault
    sigaction(SIGFPE, &sa, NULL);   // floating-point exception
    sigaction(SIGABRT, &sa, NULL);  // process aborted (asserts)

    //getchar();
}

#endif // defined
