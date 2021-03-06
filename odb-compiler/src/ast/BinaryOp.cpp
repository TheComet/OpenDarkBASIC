#include "odb-compiler/ast/BinaryOp.hpp"
#include "odb-compiler/ast/SourceLocation.hpp"
#include "odb-compiler/ast/Visitor.hpp"

namespace odb::ast {

// ----------------------------------------------------------------------------
BinaryOp::BinaryOp(BinaryOpType op, Expression* lhs, Expression* rhs, SourceLocation* location)
    : Expression(location)
    , lhs_(lhs)
    , rhs_(rhs)
    , op_(op)
{
    lhs->setParent(this);
    rhs->setParent(this);
}

// ----------------------------------------------------------------------------
BinaryOpType BinaryOp::op() const
{
    return op_;
}

// ----------------------------------------------------------------------------
Expression* BinaryOp::lhs() const
{
    return lhs_;
}

// ----------------------------------------------------------------------------
Expression* BinaryOp::rhs() const
{
    return rhs_;
}

// ----------------------------------------------------------------------------
std::string BinaryOp::toString() const
{
    return std::string("BinaryOp(") + binaryOpTypeEnumString(op_) + ")";
}

// ----------------------------------------------------------------------------
void BinaryOp::accept(Visitor* visitor)
{
    visitor->visitBinaryOp(this);
    lhs_->accept(visitor);
    rhs_->accept(visitor);
}
void BinaryOp::accept(ConstVisitor* visitor) const
{
    visitor->visitBinaryOp(this);
    lhs_->accept(visitor);
    rhs_->accept(visitor);
}

// ----------------------------------------------------------------------------
void BinaryOp::swapChild(const Node* oldNode, Node* newNode)
{
    if (lhs_ == oldNode)
        lhs_ = dynamic_cast<Expression*>(newNode);
    else if (rhs_ == oldNode)
        rhs_ = dynamic_cast<Expression*>(newNode);
    else
        assert(false);

    newNode->setParent(this);
}

// ----------------------------------------------------------------------------
Node* BinaryOp::duplicateImpl() const
{
    return new BinaryOp(
        op_,
        lhs_->duplicate<Expression>(),
        rhs_->duplicate<Expression>(),
        location());
}

}
