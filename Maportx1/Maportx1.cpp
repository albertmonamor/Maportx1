// Maportx1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <WinSock2.h>
#include "MapPort.h"

#include <vector>
#include <string>
#include <thread>
#include "Color.h"


#pragma comment(lib, "Ws2_32.lib")


string GetData(string level, string data) {

    string output;
    Color clr;
    clr.setColorOutput(10);
    cout << "level [" << level << "]::" << "enter " << data << "\n|--->";
    clr.setColorOutput(15);
    cin >> output;
    cout << endl;
    return output;
}

void ConfData(string* ip, int* timeout, int* start_range, int* range, int* threads) {

    // DEFINE DATA
    *ip = GetData("1", "ip address:");
    *timeout = stoi(GetData("2", "timeout:"));
    *start_range = stoi(GetData("3", "starting range:"));
    *range = stoi(GetData("4", "range:"));
    *threads = stoi(GetData("5", " number of threads:"));
    cout << endl;
}



int main()
{
    string ip;
    int timeout;
    int start_range;
    int range;
    int threads;

    // define all parameters
    ConfData(&ip, &timeout, &start_range, &range, &threads);

    // constractor

    Mapping_Port Search(ip ,timeout, start_range, range, threads);

    // starting...
    if (Search.TCPIsValid())
    {
       
        Search.MainPortScanner();
       
    }
    else {
        cout << "something Error" << WSAGetLastError() << endl;
    }

    cout << "\n\t\t\t *****MAPPING PORT x1 FINISHED NOW*****" << endl;
    cin >> ip;
}



