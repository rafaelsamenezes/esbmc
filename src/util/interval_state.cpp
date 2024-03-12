

#include "interval_state.h"
#include <irep2/irep2_expr.h>

interval_state::integer_interval interval_state::compute_integer_expression(const expr2tc &e) const
{
  assert(is_signedbv_type(e) || is_unsignedbv_type(e));
  interval_state::integer_interval top(e->type);
  switch (e->expr_id)
  {
    // Constants
  case expr2t::constant_int_id:
  {
    const BigInt &value = to_constant_int2t(e).value;
    top.make_ge_than(value);
    top.make_le_than(value);
    return top;
  }

  case expr2t::constant_bool_id:
  {
    top.set_lower(to_constant_bool2t(e).is_true());
    top.set_upper(to_constant_bool2t(e).is_true());
    return top;
  }

  case expr2t::constant_fixedbv_id:
  case expr2t::constant_floatbv_id:
    abort();
    break;

    // Symbol
  case expr2t::symbol_id:
    //TODO: result = get_interval_from_symbol<T>(to_symbol2t(e));
    break;

    // Arithmetic
  case expr2t::add_id:
  case expr2t::sub_id:
  case expr2t::mul_id:
  case expr2t::div_id:
  case expr2t::modulus_id:
  {
    const auto &arith_op = dynamic_cast<const arith_2ops &>(*e);
    wrapped_interval lhs = compute_integer_expression(arith_op.side_1);
    wrapped_interval rhs = compute_integer_expression(arith_op.side_2);

    if (is_add2t(e))
      return lhs + rhs;

    else if (is_sub2t(e))
      return lhs - rhs;

    else if (is_mul2t(e))
      return lhs * rhs;

    else if (is_div2t(e))
      return lhs / rhs;

    else if (is_modulus2t(e))
      return lhs % rhs;

    log_error("Unsupported expression");
    e->dump();
    abort();
  }
  case expr2t::neg_id:
    return -compute_integer_expression(to_neg2t(e).value);

  case expr2t::bitnot_id:
    return integer_interval::bitnot(compute_integer_expression(to_bitnot2t(e).value));

  case expr2t::shl_id:
  case expr2t::ashr_id:
  case expr2t::lshr_id:
  case expr2t::bitor_id:
  case expr2t::bitand_id:
  case expr2t::bitxor_id:
  case expr2t::bitnand_id:
  case expr2t::bitnor_id:
  case expr2t::bitnxor_id:
  {
    const auto &bit_op = dynamic_cast<const bit_2ops &>(*e);
    integer_interval lhs = compute_integer_expression(bit_op.side_1);
    integer_interval rhs = compute_integer_expression(bit_op.side_2);
    lhs.type = bit_op.side_1->type;
    rhs.type = bit_op.side_2->type;
    if (is_shl2t(e))
      return integer_interval::left_shift(lhs, rhs);

    else if (is_ashr2t(e))
      return integer_interval::arithmetic_right_shift(lhs, rhs);

    else if (is_lshr2t(e))
      return integer_interval::logical_right_shift(lhs, rhs);

    else if (is_bitor2t(e))
      return lhs | rhs;

    else if (is_bitand2t(e))
      return lhs & rhs;
    else if (is_bitxor2t(e))
      return lhs ^ rhs;

    else if (is_bitnand2t(e))
      return integer_interval::bitnot(lhs & rhs);
    else if (is_bitnor2t(e))
      return integer_interval::bitnot(lhs | rhs);
    else if (is_bitnxor2t(e))
      return integer_interval::bitnot(lhs ^ rhs);
    log_error("Unsupported expression");
    e->dump();
    abort();
  }
    // Logic
  case expr2t::not_id:
    return integer_interval::invert_bool(compute_integer_expression(to_not2t(e).value));



    // Others

default:
  log_warning("Don't know how to process interval expression. Returning TOP");
  break;
}
  return top;
}