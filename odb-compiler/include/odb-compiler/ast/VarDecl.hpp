#pragma once

#include "odb-compiler/config.hpp"
#include "odb-compiler/ast/Statement.hpp"
#include "odb-compiler/ast/Datatypes.hpp"
#include "odb-sdk/MaybeNull.hpp"

namespace odb {
namespace ast {

class InitializerList;
class UDTRef;
class ScopedAnnotatedSymbol;
class Symbol;

class ODBCOMPILER_PUBLIC_API VarDecl : public Statement
{
public:
    VarDecl(ScopedAnnotatedSymbol* symbol, InitializerList* initializer, SourceLocation* location);
    VarDecl(ScopedAnnotatedSymbol* symbol, SourceLocation* location);

    ScopedAnnotatedSymbol* symbol() const;
    MaybeNull<InitializerList> initializer() const;

    void setInitializer(InitializerList* expression);

protected:
    Reference<ScopedAnnotatedSymbol> symbol_;
    Reference<InitializerList> initializer_;
};

#define X(dbname, cppname)                                                    \
class dbname##VarDecl : public VarDecl                                        \
{                                                                             \
public:                                                                       \
    dbname##VarDecl(ScopedAnnotatedSymbol* symbol, InitializerList* initializer, SourceLocation* location);\
    dbname##VarDecl(ScopedAnnotatedSymbol* symbol, SourceLocation* location); \
                                                                              \
    std::string toString() const override;                                    \
    void accept(Visitor* visitor) override;                                   \
    void accept(ConstVisitor* visitor) const override;                        \
    void swapChild(const Node* oldNode, Node* newNode) override;              \
                                                                              \
protected:                                                                    \
    Node* duplicateImpl() const override;                                     \
};
ODB_DATATYPE_LIST
#undef X

class ODBCOMPILER_PUBLIC_API UDTVarDecl : public VarDecl
{
public:
    UDTVarDecl(ScopedAnnotatedSymbol* symbol, UDTRef* udt, InitializerList* initializer, SourceLocation* location);
    UDTVarDecl(ScopedAnnotatedSymbol* symbol, UDTRef* udt, SourceLocation* location);

    UDTRef* udt() const;

    std::string toString() const override;
    void accept(Visitor* visitor) override;
    void accept(ConstVisitor* visitor) const override;
    void swapChild(const Node* oldNode, Node* newNode) override;

protected:
    Node* duplicateImpl() const override;

private:
    Reference<UDTRef> udt_;
};

}
}
