#pragma once

#include "odb-compiler/astpost/Process.hpp"

namespace odb {
namespace astpost {

class ResolveArrayFuncAmbiguity : public Process
{
public:
    bool execute(ast::Node** root) override final;
};

}
}
