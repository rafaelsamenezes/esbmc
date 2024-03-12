#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include <util/interval_state.h>
#include <irep2/irep2_utils.h>
#include <limits>

inline void check_singleton(const expr2tc &e, const int expected) {
  interval_state::integer_map im;
  interval_state::float_map fm;
  interval_state is(im,fm);

  wrapped_interval wi = is.compute_integer_expression(e);
  CAPTURE(wi.get_lower().to_int64(), wi.get_upper().to_int64(), expected);
  REQUIRE(wi.contains(expected));
  REQUIRE(wi.singleton());
}

SCENARIO("interval state can compute singletons", "[core][interval-analysis][brute-force]")
{
  const type2tc int_type =  get_int_type(sizeof(char) * 8);
  const type2tc uint_type =  get_uint_type(sizeof(char) * 8);

#if 0
  SECTION("Boolean")
  {
    check_singleton(constant_bool2tc(false), 0);
    check_singleton(constant_bool2tc(true), 1);
    for(char c1 = 0; c1 <= 1; c1++)
    {
      CAPTURE((int)c1);
      const expr2tc op1 = constant_bool2tc(c1);
      check_singleton(not2tc(op1), !c1);
      for (char c2 = 0; c2 <= 1; c2++)
      {
        CAPTURE((int)c2);
        const expr2tc op2 = constant_bool2tc(c2);
        check_singleton(or2tc(op1,op2), c1 || c2);
      }
    }
  }
#endif

  SECTION("Signed")
  {
    for(char c1 = CHAR_MIN; c1 < CHAR_MAX; c1++)
    {
      CAPTURE((int)c1);
      const expr2tc op1 = constant_int2tc(int_type, c1);
      check_singleton(op1, c1);
      check_singleton(neg2tc(int_type, op1), (char)-c1);
      check_singleton(bitnot2tc(int_type, op1), ~c1);
      for (char c2 = CHAR_MIN; c2 < CHAR_MAX; c2++)
      {
        CAPTURE((int)c2);

        const expr2tc op2 = constant_int2tc(int_type, c2);
        check_singleton(add2tc(int_type, op1, op2), c1 + c2);
        check_singleton(sub2tc(int_type, op1, op2), c1 - c2);
        check_singleton(mul2tc(int_type, op1, op2), c1 * c2);
        // TODO:
        //check_singleton(shl2tc(int_type, op1, op2), c1 << c2);
        //check_singleton(ashr2tc(int_type, op1, op2), (char)c1 >> c2);
        check_singleton(bitor2tc(int_type, op1, op2), c1 | c2);
        check_singleton(bitand2tc(int_type, op1, op2), c1 & c2);
        check_singleton(bitxor2tc(int_type, op1, op2), c1 ^ c2);
        if (c2)
        {
          check_singleton(div2tc(int_type, op1, op2), c1 / c2);
          check_singleton(modulus2tc(int_type, op1, op2), c1 % c2);
        }
      }
    }
  }
}