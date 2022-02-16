#pragma once

#include <iostream>
#include <WS2tcpip.h>
#include <windows.h>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
# define ERROR_CONNECT 0xefa53fe


class Mapping_Port {

public:

	// Init Class
	Mapping_Port(string SERVER_ADDR, int timeout, int start_range, int range, int _threads);

	~Mapping_Port();



	// checking if dll library is supported 
	bool TCPIsValid();

	// create socket and connect
	// return socket if have connection 
	SOCKET TCPConnect(int port);

	// Send Packet to server
	void TCPSendPacket(SOCKET s0ck, string Packet);

	// Get Packet From server
	int TCPGetPacket(SOCKET s0ck);

	// check if packet that rececive from server is valid
	bool TCPPacketIsInvalid(int status);

	// close socket fd
	void TCPCloseSock(SOCKET s0ck);

	// define ioctlsocket for time out
	void Valid_IOCTL(SOCKET sock, int mode);

	// check if port is open
	bool PortIsOpen(SOCKET sock, int port);

	// divid threads by ::THREADS var
	void MainPortScanner();

	// function per thread
	void PortScanner(int start_r, int end_r);

	// sending  GET (http method) if socket is open
	string VersionServicePort(SOCKET s0ck);	

private:
	string		SERVER_ADDR;
	char		BUFFER[28];
	int			TimeOut;
	int			TimeOutRecv;
	int			START_RANGE;
	int			RANGE;
	int			THREADS;
	int			S = 0;
	int			E = 0;
	int			num_thread;
};
