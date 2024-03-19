#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include <util/interval_state.h>
#include <irep2/irep2_utils.h>
#include <limits>

using II = wrapped_interval;
using FI = interval_templatet<BigInt>;

inline void check_singleton(const expr2tc &e, const int expected)
{
  interval_state<II,FI> is;

  std::variant<II, FI> result = is.compute_expression(e);
  REQUIRE(result.index() == 0);
  II &wi = std::get<0>(result);
  CAPTURE(wi.get_lower().to_int64(), wi.get_upper().to_int64(), expected);
  REQUIRE(wi.contains(expected));
  REQUIRE(wi.singleton());
}

SCENARIO("interval state can compute singletons", "[core][interval-analysis][brute-force]")
{
  const type2tc int_type =  get_int_type(sizeof(char) * 8);
  const type2tc uint_type =  get_uint_type(sizeof(char) * 8);

  SECTION("Boolean")
  {
    SECTION("Initialization")
    {
      check_singleton(constant_bool2tc(false), 0);
      check_singleton(constant_bool2tc(true), 1);
    }
    for(char c1 = 0; c1 <= 1; c1++)
    {
      const expr2tc op1 = constant_bool2tc(c1);
      CAPTURE((int)c1);       
      SECTION("Not")
        {
   
          check_singleton(not2tc(op1), !c1);
        }
      for (char c2 = 0; c2 <= 1; c2++)
      {
        const expr2tc op2 = constant_bool2tc(c2);
        CAPTURE((int)c2);
        SECTION("Logic Operators")
        {
          check_singleton(and2tc(op1,op2), c1 && c2);
          check_singleton(or2tc(op1, op2), c1 || c2);
          check_singleton(xor2tc(op1, op2), c1 ^ c2);
          check_singleton(implies2tc(op1, op2), !c1 || c2);          
        }
      }
    }
  }


}

SCENARIO(
  "Interval state can compute intervals",
  "[core][interval-analysis][brute-force]")
{
  const type2tc int_type =  get_int_type(sizeof(char) * 8);
  const type2tc uint_type = get_uint_type(sizeof(char) * 8);


  SECTION("Symbol")
  {
  }

  
  
}
