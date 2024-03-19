#ifndef ESBMC_INTERVAL_STATE_H
#define ESBMC_INTERVAL_STATE_H

#include "c_types.h"
#include "irep2/irep2_utils.h"
#include <goto-programs/abstract-interpretation/interval_template.h>
#include <goto-programs/abstract-interpretation/wrapped_interval.h>
#include <irep2/irep2_expr.h>
#include <variant>
//#include <goto-programs/abstract-interpretation/bitwise_bounds.h>

/**
 * Interval State Manager. This class handles the use of intervals
 * to compute and optimize expressions.
 * @tparam II Integer Template type
 * @tparam FI Float Template type
 */
template <typename II, typename FI>
class interval_state
{
public:
  using interval = std::variant<II, FI>;
  using interval_ref = std::shared_ptr<interval>;
  using interval_map = std::unordered_map<std::string, interval_ref>;

  interval_state() = default;

  /**
   * Dumps all intervals that the state holds
   */
  void dump() const;

  /**
   * Computes an integer interval for an expression
   * @param e expression to be computed
   * @return an integer interval with the expression result
   */
  interval compute_expression(const expr2tc &e) const
  {
    assert(is_signedbv_type(e) || is_unsignedbv_type(e) || is_bool_type(e));

    switch (e->expr_id)
      {
        // Constants
      case expr2t::constant_int_id:
        {
          const BigInt &value = to_constant_int2t(e).value;
          II ii(e->type);
          ii.make_ge_than(value);
          ii.make_le_than(value);
          return ii;
        }

      case expr2t::constant_bool_id:
        {
          II ii(e->type);
          ii.set_lower(to_constant_bool2t(e).is_true());
          ii.set_upper(to_constant_bool2t(e).is_true());
          return ii;
        }

      case expr2t::constant_fixedbv_id:
      case expr2t::constant_floatbv_id:
        abort();
        break;

        // Symbol
      case expr2t::symbol_id:
        abort();
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
          II lhs = std::get<II>(compute_expression(arith_op.side_1));
          II rhs = std::get<II>(compute_expression(arith_op.side_2));

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
        return -std::get<II>(compute_expression(to_neg2t(e).value));

      case expr2t::bitnot_id:
        return II::bitnot(std::get<II>(compute_expression(to_bitnot2t(e).value)));

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
          II lhs = std::get<II>(compute_expression(bit_op.side_1));
          II rhs = std::get<II>(compute_expression(bit_op.side_2));
          lhs.type = bit_op.side_1->type;
          rhs.type = bit_op.side_2->type;
          if (is_shl2t(e))
            return II::left_shift(lhs, rhs);

          else if (is_ashr2t(e))
            return II::arithmetic_right_shift(lhs, rhs);

          else if (is_lshr2t(e))
            return II::logical_right_shift(lhs, rhs);

          else if (is_bitor2t(e))
            return lhs | rhs;

          else if (is_bitand2t(e))
            return lhs & rhs;
          else if (is_bitxor2t(e))
            return lhs ^ rhs;

          else if (is_bitnand2t(e))
            return II::bitnot(lhs & rhs);
          else if (is_bitnor2t(e))
            return II::bitnot(lhs | rhs);
          else if (is_bitnxor2t(e))
            return II::bitnot(lhs ^ rhs);
          log_error("Unsupported expression");
          e->dump();
          abort();
        }
        // Logic
      case expr2t::not_id:
        return II::invert_bool(
                               std::get<II>(compute_expression(to_not2t(e).value)));

      case expr2t::or_id:        
      case expr2t::and_id:
      case expr2t::xor_id:
      case expr2t::implies_id:
        {
          II result(get_bool_type());

          // We can rely on our simplifier to simplify these operations
          expr2tc cpy = e->clone();
          auto &logic_op = dynamic_cast<logic_2ops &>(*cpy);
          const tvt &rhs = eval_boolean_expression(logic_op.side_2);
          const tvt &lhs = eval_boolean_expression(logic_op.side_1);

          if (lhs.is_known())
            logic_op.side_1 =
              lhs.is_false() ? gen_false_expr() : gen_true_expr();

          if (rhs.is_known())
            logic_op.side_2 =
              rhs.is_false() ? gen_false_expr() : gen_true_expr();

          const expr2tc &simplified = logic_op.do_simplify();
          if (is_constant_bool2t(simplified))
            {
              result.set_upper(to_constant_bool2t(simplified).value);
              result.set_lower(to_constant_bool2t(simplified).value);
            }

          return result;
        }

      case expr2t::if_id:
        {
          const II cond = std::get<II>(compute_expression(to_if2t(e).cond));
          const II lhs = std::get<II>(compute_expression(to_if2t(e).true_value));
          const II rhs = std::get<II>(compute_expression(to_if2t(e).false_value));
          return II::ternary_if(cond, lhs, rhs);
        }
        // Others
      case expr2t::typecast_id:
        {
          // Special case: boolean
          if (is_bool_type(to_typecast2t(e).type))
            {
              tvt truth = eval_boolean_expression(to_typecast2t(e).from);
              II result(e->type);
              result.set_lower(0);
              result.set_upper(1);
              
              if (truth.is_true())
                result.set_lower(1);
              
              if (truth.is_false())
                result.set_upper(0);

              return result;
            }
          const II inner = std::get<II>(compute_expression(to_typecast2t(e).from));
          return II::cast(inner, to_typecast2t(e).type);
        }

      default:
        log_warning("Don't know how to process interval expression. Returning TOP");
        break;
      }
    //gave-up
    log_debug("interval", "Couldn't compute interval for expr: {}", *e);
    std::variant<II, FI> top = II(e->type);
    return top;
  }

  /**
   * Try to restrict all intervals such that the expression still holds.
   * @param e restriction to be applied
   */
  void restrict(const expr2tc &e);

  /**
   * Optimizes a given expression using the known intervals
   * @param e expression to optimize
   */
  void optimize(expr2tc &e) const;

protected:
  /**
   * Casts FI domain into II domain
   * @param i integer interval to be converted into FI
   */
  FI from_integer_interval(const II &i) const;
  /**
   * Casts II domain into FI domain
   * @param f float interval to be converted into II
   */
  II from_float_interval(const FI &f) const;

  tvt eval_boolean_expression(const expr2tc &expr) const
  {
    assert(is_bool_type(expr));
    II interval = std::get<II>(compute_expression(expr));
    // If the interval does not contain zero then it's always true
    if (!interval.contains(0))
      return tvt(tvt::TV_TRUE);
    
    // If it does contain zero and its singleton then it's always false
    if (interval.singleton())
      return tvt(tvt::TV_FALSE);
    
    return tvt(tvt::TV_UNKNOWN);
  }

  /// State
  interval_map im;
};
#endif


