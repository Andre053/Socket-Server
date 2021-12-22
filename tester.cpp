#include <iostream>
#include <fstream>

int main() {
    std::cout << "Running tester file\n" << std::endl;

    std::ifstream MyFile("/Users/andredebiasi/documents/sometext.rtf");
    std::string output, text;

    while (getline (MyFile, text)) {
        output += text;
    }

    MyFile.close();

    std::cout << "File data is: " << output << std::endl;
    return 0;
}
