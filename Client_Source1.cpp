#include <iostream>
#include <dirent.h>
#include <string>
#include <fstream>

using namespace std;

int filesize(string filename)
{
    FILE* fp;
    ifstream fps;
    fps.open(filename.c_str(), ios::binary);
    return fps.tellg();
}

int search(string path)
{
    string fullpath;
    string names;
    ofstream file;
    ofstream file2;
    file.open("files.txt", ios::app);
    file2.open("folders.txt", ios::app);
    dirent* entry;
    DIR* dir;

    if (( dir = opendir(path.c_str())) == NULL)
    {
        cout << "Wrong Address" << endl;
        file << "Wrong Address;";
        file2 << "Wrong Address;";
        return 0;
    }

    int index = 0;
    //file << "FILES FOUND:;;";
    //file2 << ";;FOLDERS FOUND:;";
    while (entry = readdir(dir))
    {
        fullpath = path + (string)(entry->d_name);
        names = (string)entry->d_name;
        if (filesize(fullpath) != -1)
        {
            index++;
            file << fullpath << ";";
        }
        else
        {
            if (names.at(0) != '.')
            {
                file2 << fullpath << ";";
            }
        }
    }
    file.close();
    file2.close();
    closedir(dir);
    return index;
}

void list_files()
{
    cout << endl << "Files Found:" << endl << endl;
    string temp;
    ifstream rfile;
    rfile.open("files.txt");
    char delim = ';';

    string str;
    string s;
    while (getline(rfile, temp, delim))
    {
        cout << temp << endl;
    }
    rfile.close();
}

int listfiles(string str)
{
    int temp;
    remove("files.txt");
    remove("folders.txt");
    //string str;
    //cout << "Enter Path: ";
    //cin.ignore(1000, '\n');
    //getline(cin, str);
    while (str.find("/") != string::npos)
    {
        str.replace(str.find("/"), 1, "\\");
    }
    if (str.find_last_of("\\") != (str.size() - 1))
    {
        str.append("\\");
    }
    temp = search(str);
    //crawl();
    list_files();
    return temp;
}