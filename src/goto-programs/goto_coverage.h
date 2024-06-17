#include <goto-programs/goto_functions.h>
#include <goto-programs/loop_unroll.h>
#include <langapi/language_util.h>
#include <unordered_set>

class goto_coveraget
{
public:
  explicit goto_coveraget(const namespacet &ns, goto_functionst &goto_functions)
    : ns(ns), goto_functions(goto_functions)
  {
    target_num = -1;
  };
  explicit goto_coveraget(
    const namespacet &ns,
    goto_functionst &goto_functions,
    const std::string filename)
    : ns(ns), goto_functions(goto_functions), filename(filename)
  {
    target_num = -1;
  };
  // add an assert(0)
  // - at the beginning of each GOTO program
  // - at the beginning of each branch body
  // - before each END_FUNCTION statement
  void add_false_asserts();

  void insert_assert(
    goto_programt &goto_program,
    goto_programt::targett &it,
    const expr2tc &guard);

  // customize comment
  void insert_assert(
    goto_programt &goto_program,
    goto_programt::targett &it,
    const expr2tc &guard,
    const std::string &idf);

  // convert every assertion to an assert(0)
  void make_asserts_false();

  // convert every assertion to an assert(1)
  void make_asserts_true();

  // condition cov
  void gen_cond_cov();
  exprt gen_no_eq_expr(const exprt &lhs, const exprt &rhs);
  exprt gen_and_expr(const exprt &lhs, const exprt &rhs);
  exprt gen_not_expr(const exprt &expr);
  int get_total_instrument() const;
  int get_total_assert_instance() const;
  std::unordered_set<std::string> get_total_cond_assert() const;
  std::string get_filename_from_path(std::string path);

protected:
  // turn a OP b OP c into a list a, b, c
  static void
  collect_operands(const exprt &expr, std::list<exprt> &operands, bool &flag);
  static void
  collect_operators(const exprt &expr, std::list<std::string> &operators);
  static void collect_atom_operands(const exprt &expr, std::set<exprt> &atoms);
  exprt handle_single_guard(exprt &guard);
  void add_cond_cov_assert(
    const exprt &top_ptr,
    const exprt &pre_cond,
    goto_programt &goto_program,
    goto_programt::instructiont::targett &it);
  void gen_cond_cov_assert(
    exprt top_ptr,
    exprt pre_cond,
    goto_programt &goto_program,
    goto_programt::instructiont::targett &it);

  namespacet ns;
  goto_functionst &goto_functions;
  std::string filename;
  int target_num;
};
