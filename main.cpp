#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "Engine.h"

using std::wstring;

using namespace irr;
using namespace irr::core;

wchar_t* getWideCharString(char* str);

#ifdef WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char** argv)
#endif
{
    // Parse commandline to check if a filename argument has been passed
#ifdef WIN32
    int argc;
    char** argv;

    LPWSTR* args;
    args = CommandLineToArgvW(GetCommandLineW(), &argc);

    argv = (char**)malloc(sizeof(char*) * argc);
    for (int index = 0; index < argc; index++) {
        int argumentBufferLength = wcslen(args[index]) + 1;
        argv[index] = (char*)malloc(sizeof(char) * argumentBufferLength);
        sprintf_s(argv[index], argumentBufferLength, "%ws", args[index]);
    }

    LocalFree(args);
#endif

    Engine* engine = new Engine();
    if (argc >= 2) {
        for (int i = 1; i < argc; i++) {
            wchar_t* optionCS = getWideCharString(argv[i]);
            if ((strlen(argv[i]) >=2 ) && (argv[i][0] == '-') && (argv[i][1] == '-')) {
                std::cerr << "using option: " << argv[i] << std::endl;
                engine->pushOption(wstring(optionCS));
            }
            else {
                std::cerr << "loading mesh: " << argv[i] << std::endl;
                engine->loadMesh(wstring(optionCS), false);  // false: do not add to recent, since is loaded via CLI
                // TODO: ^ Add to recent anyway?
            }
            free(optionCS);
        }
    }
    //else
    //    engine->loadMesh(L"test.b3d");

    engine->run();

    delete engine;

#ifdef WIN32
    for (int index = 0; index < argc; index++)
        free(argv[index]);
    free(argv);
#endif
}

wchar_t* getWideCharString(char* str)
{
    wchar_t* dest = static_cast<wchar_t*>(
        malloc(sizeof(wchar_t) * (strlen(str) + 1))
    );

    size_t resultSize = mbstowcs(nullptr, str, strlen(str));
    mbstowcs(dest, str, strlen(str));

    dest[resultSize] = '\0';

    return dest;
}
