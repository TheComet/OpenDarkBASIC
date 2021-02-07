#include "odb-compiler/ast/Exit.hpp"
#include "odb-compiler/ast/SourceLocation.hpp"
#include "odb-compiler/ast/Visitor.hpp"

namespace odb::ast {

// ----------------------------------------------------------------------------
Exit::Exit(SourceLocation* location) :
    Statement(location)
{
}

// ----------------------------------------------------------------------------
void Exit::accept(Visitor* visitor)
{
    visitor->visitBreak(this);
}
void Exit::accept(ConstVisitor* visitor) const
{
    visitor->visitExit(this);
}

// ----------------------------------------------------------------------------
void Exit::swapChild(const Node* oldNode, Node* newNode)
{
    assert(false);
}

// ----------------------------------------------------------------------------
Node* Break::duplicateImpl() const
{
    return new Break(location());
}

}