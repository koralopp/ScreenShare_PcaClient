#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <psapi.h>
#include <ctime>
#include <algorithm>

struct ProcessInfo {
    std::string path;
    std::string date;
    DWORD pid;
};

void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

std::string getLastModifiedDate(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess != nullptr) {
        FILETIME ftCreation, ftExit, ftKernel, ftUser;
        SYSTEMTIME stUTC, stLocal;
        if (GetProcessTimes(hProcess, &ftCreation, &ftExit, &ftKernel, &ftUser)) {
            FileTimeToSystemTime(&ftCreation, &stUTC);
            SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);

            struct tm tmLocal = {};
            tmLocal.tm_year = stLocal.wYear - 1900;
            tmLocal.tm_mon = stLocal.wMonth - 1;
            tmLocal.tm_mday = stLocal.wDay;
            tmLocal.tm_hour = stLocal.wHour;
            tmLocal.tm_min = stLocal.wMinute;
            tmLocal.tm_sec = stLocal.wSecond;
            tmLocal.tm_isdst = -1;

            char buf[80];
            std::strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", &tmLocal);
            CloseHandle(hProcess);
            return std::string(buf);
        }
        CloseHandle(hProcess);
    }
    return "Unknown";
}

std::string getExecutablePath(DWORD pid) {
    TCHAR path[MAX_PATH];
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (hProcess != nullptr) {
        if (GetModuleFileNameEx(hProcess, nullptr, path, sizeof(path) / sizeof(TCHAR))) {
            CloseHandle(hProcess);
            return std::string(path);
        }
        CloseHandle(hProcess);
    }
    return "";
}

void getRunningProcesses(std::vector<ProcessInfo>& processList) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        return;
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    for (i = 0; i < cProcesses; i++) {
        DWORD pid = aProcesses[i];
        if (pid != 0) {
            std::string path = getExecutablePath(pid);
            if (!path.empty()) {
                std::string date = getLastModifiedDate(pid);
                ProcessInfo procInfo = { path, date, pid };
                processList.push_back(procInfo);
            }
        }
    }
}

bool compareByDate(const ProcessInfo& a, const ProcessInfo& b) {
    return a.date > b.date;
}

void showProcesses(std::vector<ProcessInfo>& processList) {
    std::sort(processList.begin(), processList.end(), compareByDate);

    setConsoleColor(8);
    std::cout << "--------------------------------------------------------------------" << std::endl;

    for (size_t i = 0; i < processList.size(); ++i) {
        const ProcessInfo& proc = processList[i];

        setConsoleColor(9);
        std::cout << "Date: ";
        setConsoleColor(11);
        std::cout << proc.date;

        setConsoleColor(8);
        std::cout << " | ";

        setConsoleColor(9);
        std::cout << "Path: ";
        setConsoleColor(11);
        std::cout << proc.path;

        setConsoleColor(8);
        std::cout << " | ";

        setConsoleColor(9);
        std::cout << "PID: ";
        setConsoleColor(11);
        std::cout << proc.pid << std::endl;

        setConsoleColor(8);
        std::cout << "--------------------------------------------------------------------" << std::endl;
    }
}

int main() {
    SetConsoleTitle("PcaClient by @koralopp");

    std::vector<ProcessInfo> processList;

    setConsoleColor(12);
    std::cout << "  ____   ____    _       ____ _     ___ _____ _   _ _____ " << std::endl;
    std::cout << " |  _ \\ / ___|  / \\     / ___| |   |_ _| ____| \\ | |_   _|" << std::endl;
    std::cout << " | |_) | |     / _ \\   | |   | |    | ||  _| |  \\| | | |  " << std::endl;
    std::cout << " |  __/| |___ / ___ \\  | |___| |___ | || |___| |\\  | | |  " << std::endl;
    std::cout << " |_|    \\____/_/   \\_\\  \\____|_____|___|_____|_| \\_| |_|  " << std::endl;

    setConsoleColor(6);
    std::cout << "Developed by:" << std::endl;
    std::cout << "https://github.com/koralopp" << std::endl;
    std::cout << "https://github.com/reqoan" << std::endl;

    setConsoleColor(14);
    std::cout << "Press 1 to start the process or 0 to exit: ";
    int userChoice;
    std::cin >> userChoice;

    if (userChoice == 0) {
        std::cout << "Exiting program..." << std::endl;
        return 0;
    } else if (userChoice == 1) {
        getRunningProcesses(processList);
        showProcesses(processList);

        setConsoleColor(15);
        std::cout << "Done! press enter to exit..." << std::endl;
        std::cin.get();
        std::cin.get();
    } else {
        setConsoleColor(4);
        std::cout << "Invalid option. Exiting program..." << std::endl;
        return 0;
    }

    return 0;
}
