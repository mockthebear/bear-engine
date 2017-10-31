#include "../settings/definitions.hpp"

/*
    Code from otclient
*/
#if defined(_WIN32) && defined(CRASH_HANDLER)

#include "crashhandler.hpp"
#include "../engine/gamebase.hpp"
#include "../framework/utils.hpp"
#include <windows.h>
#include <imagehlp.h>
#include <sstream>
#include <fstream>

const char *getExceptionName(DWORD exceptionCode)
{
    switch (exceptionCode) {
        case EXCEPTION_ACCESS_VIOLATION:         return "Access violation";
        case EXCEPTION_DATATYPE_MISALIGNMENT:    return "Datatype misalignment";
        case EXCEPTION_BREAKPOINT:               return "Breakpoint";
        case EXCEPTION_SINGLE_STEP:              return "Single step";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "Array bounds exceeded";
        case EXCEPTION_FLT_DENORMAL_OPERAND:     return "Float denormal operand";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "Float divide by zero";
        case EXCEPTION_FLT_INEXACT_RESULT:       return "Float inexact result";
        case EXCEPTION_FLT_INVALID_OPERATION:    return "Float invalid operation";
        case EXCEPTION_FLT_OVERFLOW:             return "Float overflow";
        case EXCEPTION_FLT_STACK_CHECK:          return "Float stack check";
        case EXCEPTION_FLT_UNDERFLOW:            return "Float underflow";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "Integer divide by zero";
        case EXCEPTION_INT_OVERFLOW:             return "Integer overflow";
        case EXCEPTION_PRIV_INSTRUCTION:         return "Privileged instruction";
        case EXCEPTION_IN_PAGE_ERROR:            return "In page error";
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "Illegal instruction";
        case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "Noncontinuable exception";
        case EXCEPTION_STACK_OVERFLOW:           return "Stack overflow";
        case EXCEPTION_INVALID_DISPOSITION:      return "Invalid disposition";
        case EXCEPTION_GUARD_PAGE:               return "Guard page";
        case EXCEPTION_INVALID_HANDLE:           return "Invalid handle";
    }
    return "Unknown exception";
}

void Stacktrace(LPEXCEPTION_POINTERS e, std::stringstream& ss)
{
    PIMAGEHLP_SYMBOL pSym;
    STACKFRAME sf;
    HANDLE process, thread;
    ULONG_PTR dwModBase, Disp;
    BOOL more = FALSE;
    DWORD machineType;
    int count = 0;
    char modname[MAX_PATH];
    char symBuffer[sizeof(IMAGEHLP_SYMBOL) + 255];

    pSym = (PIMAGEHLP_SYMBOL)symBuffer;

    ZeroMemory(&sf, sizeof(sf));
#ifdef _WIN64
    sf.AddrPC.Offset = e->ContextRecord->Rip;
    sf.AddrStack.Offset = e->ContextRecord->Rsp;
    sf.AddrFrame.Offset = e->ContextRecord->Rbp;
    machineType = IMAGE_FILE_MACHINE_AMD64;
#else
    sf.AddrPC.Offset = e->ContextRecord->Eip;
    sf.AddrStack.Offset = e->ContextRecord->Esp;
    sf.AddrFrame.Offset = e->ContextRecord->Ebp;
    machineType = IMAGE_FILE_MACHINE_I386;
#endif

    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrStack.Mode = AddrModeFlat;
    sf.AddrFrame.Mode = AddrModeFlat;

    process = GetCurrentProcess();
    thread = GetCurrentThread();

    while(1) {
        more = StackWalk(machineType,  process, thread, &sf, e->ContextRecord, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL);
        if(!more || sf.AddrFrame.Offset == 0)
            break;

        dwModBase = SymGetModuleBase(process, sf.AddrPC.Offset);
        if(dwModBase)
            GetModuleFileName((HINSTANCE)dwModBase, modname, MAX_PATH);
        else
            strcpy(modname, "Unknown");

        Disp = 0;
        pSym->SizeOfStruct = sizeof(symBuffer);
        pSym->MaxNameLength = 254;

        if(SymGetSymFromAddr(process, sf.AddrPC.Offset, &Disp, pSym))
            ss << utils::format("    %d: %s(%s+%#0lx) [0x%016lX]\n", count, modname, pSym->Name, Disp, sf.AddrPC.Offset);
        else
            ss << utils::format("    %d: %s [0x%016lX]\n", count, modname, sf.AddrPC.Offset);
        ++count;
    }
    GlobalFree(pSym);
}

LONG CALLBACK ExceptionHandler(LPEXCEPTION_POINTERS e)
{
    Game::Crashed = true;
    // generate crash report
    SymInitialize(GetCurrentProcess(), 0, TRUE);
    std::stringstream ss;
    ss << "== application crashed\n";
    ss << utils::format("build date: %s\n", __DATE__);
    ss << utils::format("exception: %s (0x%08lx)\n", getExceptionName(e->ExceptionRecord->ExceptionCode), e->ExceptionRecord->ExceptionCode);
    ss << utils::format("exception address: 0x%08lx\n", (size_t)e->ExceptionRecord->ExceptionAddress);
    ss << utils::format("  backtrace:\n");
    Stacktrace(e, ss);
    ss << "\n";
    SymCleanup(GetCurrentProcess());

    std::cout << ss.str() << "\n";

    char dir[MAX_PATH];
    GetCurrentDirectory(sizeof(dir) - 1, dir);
    std::string fileName = utils::format("%s\\crashreport.log", dir);
    std::ofstream fout(fileName.c_str(), std::ios::out | std::ios::app);
    if(fout.is_open() && fout.good()) {
        fout << ss.str();
        fout.close();
        std::cout <<(utils::format("Crash report saved to file %s\n", fileName));
    } else
       std::cout <<"Failed to save crash report!\n";

    // inform the user
    std::string msg = utils::format(
        "The application has crashed.\n\n"
        "A crash report has been written to:\n"
        "%s", fileName.c_str());
    MessageBox(NULL, msg.c_str(), "Application crashed", 0);

    // this seems to silently close the application
    //return EXCEPTION_EXECUTE_HANDLER;

    // this triggers the microsoft "application has crashed" error dialog
    return EXCEPTION_CONTINUE_SEARCH;
}

void installCrashHandler()
{
    SetUnhandledExceptionFilter(ExceptionHandler);
}

#endif
