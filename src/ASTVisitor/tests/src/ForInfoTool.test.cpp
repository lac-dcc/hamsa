#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest/doctest/doctest.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#define LOG "log.txt"

TEST_CASE("Testing sample1") {
    std::system("../build/bin/ForInfoTool samples/sample1.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp;
    int valI, valF;

    log >> type >> name >> atrb >> valI >> name2 >> comp >> valF;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(valI == 0);
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare("<"));
    CHECK(valF == 10);
    log.close();
}