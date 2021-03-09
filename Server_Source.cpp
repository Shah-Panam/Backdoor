#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <WS2tcpip.h>
#include <vector>
#include <fstream>
#include <string>
#ifdef _WIN32
#include <WinSock2.h>
#else
#include <sys/sockets.h>
#endif // For Windows And Linux Distribution
#pragma comment(lib, "ws2_32.lib")
using namespace std;
const int BUFFER_SIZE = 4096;

void recv_file(SOCKET new_s)
{
    int val_read = 0, size, recv_size = 0;
    string addr, name;
    vector <char> buffer(BUFFER_SIZE + 1, 0);

    recv(new_s, buffer.data(), 1024, 0);
    name = buffer.data();

    //cout << name.substr(name.find(":") + 1);
    size = stoi(name.substr(name.find(":") + 1));
    name = name.substr(0, name.find(":"));
    cout << "File Name: " << name << endl << "Size: " << size << endl;

    ofstream wfile;
    wfile.open(name, ios::binary);
    if (wfile.is_open())
    {
        cout << "File Created" << endl;
    }
    else
    {
        cout << "Error in creating file" << endl;
    }

    while (recv_size < size)
    {
        val_read = (recv(new_s, buffer.data(), BUFFER_SIZE, 0));
        cout << val_read << endl;
        wfile.write(buffer.data(), val_read);
        recv_size = recv_size + val_read;
    }
    cout << endl << "File Received" << endl;

    wfile.close();
}

void menu()
{
    cout << endl << endl;
    cout << "1. To Browse Files and Folders" << endl;
    cout << "2. To Dowload a file" << endl;
    cout << "3. To Dowload a Folder" << endl;
    cout << "4. Clear Screen" << endl;
    cout << "5. End this Session" << endl;
    cout << "6. End Program" << endl;
    cout << "Enter Your Choice: ";
}

int main()
{
    SOCKET s, new_s;
    sockaddr_in server;
    string addr, message, ch;
    int valread, c;
    vector <char> buffer(BUFFER_SIZE + 1, 0);

#ifdef _WIN32
    WSADATA wsa;
    cout << "Initialising Winsock..." << endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        cout << "Failed. Error Code: " << WSAGetLastError << endl;
    }
    cout << "Initialised" << endl;
#endif // For Windows Environment

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        cout << "Socket Error" << endl;
        return 1;
    }

    cout << "Enter Server Address: ";
    cin >> addr;
    if (inet_pton(AF_INET, addr.c_str(), &server.sin_addr) <= 0)
    {
        cout << "invalid address" << endl;
        return 1;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(50000);

    if (bind(s, (sockaddr*)&server, sizeof(server)) < 0)
    {
        cout << "Connection Error" << endl;
        return 1;
    }
    cout << "Connected" << endl;

    if (listen(s, 5) < 0)
    {
        cout << "Listen Error" << endl;
    }

    if ((new_s = accept(s, (sockaddr*)&server, (socklen_t*)(&server))) == INVALID_SOCKET)
    {
        cout << "Accept Failed" << endl;
    }
    cout << "Accepted" << endl;

    while (1)
    {
        message.clear();
        menu();
        cin >> ch;
        try
        {
            c = stoi(ch);
        }
        catch (invalid_argument)
        {
            cout << "Enter Proper Choice" << endl;
            c = 0;
        }
        if (c == 1)
        {
            cout << "Enter the Address to Search: ";
            cin.ignore(1000, '\n');
            getline(cin, message);
            message.insert(0, to_string(c));
            send(new_s, message.c_str(), 1024, 0);
            
            while ((valread = recv(new_s, buffer.data(), 1024, 0)) != 0)
            {
                cout << buffer.data() << endl;
                if (strcmp(buffer.data(), "complete") == 0)
                    break;
                buffer.clear();
            }
        }

        else if (c == 2)
        {
            cout << "Enter the Address of File to Download: ";
            cin.ignore(1000, '\n');
            getline(cin, message);
            message.insert(0, to_string(c));
            send(new_s, message.c_str(), strlen(message.c_str()), 0);
            recv_file(new_s);
        }

        else if (c == 3)
        {
            cout << "Enter the Address of Folder to Download: ";
            cin.ignore(1000, '\n');
            getline(cin, message);
            message.insert(0, to_string(c));
            send(new_s, message.c_str(), 1024, 0);
            recv(new_s, buffer.data(), 1024, 0);
            cout << buffer.data() << endl;
            for (int i = 0; i < stoi(buffer.data()); i++)
            {
                cout << "Receiving: " << i + 1 << endl;
                recv_file(new_s);
                send(new_s, "Received", 1024, 0);
            }
        }

        else if (c == 4)
            cout << "\033[2J\033[1;1H";

        else if (c == 5 || c == 6)
        {
            send(new_s, ch.c_str(), 1024, 0);
            break;
        }
    }

    closesocket(s);
    closesocket(new_s);
    WSACleanup();
}