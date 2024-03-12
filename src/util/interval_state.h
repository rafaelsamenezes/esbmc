#ifndef ESBMC_INTERVAL_STATE_H
#define ESBMC_INTERVAL_STATE_H

#include <goto-programs/abstract-interpretation/interval_template.h>
#include <goto-programs/abstract-interpretation/wrapped_interval.h>
//#include <goto-programs/abstract-interpretation/bitwise_bounds.h>

/**
 * Interval State Manager. This class handles the use of intervals
 * to compute and optimize expressions.
 * @tparam II Integer Template type
 * @tparam FI Float Template type
 */
class interval_state
{
public:
  using integer_interval = wrapped_interval;
  // TODO: set a float interval
  using float_interval = wrapped_interval;

  using integer_map = std::unordered_map<std::string, integer_interval>;
  using float_map = std::unordered_map<std::string, float_interval>;

  interval_state(integer_map &im, float_map &fm) : im(im), fm(fm) {}

  void dump() const;

  /**
   * Computes an integer interval for an expression
   * @param e expression to be computed
   * @return an integer interval with the expression result
   */
  integer_interval compute_integer_expression(const expr2tc &e) const;

  /**
   * Computes a float interval for an expression
   * @param e expression to be computed
   * @return a float interval with the expression result
   */
  float_interval compute_float_expression(const expr2tc &e) const;

  /**
   * Casts a float interval into a typed integer interval
   * @param fi float interval
   * @param t destination type
   * @return integer interval resulted by casting fi into t
   */
  integer_interval from_float(const float_interval &fi, const type2tc &t) const;
  /**
   * Casts an integer interval into a typed float interval
   * @param ii integer interval
   * @param t destination type
   * @return float interval resulted by casting ii into t
   */
  float_interval from_integer(const integer_interval &ii, const type2tc &t) const;

  /**
   * Try to restrict all intervals such that the expression still holds.
   * @param e restriction to be applied
   */
  void assume(const expr2tc &e);

  /**
   * Optimizes a given expression using the known intervals
   * @param e expression to optimize
   */
  void optimize(expr2tc &e) const;

protected:
  integer_map &im;
  float_map &fm;
};

#endif 
