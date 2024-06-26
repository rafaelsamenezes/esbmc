/*******************************************************************\

Module: C++ Language Type Checking

Author: Daniel Kroening, kroening@cs.cmu.edu

\*******************************************************************/

#include <cassert>
#include <cpp/cpp_declarator.h>
#include <iostream>

void cpp_declaratort::output(std::ostream &out) const
{
  out << "  name: " << name().pretty() << std::endl;
  out << "  type: " << type().pretty() << std::endl;
  out << "  value: " << value().pretty() << std::endl;
  out << "  init_args: " << init_args().pretty() << std::endl;
  out << "  method_qualifier: " << method_qualifier().pretty() << std::endl;
}

typet cpp_declaratort::merge_type(const typet &declaration_type) const
{
  typet dest_type = type();

  if (declaration_type.id() == "cpp-cast-operator")
    return dest_type;

  typet *p = &dest_type;

  // walk down subtype until we hit nil
  while (true)
  {
    typet &t = *p;
    if (t.is_nil())
    {
      t = declaration_type;
      break;
    }

    assert(t.id() != "");
    p = &t.subtype();
  }

  return dest_type;
}
