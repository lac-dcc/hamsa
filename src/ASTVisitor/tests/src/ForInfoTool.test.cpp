#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#define LOG "log.txt"

TEST_CASE("Sample: 1") {
    std::system("../build/bin/ForInfoTool samples/sample1.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp, increment;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond >> increment;

    CHECK(type == "int");
    CHECK(name == "i");
    CHECK(atrb == "=");
    CHECK(valInit == 0);
    CHECK(name2 == "i");
    CHECK(comp == "<");
    CHECK(valCond == 10);
    CHECK(increment == "i++");
    log.close();
}

TEST_CASE("Sample: 2") {
    std::system("../build/bin/ForInfoTool samples/sample2.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp, increment;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond >> increment;

    CHECK(type == "int");
    CHECK(name == "i");
    CHECK(atrb == "=");
    CHECK(valInit == 10);
    CHECK(name2 == "i");
    CHECK(comp == ">");
    CHECK(valCond == 0);
    CHECK(increment == "i--");
    log.close();
}

TEST_CASE("Sample: 3") {
    std::system("../build/bin/ForInfoTool samples/sample3.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp, increment;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond >> increment;

    CHECK(type == "int");
    CHECK(name == "i");
    CHECK(atrb == "=");
    CHECK(valInit == 0);
    CHECK(name2 == "i");
    CHECK(comp == "<=");
    CHECK(valCond == 10);
    CHECK(increment == "i++");
    log.close();
}

TEST_CASE("Sample: 4") {
    std::system("../build/bin/ForInfoTool samples/sample4.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp, increment;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond >> increment;

    CHECK(type == "int");
    CHECK(name == "i");
    CHECK(atrb == "=");
    CHECK(valInit == 10);
    CHECK(name2 == "i");
    CHECK(comp == ">=");
    CHECK(valCond == 0);
    CHECK(increment == "i--");
    log.close();
}

TEST_CASE("Sample: 5") {
    std::system("../build/bin/ForInfoTool samples/sample5.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, valInit, name2, comp, increment;
    int valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond >> increment;

    CHECK(type == "int");
    CHECK(name == "i");
    CHECK(atrb == "=");
    CHECK(valInit == "n");
    CHECK(name2 == "i");
    CHECK(comp == "<");
    CHECK(valCond == 10);
    CHECK(increment == "i++");
    log.close();
}

TEST_CASE("Sample: 6") {
    std::system("../build/bin/ForInfoTool samples/sample6.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, valCond, name2, comp, increment;
    int valInit;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond >> increment;

    CHECK(type == "int");
    CHECK(name == "i");
    CHECK(atrb == "=");
    CHECK(valInit == 0);
    CHECK(name2 == "i");
    CHECK(comp == "<");
    CHECK(valCond == "n");
    CHECK(increment == "i++");
    log.close();
}

TEST_CASE("Sample: 7") {
    std::system("../build/bin/ForInfoTool samples/sample7.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, valInit, name2, comp, valCond, increment;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond >> increment;

    CHECK(type == "int");
    CHECK(name == "i");
    CHECK(atrb == "=");
    CHECK(valInit == "n");
    CHECK(name2 == "i");
    CHECK(comp == "<");
    CHECK(valCond == "m");
    CHECK(increment == "i++");
    log.close();
}

TEST_CASE("Sample: 8") {
    std::system("../build/bin/ForInfoTool samples/sample8.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type_1, name_1, atrb_1, name2_1, comp_1, increment_1;
    std::string type_2, name_2, atrb_2, name2_2, comp_2, increment_2;
    int valInit_1, valCond_1;
    int valInit_2, valCond_2;

    log >> type_1 >> name_1 >> atrb_1 >> valInit_1 >> name2_1 >> comp_1 >> valCond_1 >> increment_1;
    log >> type_2 >> name_2 >> atrb_2 >> valInit_2 >> name2_2 >> comp_2 >> valCond_2 >> increment_2;

    CHECK(type_1 == "int");
    CHECK(name_1 == "i");
    CHECK(atrb_1 == "=");
    CHECK(valInit_1 == 0);
    CHECK(name2_1 == "i");
    CHECK(comp_1 == "<");
    CHECK(valCond_1 == 10);
    CHECK(increment_1 == "i++");

    CHECK(type_2 == "int");
    CHECK(name_2 == "j");
    CHECK(atrb_2 == "=");
    CHECK(valInit_2 == 10);
    CHECK(name2_2 == "j");
    CHECK(comp_2 == "<=");
    CHECK(valCond_2 == 0);
    CHECK(increment_2 == "j--");
    
    log.close();
}

TEST_CASE("Sample: 9") {
    std::system("../build/bin/ForInfoTool samples/sample9.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type_1, name_1, atrb_1, name2_1, comp_1, valCond_1, increment_1;
    std::string type_2, name_2, atrb_2, name2_2, comp_2, valInit_2, increment_2;
    int valInit_1;
    int valCond_2;

    log >> type_1 >> name_1 >> atrb_1 >> valInit_1 >> name2_1 >> comp_1 >> valCond_1 >> increment_1;
    log >> type_2 >> name_2 >> atrb_2 >> valInit_2 >> name2_2 >> comp_2 >> valCond_2 >> increment_2;

    CHECK(type_1 == "int");
    CHECK(name_1 == "i");
    CHECK(atrb_1 == "=");
    CHECK(valInit_1 == 0);
    CHECK(name2_1 == "i");
    CHECK(comp_1 == "<");
    CHECK(valCond_1 == "m");
    CHECK(increment_1 == "i++");

    CHECK(type_2 == "int");
    CHECK(name_2 == "j");
    CHECK(atrb_2 == "=");
    CHECK(valInit_2 == "n");
    CHECK(name2_2 == "j");
    CHECK(comp_2 == ">=");
    CHECK(valCond_2 == 0);
    CHECK(increment_2 == "j--");
    
    log.close();
}

TEST_CASE("Sample: 10") {
    std::system("../build/bin/ForInfoTool samples/sample10.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string name_1, atrb_1, name2_1, comp_1, valCond_1, increment_1;
    std::string name_2, atrb_2, name2_2, comp_2, valInit_2, valCond_2, increment_2;
    std::string name_3, atrb_3, name2_3, comp_3, valCond_3, increment_3;
    int valInit_1;
    int valInit_3;

    log >> name_1 >> atrb_1 >> valInit_1 >> name2_1 >> comp_1 >> valCond_1 >> increment_1;
    log >> name_2 >> atrb_2 >> valInit_2 >> name2_2 >> comp_2 >> valCond_2 >> increment_2;
    log >> name_3 >> atrb_3 >> valInit_3 >> name2_3 >> comp_3 >> valCond_3 >> increment_3;

    CHECK(name_1 == "j");
    CHECK(atrb_1 == "=");
    CHECK(valInit_1 == 1);
    CHECK(name2_1 == "j");
    CHECK(comp_1 == "<=");
    CHECK(valCond_1 == "n");
    CHECK(increment_1 == "j++");

    CHECK(name_2 == "i");
    CHECK(atrb_2 == "=");
    CHECK(valInit_2 == "l");
    CHECK(name2_2 == "i");
    CHECK(comp_2 == "<=");
    CHECK(valCond_2 == "j");
    CHECK(increment_2 == "i++");

    CHECK(name_3 == "i");
    CHECK(atrb_3 == "=");
    CHECK(valInit_3 == 1);
    CHECK(name2_3 == "i");
    CHECK(comp_3 == "<=");
    CHECK(valCond_3 == "m");
    CHECK(increment_3 == "i++");
    
    log.close();
}