#include "odb-compiler/ast/Exporters.hpp"
#include "odb-compiler/ast/AnnotatedSymbol.hpp"
#include "odb-compiler/ast/ArgList.hpp"
#include "odb-compiler/ast/ArrayDecl.hpp"
#include "odb-compiler/ast/ArrayRef.hpp"
#include "odb-compiler/ast/Assignment.hpp"
#include "odb-compiler/ast/BinaryOp.hpp"
#include "odb-compiler/ast/Block.hpp"
#include "odb-compiler/ast/CommandExpr.hpp"
#include "odb-compiler/ast/CommandStmnt.hpp"
#include "odb-compiler/ast/Conditional.hpp"
#include "odb-compiler/ast/ConstDecl.hpp"
#include "odb-compiler/ast/Exit.hpp"
#include "odb-compiler/ast/FuncCall.hpp"
#include "odb-compiler/ast/FuncDecl.hpp"
#include "odb-compiler/ast/Goto.hpp"
#include "odb-compiler/ast/InitializerList.hpp"
#include "odb-compiler/ast/Label.hpp"
#include "odb-compiler/ast/Literal.hpp"
#include "odb-compiler/ast/Loop.hpp"
#include "odb-compiler/ast/Node.hpp"
#include "odb-compiler/ast/ScopedAnnotatedSymbol.hpp"
#include "odb-compiler/ast/SelectCase.hpp"
#include "odb-compiler/ast/SourceLocation.hpp"
#include "odb-compiler/ast/Statement.hpp"
#include "odb-compiler/ast/Subroutine.hpp"
#include "odb-compiler/ast/Symbol.hpp"
#include "odb-compiler/ast/UDTDecl.hpp"
#include "odb-compiler/ast/UDTField.hpp"
#include "odb-compiler/ast/UDTRef.hpp"
#include "odb-compiler/ast/UnaryOp.hpp"
#include "odb-compiler/ast/VarDecl.hpp"
#include "odb-compiler/ast/VarRef.hpp"
#include "odb-compiler/ast/Visitor.hpp"
#include "odb-compiler/commands/Command.hpp"
#include "odb-sdk/Str.hpp"
#include <unordered_map>

namespace odb::ast {

#ifdef ODBCOMPILER_DOT_EXPORT

// ----------------------------------------------------------------------------
class NodeGUIDs : public GenericConstVisitor
{
public:
    void calculate(const Node* root) { root->accept(this); }

public:
    void visit(const Node* node) override { map_.emplace(node, guidCounter_++); }
    int get(const Node* node) const { return map_.at(node); }

private:
    std::unordered_map<const Node*, int> map_;
    int guidCounter_ = 1;
};

// ----------------------------------------------------------------------------
class ConnectionWriter : public ConstVisitor
{
public:
    void write(FILE* fp, const NodeGUIDs* guids, const Node* root)
    {
        fp_ = fp;
        guids_ = guids;
        root->accept(this);
    }

private:
    void writeNamedConnection(const Node* from, const Node* to, const std::string& name)
    {
        fprintf(fp_, "N%d -> N%d [label=\"%s\"];\n",
                guids_->get(from), guids_->get(to), name.c_str());
        /*fprintf(fp_, "N%d -> N%d [color=\"blue\"];\n",
                guids_->get(to), guids_->get(to->parent()));*/
    }

    void visitAnnotatedSymbol(const AnnotatedSymbol* node) override {}
    void visitArgList(const ArgList* node) override
    {
        int i = 0;
        for (const auto& expr : node->expressions())
        {
            writeNamedConnection(node, expr, "arglist[" + std::to_string(i++) + "]");
        }
    }
    void visitArrayRef(const ArrayRef* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        writeNamedConnection(node, node->args(), "args");
    }
    void visitArrayAssignment(const ArrayAssignment* node) override
    {
        writeNamedConnection(node, node->array(), "array");
        writeNamedConnection(node, node->expression(), "expr");
    }
    void visitBinaryOp(const BinaryOp* node) override
    {
        writeNamedConnection(node, node->lhs(), "lhs");
        writeNamedConnection(node, node->rhs(), "rhs");
    }
    void visitBlock(const Block* node) override
    {
        int i = 0;
        for (const auto& stmnt : node->statements())
            writeNamedConnection(node, stmnt, "stmnt[" + std::to_string(i++) + "]");
    }
    void visitCase(const Case* node) override
    {
        writeNamedConnection(node, node->expression(), "expr");
        if (node->body().notNull())
            writeNamedConnection(node, node->body(), "body");
    }
    void visitCaseList(const CaseList* node) override
    {
        int i = 0;
        for (const auto& case_ : node->cases())
            writeNamedConnection(node, case_, "case[" + std::to_string(i++) + "]");
        if (node->defaultCase().notNull())
            writeNamedConnection(node, node->defaultCase(), "default");
    }
    void visitExit(const Exit* node) override {}
    void visitCommandExpr(const CommandExpr* node) override
    {
        if (node->args().notNull())
            writeNamedConnection(node, node->args(), "args");
    }
    void visitCommandStmnt(const CommandStmnt* node) override
    {
        if (node->args().notNull())
            writeNamedConnection(node, node->args(), "args");
    }
    void visitConditional(const Conditional* node) override
    {
        writeNamedConnection(node, node->condition(), "cond");
        if (node->trueBranch().notNull())
            writeNamedConnection(node, node->trueBranch(), "true");
        if (node->falseBranch().notNull())
            writeNamedConnection(node, node->falseBranch(), "false");
    }
    void visitConstDecl(const ConstDecl* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        writeNamedConnection(node, node->literal(), "literal");
    }
    void visitConstDeclExpr(const ConstDeclExpr* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        writeNamedConnection(node, node->expression(), "expr");
    }
    void visitDefaultCase(const DefaultCase* node) override
    {
        if (node->body().notNull())
            writeNamedConnection(node, node->body(), "body");
    }
    void visitForLoop(const ForLoop* node) override
    {
        writeNamedConnection(node, node->counter(), "counter");
        writeNamedConnection(node, node->endValue(), "endValue");

        if (node->stepValue().notNull())
            writeNamedConnection(node, node->stepValue(), "stepValue");
        if (node->nextSymbol().notNull())
            writeNamedConnection(node, node->nextSymbol(), "nextSymbol");
        if (node->body().notNull())
            writeNamedConnection(node, node->body(), "body");
    }
    void visitFuncCallExpr(const FuncCallExpr* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        if (node->args().notNull())
            writeNamedConnection(node, node->args(), "args");
    }
    void visitFuncCallExprOrArrayRef(const FuncCallExprOrArrayRef* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        writeNamedConnection(node, node->args(), "args");
    }
    void visitFuncCallStmnt(const FuncCallStmnt* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        if (node->args().notNull())
            writeNamedConnection(node, node->args(), "args");
    }
    void visitFuncDecl(const FuncDecl* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        if (node->args().notNull())
            writeNamedConnection(node, node->args(), "args");
        if (node->body().notNull())
            writeNamedConnection(node, node->body(), "body");
        if (node->returnValue().notNull())
            writeNamedConnection(node, node->returnValue(), "returnValue");
    }
    void visitFuncExit(const FuncExit* node) override
    {
        if (node->returnValue().notNull())
            writeNamedConnection(node, node->returnValue(), "returnValue");
    }
    void visitGoto(const Goto* node) override
    {
        writeNamedConnection(node, node->label(), "label");
    }
    void visitInfiniteLoop(const InfiniteLoop* node) override
    {
        if (node->body().notNull())
            writeNamedConnection(node, node->body(), "body");
    }
    void visitInitializerList(const InitializerList* node) override
    {
        int i = 0;
        for (const auto& expr : node->expressions())
        {
            writeNamedConnection(node, expr, "initlist[" + std::to_string(i++) + "]");
        }
    }
    void visitLabel(const Label* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
    }
    void visitScopedAnnotatedSymbol(const ScopedAnnotatedSymbol* node) override {}
    void visitSelect(const Select* node) override
    {
        writeNamedConnection(node, node->expression(), "expr");
        if (node->cases().notNull())
            writeNamedConnection(node, node->cases(), "cases");
    }
    void visitSubCall(const SubCall* node) override
    {
        writeNamedConnection(node, node->label(), "label");
    }
    void visitSubReturn(const SubReturn* node) override {}
    void visitSymbol(const Symbol* node) override {}
    void visitUDTArrayDecl(const UDTArrayDecl* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        writeNamedConnection(node, node->dims(), "dims");
        writeNamedConnection(node, node->udt(), "udt");
    }
    void visitUDTDecl(const UDTDecl* node) override
    {
        writeNamedConnection(node, node->typeName(), "typeName");
        writeNamedConnection(node, node->body(), "body");
    }
    void visitUDTDeclBody(const UDTDeclBody* node) override
    {
        int i = 0;
        for (const auto& varDecl : node->varDeclarations())
            writeNamedConnection(node, varDecl, "varDecl[" + std::to_string(i++) + "]");

        i = 0;
        for (const auto& arrayDecl : node->arrayDeclarations())
            writeNamedConnection(node, arrayDecl, "arrayDecl[" + std::to_string(i++) + "]");
    }
    void visitUDTFieldOuter(const UDTFieldOuter* node) override
    {
        writeNamedConnection(node, node->left(), "left");
        writeNamedConnection(node, node->right(), "right");
    }
    void visitUDTFieldInner(const UDTFieldInner* node) override
    {
        writeNamedConnection(node, node->left(), "left");
        writeNamedConnection(node, node->right(), "right");
    }
    void visitUDTFieldAssignment(const UDTFieldAssignment* node) override
    {
        writeNamedConnection(node, node->field(), "field");
        writeNamedConnection(node, node->expression(), "expr");
    }
    void visitUDTRef(const UDTRef* node) override {}
    void visitUDTVarDecl(const UDTVarDecl* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
        writeNamedConnection(node, node->udt(), "udt");
        if (node->initializer().notNull())
            writeNamedConnection(node, node->initializer(), "initializer");
    }
    void visitUnaryOp(const UnaryOp* node) override
    {
        writeNamedConnection(node, node->expr(), "expr");
    }
    void visitUntilLoop(const UntilLoop* node) override
    {
        writeNamedConnection(node, node->exitCondition(), "exitCondition");
        if (node->body().notNull())
            writeNamedConnection(node, node->body(), "body");
    }
    void visitVarAssignment(const VarAssignment* node) override
    {
        writeNamedConnection(node, node->variable(), "var");
        writeNamedConnection(node, node->expression(), "expr");
    }
    void visitVarRef(const VarRef* node) override
    {
        writeNamedConnection(node, node->symbol(), "symbol");
    }
    void visitWhileLoop(const WhileLoop* node) override
    {
        writeNamedConnection(node, node->continueCondition(), "continueCondition");
        if (node->body().notNull())
            writeNamedConnection(node, node->body(), "body");
    }

#define X(dbname, cppname)                                                    \
    void visit##dbname##Literal(const dbname##Literal* node) override {}      \
    void visit##dbname##VarDecl(const dbname##VarDecl* node) override         \
    {                                                                         \
        writeNamedConnection(node, node->symbol(), "symbol");                 \
        if (node->initializer().notNull())                                    \
            writeNamedConnection(node, node->initializer(), "initializer");   \
    }                                                                         \
    void visit##dbname##ArrayDecl(const dbname##ArrayDecl* node) override     \
    {                                                                         \
        writeNamedConnection(node, node->symbol(), "symbol");                 \
        writeNamedConnection(node, node->dims(), "dims");                     \
    }
    ODB_DATATYPE_LIST
#undef X

private:
    FILE* fp_;
    const NodeGUIDs* guids_;
};

// ----------------------------------------------------------------------------
class NameWriter : public GenericConstVisitor
{
public:
    void write(FILE* fp, const NodeGUIDs* guids, const Node* root)
    {
        fp_ = fp;
        guids_ = guids;
        root->accept(this);
    }

private:
    void writeName(const Node* node, const std::string& name)
    {
        std::string escaped = str::escape(name);
        fprintf(fp_, "N%d [label=\"%s\"];\n",
                guids_->get(node), escaped.c_str());
    }

    void visitBlock(const Block* node) override
    {
        const auto& stmnts = node->statements();
        fprintf(fp_, "N%d [shape=record, width=%f, label=\"", guids_->get(node), stmnts.size()*2.0);
        for (size_t i = 0; i != stmnts.size(); ++i)
        {
            if (i != 0)
                fprintf(fp_, " | ");
            fprintf(fp_, "<N%d> stmnt[%zu]", guids_->get(stmnts[i].get()), i);
        }
        fprintf(fp_, "\"];\n");
    }

    void visit(const Node* node) override
    {
        writeName(node, node->toString());
    }

private:
    FILE* fp_;
    const NodeGUIDs* guids_;
};

// ----------------------------------------------------------------------------
void dumpToDOT(FILE* fp, const Node* root)
{
    NodeGUIDs guids;
    guids.calculate(root);

    fprintf(fp, "digraph name {\n");
    ConnectionWriter conns;
    conns.write(fp, &guids, root);
    NameWriter names;
    names.write(fp, &guids, root);
    fprintf(fp, "}\n");
}
#endif
}
