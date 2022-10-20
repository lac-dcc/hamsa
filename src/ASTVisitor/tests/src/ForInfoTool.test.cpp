#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest/doctest/doctest.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#define LOG "log.txt"

TEST_CASE("Sample: 1") {
    std::system("../build/bin/ForInfoTool samples/sample1.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(valInit == 0);
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare("<"));
    CHECK(valCond == 10);
    log.close();
}

TEST_CASE("Sample: 2") {
    std::system("../build/bin/ForInfoTool samples/sample2.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(valInit == 10);
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare(">"));
    CHECK(valCond == 0);
    log.close();
}

TEST_CASE("Sample: 3") {
    std::system("../build/bin/ForInfoTool samples/sample3.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(valInit == 0);
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare("<="));
    CHECK(valCond == 10);
    log.close();
}

TEST_CASE("Sample: 4") {
    std::system("../build/bin/ForInfoTool samples/sample4.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, name2, comp;
    int valInit, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(valInit == 10);
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare(">="));
    CHECK(valCond == 0);
    log.close();
}

TEST_CASE("Sample: 5") {
    std::system("../build/bin/ForInfoTool samples/sample5.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, valInit, name2, comp;
    int valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(!valInit.compare("n"));
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare("<"));
    CHECK(valCond == 10);
    log.close();
}

TEST_CASE("Sample: 6") {
    std::system("../build/bin/ForInfoTool samples/sample6.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, valCond, name2, comp;
    int valInit;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(valInit == 0);
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare("<"));
    CHECK(!valCond.compare("n"));
    log.close();
}

TEST_CASE("Sample: 7") {
    std::system("../build/bin/ForInfoTool samples/sample7.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type, name, atrb, valInit, name2, comp, valCond;

    log >> type >> name >> atrb >> valInit >> name2 >> comp >> valCond;

    CHECK(!type.compare("int"));
    CHECK(!name.compare("i"));
    CHECK(!atrb.compare("="));
    CHECK(!valInit.compare("n"));
    CHECK(!name2.compare("i"));
    CHECK(!comp.compare("<"));
    CHECK(!valCond.compare("m"));
    log.close();
}

TEST_CASE("Sample: 8") {
    std::system("../build/bin/ForInfoTool samples/sample8.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type_1, name_1, atrb_1, name2_1, comp_1;
    std::string type_2, name_2, atrb_2, name2_2, comp_2;
    int valInit_1, valCond_1;
    int valInit_2, valCond_2;

    log >> type_1 >> name_1 >> atrb_1 >> valInit_1 >> type_2 >> name_2 >> atrb_2 >> valInit_2;
    log >> name2_1 >> comp_1 >> valCond_1 >> name2_2 >> comp_2 >> valCond_2;

    CHECK(!type_1.compare("int"));
    CHECK(!name_1.compare("i"));
    CHECK(!atrb_1.compare("="));
    CHECK(valInit_1 == 0);
    CHECK(!name2_1.compare("i"));
    CHECK(!comp_1.compare("<"));
    CHECK(valCond_1 == 10);

    CHECK(!type_2.compare("int"));
    CHECK(!name_2.compare("j"));
    CHECK(!atrb_2.compare("="));
    CHECK(valInit_2 == 10);
    CHECK(!name2_2.compare("j"));
    CHECK(!comp_2.compare("<="));
    CHECK(valCond_2 == 0);
    
    log.close();
}

TEST_CASE("Sample: 9") {
    std::system("../build/bin/ForInfoTool samples/sample9.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string type_1, name_1, atrb_1, name2_1, comp_1, valCond_1;
    std::string type_2, name_2, atrb_2, name2_2, comp_2, valInit_2;
    int valInit_1;
    int valCond_2;

    log >> type_1 >> name_1 >> atrb_1 >> valInit_1 >> type_2 >> name_2 >> atrb_2 >> valInit_2;
    log >> name2_1 >> comp_1 >> valCond_1 >> name2_2 >> comp_2 >> valCond_2;

    CHECK(!type_1.compare("int"));
    CHECK(!name_1.compare("i"));
    CHECK(!atrb_1.compare("="));
    CHECK(valInit_1 == 0);
    CHECK(!name2_1.compare("i"));
    CHECK(!comp_1.compare("<"));
    CHECK(!valCond_1.compare("m"));

    CHECK(!type_2.compare("int"));
    CHECK(!name_2.compare("j"));
    CHECK(!atrb_2.compare("="));
    CHECK(!valInit_2.compare("n"));
    CHECK(!name2_2.compare("j"));
    CHECK(!comp_2.compare(">="));
    CHECK(valCond_2 == 0);
    
    log.close();
}

TEST_CASE("Sample: 10") {
    std::system("../build/bin/ForInfoTool samples/sample10.cpp > log.txt");
    std::fstream log;
    log.open(LOG, std::fstream::in);
    
    std::string name_1, atrb_1, name2_1, comp_1, valCond_1;
    std::string name_2, atrb_2, name2_2, comp_2, valInit_2, valCond_2;
    std::string name_3, atrb_3, name2_3, comp_3, valCond_3;
    int valInit_1;
    int valInit_3;

    log >> name_1 >> atrb_1 >> valInit_1 >> name_2 >> atrb_2 >> valInit_2 >> name_3 >> atrb_3 >> valInit_3;
    log >> name2_1 >> comp_1 >> valCond_1 >> name2_2 >> comp_2 >> valCond_2 >> name2_3 >> comp_3 >> valCond_3;

    CHECK(!name_1.compare("j"));
    CHECK(!atrb_1.compare("="));
    CHECK(valInit_1 == 1);
    CHECK(!name2_1.compare("j"));
    CHECK(!comp_1.compare("<="));
    CHECK(!valCond_1.compare("n"));

    CHECK(!name_2.compare("i"));
    CHECK(!atrb_2.compare("="));
    CHECK(!valInit_2.compare("l"));
    CHECK(!name2_2.compare("i"));
    CHECK(!comp_2.compare("<="));
    CHECK(!valCond_2.compare("j"));

    CHECK(!name_3.compare("i"));
    CHECK(!atrb_3.compare("="));
    CHECK(valInit_3 == 1);
    CHECK(!name2_3.compare("i"));
    CHECK(!comp_3.compare("<="));
    CHECK(!valCond_3.compare("m"));
    
    log.close();
}