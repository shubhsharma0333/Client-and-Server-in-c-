// ChatApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
                      
#include <WinSock2.h>     // conatains core of windows.h
#include <ws2tcpip.h>     // includes tcp/ip functions 
#include <iphlpapi.h>     // includes ip associated functions
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

int main(int argc , char **argv)
{
    WSADATA wsaData;
    int iResult;
    int recvbuflen;
    const char* sendbuf = "This is a Quick Test.";
    char recvbuf[DEFAULT_BUFLEN];

    struct addrinfo* result = NULL,
          *ptr = NULL,
          hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);   // is called to initiate use of WS2_32.dll.The WSADATA structure contains information about the Windows Sockets implementation. The MAKEWORD(2,2) parameter of WSAStartup makes a request for version 2.2 of Winsock on the system, and sets the passed version as the highest version of Windows Sockets support that the caller can use.
    if (iResult != 0)
    {
        printf("WSAStartup failed : %d\n", iResult);
        return 1;
    }

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) 
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();               //used to terminate the use of WS2_32.lib
        return 1;
    }
    SOCKET ConnectSocket = INVALID_SOCKET;


    // Attempt to connect to the first address returned by

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {   // preparing socket to connect to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET)
        {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }
    }
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)  //error checking.
    {
         closesocket(ConnectSocket);
         ConnectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)  //error checking.
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    //send an initial buffer

    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (ConnectSocket == SOCKET_ERROR)
    {
        printf("cannot send : %d", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (ConnectSocket == SOCKET_ERROR)
    {
        printf("cannot send :%d", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    //recieve data until the server has cut off the connection
    do
    {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes recieved: %ld\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else 
            printf("recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);

    //clean up
    closesocket(ConnectSocket);
    WSACleanup();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
