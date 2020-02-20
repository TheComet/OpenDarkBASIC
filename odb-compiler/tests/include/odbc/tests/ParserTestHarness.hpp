#pragma once

#include "odbc/parsers/db/Driver.hpp"
#include "odbc/parsers/keywords/KeywordMatcher.hpp"
#include "odbc/parsers/keywords/KeywordDB.hpp"
#include "odbc/ast/Node.hpp"
#include <gmock/gmock.h>
#include <fstream>
#include <filesystem>

class ParserTestHarness : public testing::Test
{
public:
    void SetUp() override
    {

        ast = nullptr;
        odbc::KeywordDB db;
        db.loadFromFile("../../odb-compiler/keywords/keywords107.ini");
        matcher.loadFromDB(&db);
        driver = new odbc::db::Driver(&ast, &matcher);
    }

    void TearDown() override
    {
        if (ast)
        {
            const testing::TestInfo* info = testing::UnitTest::GetInstance()->current_test_info();
            std::string filename = std::string("ast/") + info->test_suite_name()
                    + "__" + info->name() + ".dot";
            std::filesystem::create_directory("ast");
            std::ofstream out(filename);
            odbc::ast::dumpToDOT(out, ast);
            odbc::ast::freeNodeRecursive(ast);
        }

        delete driver;
    }
    odbc::db::Driver* driver;
    odbc::KeywordMatcher matcher;
    odbc::ast::Node* ast;
};