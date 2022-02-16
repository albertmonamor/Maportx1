#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <WinSock2.h>
#include <thread>
#include <vector>
#include "MapPort.h"
#include "Color.h"


Mapping_Port::Mapping_Port(string ip, int timeout, int start_range, int range, int _threads)
    :SERVER_ADDR(ip), TimeOut(timeout), START_RANGE(start_range), RANGE(range), THREADS(_threads)
{
    TimeOutRecv = 1;
}


Mapping_Port::~Mapping_Port() {
}


bool Mapping_Port::TCPIsValid() {
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa) == 0;
}

SOCKET Mapping_Port::TCPConnect(int port) {

    TIMEVAL timeout;
    timeout.tv_sec = TimeOut;
    timeout.tv_usec = 0;

    SOCKET mpx1 = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    inet_pton(AF_INET, SERVER_ADDR.c_str(), &hint.sin_addr);

    // TIMEOUT
    Valid_IOCTL(mpx1, 1);

    //CONNECT, and check if connect is valid
    if (connect(mpx1, (struct sockaddr*)&hint, sizeof(hint)) == false) { return ERROR_CONNECT; }

    // reset timeout
    Valid_IOCTL(mpx1, 0);

    // ..
    fd_set write, error;
    FD_ZERO(&write);
    FD_ZERO(&error);

    FD_SET(mpx1, &write);
    FD_SET(mpx1, &error);

    // SOCKET IS READY?
    select(0, NULL, &write, &error, &timeout);
    if (FD_ISSET(mpx1, &write)) {
        return mpx1;
    }
    else {
        return ERROR_CONNECT;
    }
}

int Mapping_Port::TCPGetPacket(SOCKET s0ck) 
{

    // TIMEOUT FOR RECV 
    fd_set timeOW;
    struct timeval Tout;
    FD_ZERO(&timeOW); 
    FD_SET(s0ck, &timeOW);
    Tout.tv_sec = TimeOutRecv; 
    Tout.tv_usec = 0;
    int READ = select(s0ck + 1, &timeOW, NULL, NULL, &Tout);

    // valid ? 0 is timeout -1 is error with connection
    if (READ == SOCKET_ERROR || READ == 0) {
        return ERROR_CONNECT;
    }
    // CLEAR BUFFER
    ZeroMemory(BUFFER, 28);

    int PACKET = recv(s0ck, BUFFER, 28, 0);
    if (TCPPacketIsInvalid(PACKET)) {
        return ERROR_CONNECT;
    }
    return PACKET;
}

void Mapping_Port::TCPSendPacket(SOCKET s0ck, string Packet) {


    int SendData = send(s0ck, Packet.c_str(), Packet.size() + 1, 0);
}


bool Mapping_Port::TCPPacketIsInvalid(int status) {
    if (status == SOCKET_ERROR) {
        return true;
    }
    if (status == 0) {
        return true;

    }
    return false;
}

void Mapping_Port::TCPCloseSock(SOCKET s0ck) {
    closesocket(s0ck);
}

void Mapping_Port::Valid_IOCTL(SOCKET sock, int mode) {
    unsigned long n = mode;
    int result = ioctlsocket(sock, FIONBIO, &n);
    // valid?
    if (result != 0) { cout << "error with io_sock " << result << endl; }
}


bool Mapping_Port::PortIsOpen(SOCKET s0ck, int port) {
    if (s0ck == ERROR_CONNECT) {
        return false;
    }
    else {
        Color clr;
        clr.setColorOutput(10);
        cout << "[+] PORT TCP\t" << port << " OPEN: ";
        clr.setColorOutput(11);
        cout << "description:" << VersionServicePort(s0ck) << "\n\n";
        clr.setColorOutput(12);
        return true;
    }
}

void Mapping_Port::MainPortScanner() {
   
    Color clr;
    // CHECKing RANGE
    if (RANGE > 65353) {
        clr.setColorOutput(12);
        cout << "[!] range of ports is 65353, you write" << RANGE << ". exit!" << endl;
        exit(0);
    }
    // CHECK IF THREADS BIGGER OF RANGE
    if (THREADS >= RANGE || 200 <= THREADS) {
        clr.setColorOutput(12);
        cout << "[!] numbers of threads will cause to logical failure. exit!" << endl;
        exit(0);
    }

    // THREADS
    std::vector<std::thread> prtbstr;
    for (int i = 0; i < THREADS; i++) {
        E += RANGE / THREADS;
        
        prtbstr.push_back(std::thread([this] {this->PortScanner((S + START_RANGE), (E + START_RANGE)); }));
        std::this_thread::sleep_for(0.01s);  // important!
        S = E;
    }

    // remnent of division
    prtbstr.push_back(std::thread([this] {this->PortScanner(E, E + (RANGE % E)); }));

    for (auto& TH : prtbstr) {
        TH.join();
    }


    // initializing local variable of class 
    E, S = 0, 0;
}

void Mapping_Port::PortScanner(int start_r, int end_r) {
    //cout << start_r << endl;
    for (int p = start_r; p < end_r; p++) {
        SOCKET mpx1 = TCPConnect(p);
        PortIsOpen(mpx1, p);
        TCPCloseSock(mpx1);
    }

}

string Mapping_Port::VersionServicePort(SOCKET s0ck) {

    string request = "GET / HTTP / 1.0\r\n\r\n";
    TCPSendPacket(s0ck, request);
    int response = TCPGetPacket(s0ck);
    if (response == ERROR_CONNECT) {
        return "unknown";
    }
    return string(BUFFER, 0, response);
}