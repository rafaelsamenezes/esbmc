/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/

#include <assert.h>

#include <map>

#include <irep2.h>
#include <migrate.h>
#include "prop_conv.h"

bool prop_convt::get_bool(const exprt &expr, tvt &value) const
{

  expr2tc new_expr;
  migrate_expr(expr, new_expr);

  cachet::const_iterator cache_result=cache.find(new_expr);
  if(cache_result==cache.end()) return true;

  value=l_get(cache_result->second);
  return false;
}

literalt prop_convt::convert(const expr2tc &expr)
{

  std::pair<cachet::iterator, bool> result=
    cache.insert(std::pair<expr2tc, literalt>(expr, literalt()));

  if(!result.second)
    return result.first->second;

  literalt literal = convert_expr(expr);

  // insert into cache

  result.first->second=literal;

  return literal;
}

void prop_convt::ignoring(const expr2tc &expr)
{
  // fall through

  std::string msg="warning: ignoring "+expr->pretty();

  print(2, msg);
}

exprt prop_convt::get(const exprt &expr) const
{
  exprt dest;

  dest.make_nil();

  tvt value;

  if(expr.type().is_bool() &&
     !get_bool(expr, value))
  {
    switch(value.get_value())
    {
     case tvt::TV_TRUE:  dest.make_true(); return dest;
     case tvt::TV_FALSE: dest.make_false(); return dest;
     case tvt::TV_UNKNOWN: dest.make_false(); return dest; // default
    }
  }

  return dest;
}

void prop_convt::convert_smt_type(const type2t &type, void *&arg) const
{
  std::cerr << "Unhandled SMT conversion for type ID " << type.type_id <<
               std::endl;
  abort();
}

void prop_convt::convert_smt_expr(const expr2t &expr, void *&arg)
{
  std::cerr << "Unhandled SMT conversion for expr ID " << expr.expr_id <<
               std::endl;
  abort();
}

void prop_convt::set_equal(literalt a, literalt b)
{
  bvt bv;
  bv.resize(2);
  bv[0]=a;
  bv[1]=lnot(b);
  lcnf(bv);
  bv[0]=lnot(a);
  bv[1]=b;
  lcnf(bv);
}
