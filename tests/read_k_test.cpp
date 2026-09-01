#include "core/Reader.h"
#include <iostream>
int main(int argc, char** argv){
    const char* path = "mdsp_instances/drand-10-75-1.txt";
    if (argc>1) path = argv[1];
    auto inst = MDSPReader::readFromFile(path);
    std::cout << "Loaded " << path << " -> |D| = " << inst.k() << "\n";
    std::cout << "D:";
    for (auto d: inst.D) std::cout << " " << d;
    std::cout << "\n";
    return 0;
}
