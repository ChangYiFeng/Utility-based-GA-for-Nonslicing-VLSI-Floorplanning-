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
    outFile << "Name,Color,L,W,T,P/S" << endl;

    while (!inFile.eof())
    {
        getline(inFile, temp);
        size_t found = temp.find("MODULE");
        if (found == 0)
        {
            module_name = temp.substr(7, temp.length()-8);
            getline(inFile, temp);
            for (int i = 0; i < 4; i++) getline(inFile, temp, ' ');
            getline(inFile, length, ' ');
            getline(inFile, width, ' ');
            outFile << module_name + "," << color + "," << length + "," << width + "," << height + "," << "0" << endl;
        }
    }
    return 0;
}