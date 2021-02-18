#include "odb-compiler/ast/SourceLocation.hpp"
#include "odb-compiler/ast/ConstDecl.hpp"
#include "odb-compiler/commands/Command.hpp"
#include "odb-compiler/parsers/db/Driver.hpp"
#include "odb-compiler/tests/ASTMatchers.hpp"
#include "odb-compiler/tests/ASTMockVisitor.hpp"
#include "odb-compiler/tests/ParserTestHarness.hpp"
#include <fstream>

#define NAME db_parser_constant

using namespace testing;
using namespace odb;

class NAME : public ParserTestHarness
{
public:
};

TEST_F(NAME, bool_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mybool1 true\n"
        "#constant mybool2 false\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(2)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mybool1"))).After(exp);
    exp = EXPECT_CALL(v, visitBooleanLiteral(BooleanLiteralEq(true))).After(exp);
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mybool2"))).After(exp);
    exp = EXPECT_CALL(v, visitBooleanLiteral(BooleanLiteralEq(false))).After(exp);

    ast->accept(&v);
}

TEST_F(NAME, byte_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant myint 20\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "myint"))).After(exp);
    exp = EXPECT_CALL(v, visitByteLiteral(ByteLiteralEq(20)));

    ast->accept(&v);
}

TEST_F(NAME, word_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant myint 2000\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "myint"))).After(exp);
    exp = EXPECT_CALL(v, visitWordLiteral(WordLiteralEq(2000)));

    ast->accept(&v);
}

TEST_F(NAME, integer_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant myint 2147483647\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "myint"))).After(exp);
    exp = EXPECT_CALL(v, visitIntegerLiteral(IntegerLiteralEq(2147483647)));

    ast->accept(&v);
}

TEST_F(NAME, dword_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant myint 4294967295\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "myint"))).After(exp);
    exp = EXPECT_CALL(v, visitDwordLiteral(DwordLiteralEq(4294967295)));

    ast->accept(&v);
}

TEST_F(NAME, double_integer_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant myint 4294967296\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "myint"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleIntegerLiteral(DoubleIntegerLiteralEq(4294967296)));

    ast->accept(&v);
}

TEST_F(NAME, double_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble 5.2\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(5.2)));

    ast->accept(&v);
}

TEST_F(NAME, double_constant_annotated)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble# 5.2\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::FLOAT, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(5.2)));

    ast->accept(&v);
}

TEST_F(NAME, string_constant)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mystring \"hello world!\"\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mystring"))).After(exp);
    exp = EXPECT_CALL(v, visitStringLiteral(StringLiteralEq("hello world!")));

    ast->accept(&v);
}

TEST_F(NAME, string_constant_annotated)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mystring$ \"hello world!\"\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::STRING, "mystring"))).After(exp);
    exp = EXPECT_CALL(v, visitStringLiteral(StringLiteralEq("hello world!")));

    ast->accept(&v);
}

TEST_F(NAME, more_than_one_value_fails)
{
    ast = driver->parse("test",
        "#constant fail1 true false\n",
        matcher);
    EXPECT_THAT(ast, IsNull());

    ast = driver->parse("test",
        "#constant fail2 23 3.4\n",
        matcher);
    EXPECT_THAT(ast, IsNull());
}

TEST_F(NAME, double_1_notation)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble 53.\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(53)));

    ast->accept(&v);
}

TEST_F(NAME, double_2_notation)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble .53\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(.53)));

    ast->accept(&v);
}

TEST_F(NAME, double_3_notation)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble 53.f\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(53)));

    ast->accept(&v);
}

TEST_F(NAME, double_4_notation)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble .53f\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(.53)));

    ast->accept(&v);
}

TEST_F(NAME, double_exponential_notation_1)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble 12e2\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(12e2)));

    ast->accept(&v);
}

TEST_F(NAME, double_exponential_notation_2)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble 12.4e2\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(12.4e2)));

    ast->accept(&v);
}

TEST_F(NAME, double_exponential_notation_3)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble .4e2\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(40)));

    ast->accept(&v);
}

TEST_F(NAME, double_exponential_notation_4)
{
    using Annotation = ast::Symbol::Annotation;

    ast = driver->parse("test",
        "#constant mydouble 43.e2\n",
        matcher);
    ASSERT_THAT(ast, NotNull());

    StrictMock<ASTMockVisitor> v;
    Expectation exp;
    exp = EXPECT_CALL(v, visitBlock(BlockStmntCountEq(1)));
    exp = EXPECT_CALL(v, visitConstDecl(_)).After(exp);
    exp = EXPECT_CALL(v, visitAnnotatedSymbol(AnnotatedSymbolEq(Annotation::NONE, "mydouble"))).After(exp);
    exp = EXPECT_CALL(v, visitDoubleFloatLiteral(DoubleFloatLiteralEq(4300)));

    ast->accept(&v);
}
