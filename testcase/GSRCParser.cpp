#include <iostream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
using namespace std;

int main()
{
    string file_name;
    string temp;
    string module_name;
    string length;
    string width;
    string height("10");
    string color("7DA7B0");

    cout << "Enter file name" << endl;
    cin >> file_name;

    ifstream inFile(file_name + ".txt");
    ofstream outFile(file_name + ".csv");
    outFile << "Name,Color,L,W,T,P/S";

    for (int i = 0; i < 9; i++) {
        getline(inFile, temp);
    }

    while (!inFile.eof())
    {
        getline(inFile, temp, ' ');
        size_t found = temp.find("sb");
        if (found == 0)
        {
            outFile << endl;
            module_name = temp;
            for (int i = 0; i < 7; i++) getline(inFile, temp, ' ');
            length = temp.substr(1, temp.length()-1);
            getline(inFile, temp, ' ');
            width = temp.substr(0, temp.length()-1);
            outFile << module_name + "," << color + "," << length << width + "," << height + "," << "0";
        }
        getline(inFile, temp);
    }
    return 0;
}