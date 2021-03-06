#pragma once

#include "odb-compiler/config.hpp"
#include "odb-compiler/ast/LValue.hpp"

namespace odb::ast {

class UDTFieldOuter : public LValue
{
public:
    UDTFieldOuter(Expression* left, LValue* right, SourceLocation* location);

    Expression* left() const;
    LValue* right() const;

    std::string toString() const override;
    void accept(Visitor* visitor) override;
    void accept(ConstVisitor* visitor) const override;
    void swapChild(const Node* oldNode, Node* newNode) override;

protected:
    Node* duplicateImpl() const override;

private:
    Reference<Expression> left_;
    Reference<LValue> right_;
};

class UDTFieldInner : public LValue
{
public:
    UDTFieldInner(LValue* left, LValue* right, SourceLocation* location);

    LValue* left() const;
    LValue* right() const;

    std::string toString() const override;
    void accept(Visitor* visitor) override;
    void accept(ConstVisitor* visitor) const override;
    void swapChild(const Node* oldNode, Node* newNode) override;

protected:
    Node* duplicateImpl() const override;

private:
    Reference<LValue> left_;
    Reference<LValue> right_;
};

}
