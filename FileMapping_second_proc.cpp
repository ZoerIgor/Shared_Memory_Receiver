#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <tchar.h>

constexpr auto FINISH = 100;
constexpr auto SIZE_BUFFER = 12;

int ConvertCharsToInt(char buff[])
{
    std::string str;
    size_t i = 0;
    while (true)
    {
        if (buff[i] != '\0')
        {
            str += buff[i++];
        }
        else
        {
            break;
        }
    }
    return std::stoi(str);
}
int main()
{
    int nextProgress = 0;
    int prevProgress = 0;
    std::thread th([&]()
        {
            while (true)
            {
                HANDLE  hFileMap;
                WCHAR* pBuffer = nullptr;
                char* cBuffer = new char[SIZE_BUFFER];
                size_t  size = sizeof(cBuffer);
                hFileMap = OpenFileMapping(
                    FILE_MAP_ALL_ACCESS,
                    FALSE,
                    L"Local\\MyFileMap");
                pBuffer = (WCHAR*)MapViewOfFile(
                    hFileMap,
                    FILE_MAP_ALL_ACCESS,
                    0,
                    0,
                    256);
                if (pBuffer != nullptr)
                {
                    CopyMemory(cBuffer, pBuffer, size);
                    nextProgress = ConvertCharsToInt(cBuffer);
                }
                UnmapViewOfFile(pBuffer);
                CloseHandle(hFileMap);
                delete[] cBuffer;
                if (nextProgress == FINISH)
                {
                    break;
                }
            }
        });
    while (true)
    {
        if (prevProgress != nextProgress)
        {
            prevProgress = nextProgress;
            std::cout << "Reading progress - " << prevProgress << std::endl;
            if (prevProgress == FINISH)
                break;
        }
    }
    th.join();
    return 0;
}