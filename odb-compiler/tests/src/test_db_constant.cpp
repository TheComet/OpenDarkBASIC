#include <gmock/gmock.h>
#include "odbc/parsers/db/Driver.hpp"
#include "odbc/ast/Node.hpp"
#include "odbc/tests/ParserTestHarness.hpp"
#include <fstream>

#define NAME db_constant

using namespace testing;

class NAME : public ParserTestHarness
{
public:
};

using namespace odbc;

TEST_F(NAME, bool_constant)
{
    ASSERT_THAT(driver->parseString(
        "#constant mybool1 true\n"
        "#constant mybool2 false\n"), IsTrue());

    ast::Node* block = ast;
    ASSERT_THAT(block->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(block->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));

    block = block->block.next;
    ASSERT_THAT(block->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(block->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, integer_constant)
{
    ASSERT_THAT(driver->parseString(
        "#constant myint 20\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_constant)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat 5.2\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, string_constant)
{
    ASSERT_THAT(driver->parseString(
        "#constant mystring \"hello world!\"\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, more_than_one_value_fails)
{
    ASSERT_THAT(driver->parseString(
        "#constant fail1 true false\n"), IsFalse());
    ASSERT_THAT(driver->parseString(
        "#constant fail2 23 3.4\n"), IsFalse());
    ASSERT_THAT(ast, IsNull());
}

TEST_F(NAME, float_1_notation)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat 53.\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_2_notation)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat .53\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_3_notation)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat 53.f\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_4_notation)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat .53f\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_exponential_notation_1)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat 12e2\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_exponential_notation_2)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat 12.4e2\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_exponential_notation_3)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat .4e2\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}

TEST_F(NAME, float_exponential_notation_4)
{
    ASSERT_THAT(driver->parseString(
        "#constant myfloat 43.e2\n"), IsTrue());
    ASSERT_THAT(ast->info.type, Eq(ast::NT_BLOCK));
    ASSERT_THAT(ast->block.statement->info.type, Eq(ast::NT_SYM_CONST_DECL));
}
