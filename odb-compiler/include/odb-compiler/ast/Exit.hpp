#pragma once

#include "odb-compiler/config.hpp"
#include "odb-compiler/ast/Statement.hpp"

namespace odb::ast {

class ODBCOMPILER_PUBLIC_API Exit : public Statement
{
public:
    Exit(SourceLocation* location);

    std::string toString() const override;
    void accept(Visitor* visitor) override;
    void accept(ConstVisitor* visitor) const override;
    void swapChild(const Node* oldNode, Node* newNode) override;

protected:
    Node* duplicateImpl() const override;
};

}
