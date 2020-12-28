#pragma once

#include "odb-compiler/config.hpp"
#include "odb-compiler/ast/Visitor.hpp"
#include <string>
#include <unordered_set>

namespace odb {
namespace ast {
class Node;
}
namespace astpost {

class Process
{
public:
    virtual bool execute(ast::Node** root) = 0;
};

}
}
