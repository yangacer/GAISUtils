#ifndef _RVISITOR_H
#define _RVISITOR_H

#include "loki/Visitor.h"
#include "fieldTypes.h"
#include <string>

// forward decl
template<typename T>
class field;

typedef Loki::CyclicVisitor<void, SupportedTypes > StrVisitor;

struct StrOperator : public StrVisitor
{
	StrVisitor::ReturnType Visit(field<std::string> &f)
	{}

};

#endif
