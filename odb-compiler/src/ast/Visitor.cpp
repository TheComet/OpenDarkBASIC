#include "odb-compiler/ast/ArrayRef.hpp"
#include "odb-compiler/ast/Assignment.hpp"
#include "odb-compiler/ast/Block.hpp"
#include "odb-compiler/ast/ConstDecl.hpp"
#include "odb-compiler/ast/ExpressionList.hpp"
#include "odb-compiler/ast/FuncCall.hpp"
#include "odb-compiler/ast/Keyword.hpp"
#include "odb-compiler/ast/Literal.hpp"
#include "odb-compiler/ast/Loop.hpp"
#include "odb-compiler/ast/Node.hpp"
#include "odb-compiler/ast/SourceLocation.hpp"
#include "odb-compiler/ast/Symbol.hpp"
#include "odb-compiler/ast/VarDecl.hpp"
#include "odb-compiler/ast/VarRef.hpp"
#include "odb-compiler/ast/Visitor.hpp"

namespace odb {
namespace ast {

// ----------------------------------------------------------------------------
void GenericVisitor::visitAnnotatedSymbol(const AnnotatedSymbol* node)               { visit(node); }
void GenericVisitor::visitArrayRef(const ArrayRef* node)                             { visit(node); }
void GenericVisitor::visitBlock(const Block* node)                                   { visit(node); }
void GenericVisitor::visitConstDecl(const ConstDecl* node)                           { visit(node); }
void GenericVisitor::visitExpressionList(const ExpressionList* node)                 { visit(node); }
void GenericVisitor::visitForLoop(const ForLoop* node)                               { visit(node); }
void GenericVisitor::visitFuncCallExpr(const FuncCallExpr* node)                     { visit(node); }
void GenericVisitor::visitFuncCallExprOrArrayRef(const FuncCallExprOrArrayRef* node) { visit(node); }
void GenericVisitor::visitFuncCallStmnt(const FuncCallStmnt* node)                   { visit(node); }
void GenericVisitor::visitInfiniteLoop(const InfiniteLoop* node)                     { visit(node); }
void GenericVisitor::visitKeywordExpr(const KeywordExpr* node)                       { visit(node); }
void GenericVisitor::visitKeywordExprSymbol(const KeywordExprSymbol* node)           { visit(node); }
void GenericVisitor::visitKeywordStmntSymbol(const KeywordStmntSymbol* node)         { visit(node); }
void GenericVisitor::visitKeywordStmnt(const KeywordStmnt* node)                     { visit(node); }
void GenericVisitor::visitScopedSymbol(const ScopedSymbol* node)                     { visit(node); }
void GenericVisitor::visitScopedAnnotatedSymbol(const ScopedAnnotatedSymbol* node)   { visit(node); }
void GenericVisitor::visitSymbol(const Symbol* node)                                 { visit(node); }
void GenericVisitor::visitUntilLoop(const UntilLoop* node)                           { visit(node); }
void GenericVisitor::visitVarAssignment(const VarAssignment* node)                   { visit(node); }
void GenericVisitor::visitVarRef(const VarRef* node)                                 { visit(node); }
void GenericVisitor::visitWhileLoop(const WhileLoop* node)                           { visit(node); }

#define X(dbname, cppname) \
    void GenericVisitor::visit##dbname##Literal(const dbname##Literal* node)         { visit(node); }
ODB_DATATYPE_LIST
#undef X

#define X(dbname, cppname) \
    void GenericVisitor::visit##dbname##VarDecl(const dbname##VarDecl* node)         { visit(node); }
ODB_DATATYPE_LIST
#undef X

}
}
