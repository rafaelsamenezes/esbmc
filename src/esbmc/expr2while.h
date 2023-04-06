#ifndef CPROVER_EXPR2WHILE_H
#define CPROVER_EXPR2WHILE_H

#include <map>
#include <set>
#include <util/c_qualifiers.h>
#include <util/expr.h>
#include <util/namespace.h>
#include <util/std_code.h>

#include <iostream>
#include <fstream>

int convert_to_while(
  contextt &context,
  std::string rapid_file_name);

/*std::string
expr2c(const exprt &expr, const namespacet &ns, bool fullname = false);
std::string
type2c(const typet &type, const namespacet &ns, bool fullname = false);*/

class expr2whilet
{
public:
  expr2whilet(const namespacet &_ns, std::string rapid_file_name)
    : ns(_ns)
  {
    rapid_file = std::ofstream(rapid_file_name);
  }
  virtual ~expr2whilet(){
    rapid_file.close();
  }

  virtual void convert(const typet &src);
  virtual void convert(const exprt &src);
  void convert_main(const codet &src);

 // void get_shorthands(const exprt &expr);

protected:
  const namespacet &ns;
  std::ofstream rapid_file;

//  const bool fullname;

  virtual void convert_rec(
    const typet &src,
    const c_qualifierst &qualifiers);

  static std::string indent_str(unsigned indent);

  std::set<exprt> symbols;
  std::map<irep_idt, exprt> shorthands;
  std::set<irep_idt> ns_collision;

  void get_symbols(const exprt &expr);
  std::string id_shorthand(const exprt &expr) const;

  void convert_typecast(const exprt &src, unsigned &precedence);

 // void convert_bitcast(const exprt &src, unsigned &precedence);

  //std::string
  //convert_implicit_address_of(const exprt &src, unsigned &precedence);

  void convert_binary(
    const exprt &src,
    const std::string &symbol,
    unsigned precedence,
    bool full_parentheses);

  // AYB not sure what this does, but don't think I need it
  //void convert_cond(const exprt &src, unsigned precedence);

  //std::string
  //convert_struct_member_value(const exprt &src, unsigned precedence);

  //void convert_array_member_value(const exprt &src, unsigned precedence);

  void convert_member(const exprt &src, unsigned precedence);

  //std::string
  //convert_pointer_object_has_type(const exprt &src, unsigned precedence);

  //void convert_array_of(const exprt &src, unsigned precedence);

  void convert_trinary(
    const exprt &src,
    const std::string &symbol1,
    const std::string &symbol2,
    unsigned precedence);

  void convert_overflow(const exprt &src, unsigned &precedence);

  /*void convert_quantifier(
    const exprt &src,
    const std::string &symbol,
    unsigned precedence);*/

  // AYB not sure what this does, but don't think I need it
  //void convert_with(const exprt &src, unsigned precedence);

  void convert_index(const exprt &src, unsigned precedence);

  void convert_unary(
    const exprt &src,
    const std::string &symbol,
    unsigned precedence);

  void convert_unary_post(
    const exprt &src,
    const std::string &symbol,
    unsigned precedence);

  void convert_function(
    const exprt &src,
    const std::string &symbol,
    unsigned precedence);

  //void convert_Hoare(const exprt &src);

  void convert_code(const codet &src, unsigned indent);
  void convert_code_label(const code_labelt &src, unsigned indent);
  void
  convert_code_switch_case(const code_switch_caset &src, unsigned indent);
 // void convert_code_asm(const codet &src, unsigned indent);
  void convert_code_assign(const codet &src, unsigned indent);
  void convert_code_free(const codet &src, unsigned indent);
  void convert_code_init(const codet &src, unsigned indent);
  void convert_code_ifthenelse(const codet &src, unsigned indent);
  void convert_code_for(const codet &src, unsigned indent);
  void convert_code_while(const codet &src, unsigned indent);
  void convert_code_dowhile(const codet &src, unsigned indent);
  void convert_code_block(const codet &src, unsigned indent);
  void convert_code_expression(const codet &src, unsigned indent);
  //void convert_code_return(const codet &src, unsigned indent);
  //void convert_code_goto(const codet &src, unsigned indent);
  //void convert_code_gcc_goto(const codet &src, unsigned indent);
  void convert_code_assume(const codet &src, unsigned indent);
  void convert_code_assert(const codet &src, unsigned indent);
  //void convert_code_break(const codet &src, unsigned indent);
  void convert_code_switch(const codet &src, unsigned indent);
  //void convert_code_continue(const codet &src, unsigned indent);
  void convert_code_decl(const codet &src, unsigned indent);
  void convert_code_decl_block(const codet &src, unsigned indent);
  void convert_code_dead(const codet &src, unsigned indent);
  //void convert_code_function_call(const code_function_callt &src, unsigned indent);
  void convert_code_lock(const codet &src, unsigned indent);
  void convert_code_unlock(const codet &src, unsigned indent);

  virtual void convert(const exprt &src, unsigned &precedence);

  void convert_function_call(const exprt &src, unsigned &precedence);
  void convert_malloc(const exprt &src, unsigned &precedence);
 // void convert_realloc(const exprt &src, unsigned &precedence);
 // void convert_alloca(const exprt &src, unsigned &precedence);
  void convert_nondet(const exprt &src, unsigned &precedence);
//  std::string
//  convert_statement_expression(const exprt &src, unsigned &precedence);

  virtual void convert_symbol(const exprt &src, unsigned &precedence);
 // void convert_predicate_symbol(const exprt &src, unsigned &precedence);
 // std::string
 // convert_predicate_next_symbol(const exprt &src, unsigned &precedence);
  void convert_nondet_symbol(const exprt &src, unsigned &precedence);
  void convert_quantified_symbol(const exprt &src, unsigned &precedence);
  void convert_nondet_bool(const exprt &src, unsigned &precedence);
  void convert_object_descriptor(const exprt &src, unsigned &precedence);
  virtual void convert_constant(const exprt &src, unsigned &precedence);

  //void convert_norep(const exprt &src, unsigned &precedence);

  void convert_struct_union_body(
    const exprt::operandst &operands,
    const struct_union_typet::componentst &components);
  virtual void convert_struct(const exprt &src, unsigned &precedence);
  void convert_union(const exprt &src, unsigned &precedence);
//  void convert_array(const exprt &src, unsigned &precedence);
//  void convert_array_list(const exprt &src, unsigned &precedence);
};

#endif
