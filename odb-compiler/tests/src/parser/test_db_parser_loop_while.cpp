#include "gmock/gmock.h"
#include "odb-compiler/ast/SourceLocation.hpp"
#include "odb-compiler/parsers/db/Driver.hpp"
#include "odb-compiler/tests/ParserTestHarness.hpp"
#include "odb-compiler/tests/ASTMockVisitor.hpp"

#define NAME db_parser_loop_while

using namespace testing;

class NAME : public ParserTestHarness
{
public:
};

using namespace odb;

TEST_F(NAME, infinite_loop)
{
    ast = driver->parse("test", "while cond\nfoo()\nendwhile\n", matcher);
    ASSERT_THAT(ast, NotNull());
}

TEST_F(NAME, empty_loop)
{
    ast = driver->parse("test", "while cond\nendwhile\n", matcher);
    ASSERT_THAT(ast, NotNull());
}

TEST_F(NAME, break_from_loop)
{
    ast = driver->parse("test", "while cond\nexit\nendwhile\n", matcher);
    ASSERT_THAT(ast, NotNull());
}
