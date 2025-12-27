#include <windows.h>
#include <iostream>
DWORD WINAPI threadFunc(LPVOID lpParam)
{
    const char *msg = (const char *)lpParam;
    for (int i = 0; i < 5; i++)
    {
        std::cout << msg << " running...\n";
        Sleep(400);
    }
    return 0;
}
int main()
{
    HANDLE t1 = CreateThread(NULL, 0, threadFunc, (LPVOID) "Thread A", 0, NULL);
    HANDLE t2 = CreateThread(NULL, 0, threadFunc, (LPVOID) "Thread B", 0, NULL);
    WaitForSingleObject(t1, INFINITE);

    WaitForSingleObject(t2, INFINITE);
    CloseHandle(t1);
    CloseHandle(t2);
    std::cout << "Main thread finished.\n";
    return 0;
}