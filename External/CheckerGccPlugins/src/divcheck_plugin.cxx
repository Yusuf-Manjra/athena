// $Id$
/**
 * @file CheckerGccPlugins/src/divcheck_plugin.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2015
 * @brief Check for redundant divisions.
 */


#include "checker_gccplugins.h"
#include "tree.h"
#if GCC_VERSION >= 4009
#include "stor-layout.h"
#endif
#include "function.h"
#include "basic-block.h"
#include "coretypes.h"
#include "is-a.h"
#include "predict.h"
#include "internal-fn.h"
#include "tree-ssa-alias.h"
#if GCC_VERSION >= 4009
# include "gimple-expr.h"
#endif
#include "gimple.h"
#if GCC_VERSION >= 4009
# include "gimple-iterator.h"
# include "tree-ssa-loop.h"
#endif
#include "cp/cp-tree.h"
#include "diagnostic.h"
#if GCC_VERSION >= 4009
# include "context.h"
#endif
#include "tree-pass.h"
#include "gimple-pretty-print.h"
#if GCC_VERSION >= 4009
# include "print-tree.h"
# include "tree-cfg.h"
#endif
#include "cfgloop.h"
#include "tree-ssa-operands.h"
#include "real.h"
#if GCC_VERSION >= 4009
# include "tree-phinodes.h"
# include "gimple-ssa.h"
# include "ssa-iterators.h"
#else
# include "tree-flow.h"
#endif


using CheckerGccPlugins::gimplePtr;


namespace {


const char* url = "<https://twiki.cern.ch/twiki/bin/view/AtlasComputing/CheckerGccPlugins#divcheck_plugin>";



#if GCC_VERSION >= 4009

const pass_data divcheck_pass_data =
{
  GIMPLE_PASS, /* type */
  "divcheck", /* name */
  0, /* optinfo_flags */
#if GCC_VERSION < 5000
  false, /* has_gate */
  true, /* has_execute */
#endif
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0  /* todo_flags_finish */
};


class divcheck_pass : public gimple_opt_pass
{
public:
  divcheck_pass (gcc::context* ctxt)
    : gimple_opt_pass (divcheck_pass_data, ctxt)
  {}

#if GCC_VERSION >= 5000
  virtual unsigned int execute (function* fun) override
  { return divcheck_execute(fun); }
#else
  virtual unsigned int execute () override
  { return divcheck_execute (cfun); }
#endif

  unsigned int divcheck_execute (function* fun);

  virtual opt_pass* clone() override { return new divcheck_pass(*this); }
};

#else

class divcheck_pass
{
public:
  static unsigned int execute ()
  { return divcheck_execute (cfun); }

  static unsigned int divcheck_execute (function* fun);
};


opt_pass divcheck_pass_data =
{
  GIMPLE_PASS, /* type */
  "divcheck", /* name */
  0, /* optinfo_flags */
  0, /* gate */
  divcheck_pass::execute, /* execute */
  0, /* sub */
  0, /* next */
  0, /* static_pass_number */
  TV_NONE, /* tv_id */
  0, /* properties_required */
  0, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  0  /* todo_flags_finish */
};


#endif


// Test to see if a constant has an exact inverse.
bool has_exact_inverse (tree val)
{
  if (TREE_CODE (val) == REAL_CST) {
    REAL_VALUE_TYPE r =  TREE_REAL_CST (val);
    if (exact_real_inverse (TYPE_MODE (TREE_TYPE (val)), &r))
      return true;
  }
  return false;
}


// Return true if VAR doesn't depend on anything in loop NUM.
bool all_outside (tree var, const struct loop* loop)
{
  //fprintf (stderr, "all_outside0\n");
  //debug_tree(var);
  if (TREE_CODE (var) != SSA_NAME) return false;
  gimplePtr  def_stmt = SSA_NAME_DEF_STMT (var);
#if 0
  fprintf (stderr, "all_outside %d %d %s %s %d\n",
           loop->num,
           gimple_bb(def_stmt) ? gimple_bb(def_stmt)->loop_father->num : -1,
           gimple_code_name[gimple_code(def_stmt)],
           "", //get_tree_code_name(gimple_expr_code(def_stmt)),
           gimple_num_ops (def_stmt));
  debug_gimple_stmt (def_stmt);
#endif
  if (!gimple_bb (def_stmt)) {
    if (gimple_code (def_stmt) == GIMPLE_NOP)
      return true; // function argument?
    return false;
  }
  if (loop != gimple_bb (def_stmt)->loop_father && 
      !flow_loop_nested_p (loop, gimple_bb (def_stmt)->loop_father))
  {
    // Defined outside.
    //fprintf (stderr, "defined outside\n");
    return true;
  }

  if (gimple_code (def_stmt) == GIMPLE_PHI)
    return false;

  if (gimple_code (def_stmt) == GIMPLE_CALL)
    return false;

  if (gimple_code (def_stmt) == GIMPLE_ASSIGN)
  {
    size_t nop = gimple_num_ops (def_stmt);
    for (size_t i = 1; i < nop; i++) {
      if (!all_outside (gimple_op (def_stmt, i), loop)) return false;
    }
  }

  return true;
}


bool is_const (tree val)
{
  if (TREE_CODE (val) == REAL_CST)
    return true;
  if (TREE_CONSTANT (val))
    return true;
  if (TREE_CODE (val) != SSA_NAME)
    return false;

  gimplePtr  def_stmt = SSA_NAME_DEF_STMT (val);
  if (gimple_code (def_stmt) == GIMPLE_ASSIGN) {
    if (gimple_expr_code (def_stmt) == REAL_CST)
      return true;

    if ((gimple_expr_code (def_stmt) == PLUS_EXPR ||
         gimple_expr_code (def_stmt) == MINUS_EXPR ||
         gimple_expr_code (def_stmt) == MULT_EXPR ||
         gimple_expr_code (def_stmt) == RDIV_EXPR) &&
        gimple_num_ops (def_stmt) == 3)
    {
      if (is_const (gimple_op (def_stmt, 1)) &&
          is_const (gimple_op (def_stmt, 2)))
        return true;
    }

    if (gimple_expr_code (def_stmt) == VAR_DECL && 
        gimple_num_ops (def_stmt) == 2)
    {
      return is_const (gimple_op (def_stmt, 1));
    }
  }

  if (gimple_code (def_stmt) == GIMPLE_CALL) {
    if (gimple_expr_code (def_stmt) == CALL_EXPR &&
        gimple_num_ops (def_stmt) >= 2)
    {
      tree fun = gimple_op (def_stmt, 1);
      if (fun && TREE_OPERAND_LENGTH (fun) >= 1) {
        tree decl = TREE_OPERAND (fun, 0);
        if (decl && DECL_P (decl) && DECL_DECLARED_CONSTEXPR_P (decl)) {
          for (unsigned int i = 2; i < gimple_num_ops (def_stmt); i++) {
            tree arg = gimple_op (def_stmt, i);
            if (arg && !is_const (arg))
              return false;
          }
          return true;
        }
      }
    }
  }

  return false;
}


bool is_unused (gimplePtr stmt)
{
  if (gimple_code (stmt) == GIMPLE_ASSIGN &&
      TREE_CODE (gimple_op (stmt, 0)) == SSA_NAME)
  {
    tree var = SSA_NAME_VAR (gimple_op (stmt, 0));
    if (var && DECL_P (var)) {
      if (lookup_attribute ("unused", DECL_ATTRIBUTES (var)))
        return true;
    }
  }
  return false;
}


bool ignore_decl_p (tree decl)
{
  tree ctx = DECL_CONTEXT (decl);
  while (ctx && !SCOPE_FILE_SCOPE_P (ctx)) {
    if (DECL_P (ctx)) {
      tree nsname_node = DECL_NAME (ctx);
      if (nsname_node) {
        const char* nsname = IDENTIFIER_POINTER (nsname_node);
        if (strcmp (nsname, "HepGeom") == 0) return true;
        if (strcmp (nsname, "Eigen") == 0) return true;
        if (strcmp (nsname, "fastjet") == 0) return true;
        if (strcmp (nsname, "CLHEP") == 0) return true;
        if (strcmp (nsname, "TVector2") == 0) return true;
        if (strcmp (nsname, "TMath") == 0) return true;
        if (strcmp (nsname, "boost") == 0) return true;
      }
    }
    if (DECL_P (ctx))
      ctx = DECL_CONTEXT (ctx);
    else if (TYPE_P (ctx))
      ctx = TYPE_CONTEXT (ctx);
    else
      break;
  }

  return false;
}



unsigned int divcheck_pass::divcheck_execute (function* fun)
{
  if (ignore_decl_p (fun->decl))
    return 0;

  // Ignore TVector2 operator(const TVector2&, ...)
  {
    tree funname_node = DECL_NAME (fun->decl);
    const char* funname = IDENTIFIER_POINTER (funname_node);
    if (strcmp (funname, "operator/") == 0) {
      tree arguments = DECL_ARGUMENTS (fun->decl);
      if (arguments) {
        tree atyp = DECL_ARG_TYPE (arguments);
        if (atyp && TREE_CODE (atyp) == REFERENCE_TYPE) {
          tree rtyp = TREE_TYPE (atyp);
          if (rtyp && TREE_CODE (rtyp) == RECORD_TYPE) {
            tree tname = TYPE_NAME (rtyp);
            if (tname && DECL_P (tname)) {
              tree tname_node = DECL_NAME (tname);
              if (tname_node) {
                const char* tname_str = IDENTIFIER_POINTER (tname_node);
                if (strcmp (tname_str, "TVector2") == 0) {
                  return 0;
                }
              }
            }
          }
        }
      }
    }
  }
  
#if GCC_VERSION < 4009
  loop_optimizer_init (LOOPS_MAY_HAVE_MULTIPLE_LATCHES);
#endif

  basic_block bb;
  FOR_EACH_BB_FN(bb, fun) {
    for (gimple_stmt_iterator si = gsi_start_bb (bb); 
         !gsi_end_p (si);
         gsi_next (&si))
    {
      gimplePtr stmt = gsi_stmt (si);
      if (gimple_code (stmt) == GIMPLE_ASSIGN &&
          gimple_expr_code (stmt) == RDIV_EXPR &&
          gimple_num_ops (stmt) >= 3)
      {
        // Skip if the value is marked with unused.
        if (is_unused (stmt))
          continue;
        
        tree val = gimple_op (stmt, 2);
        // Warn about dividing by a float constant.
        // But don't warn if the divisor has an exact inverse,
        // as gcc will optimize that by default.
        if (is_const (val) &&
            !is_const (gimple_op (stmt, 1)) &
            !has_exact_inverse (val))
        {
          bool ignored = false;
          gimplePtr use_stmt;
          imm_use_iterator iter;
          FOR_EACH_IMM_USE_STMT (use_stmt, iter, gimple_op (stmt, 0))
          {
            if (gimple_code (use_stmt) == GIMPLE_ASSIGN &&
                DECL_P (gimple_op (use_stmt, 0)))
            {
              tree decl = gimple_op (use_stmt, 0);
              if (decl && ignore_decl_p (decl)) {
                ignored = true;
                BREAK_FROM_IMM_USE_STMT (iter);
              }
            }
          }
          if (!ignored) {
            warning_at (gimple_location (stmt), 0,
                        "Floating-point constant used in division; consider multiplication by inverse.");
            CheckerGccPlugins::inform_url (gimple_location (stmt), url);
          }
          continue;
        }

        if (TREE_CODE(val) != SSA_NAME)
          continue;
        
        // In a loop?
        if (bb->loop_father && bb->loop_father->num > 0) {
          // Warn if the divisor was defined outside this loop.
          if (all_outside (val, bb->loop_father)) {

            basic_block header = bb->loop_father->header;
            gimplePtr gheader = nullptr;
            if (header) {
              for (gimple_stmt_iterator sih = gsi_start_bb (header);
                   !gsi_end_p (sih);
                   gsi_next (&sih))
              {
                gimplePtr s = gsi_stmt (sih);
                if (gimple_code (s) != GIMPLE_DEBUG) {
                  gheader = s;
                  break;
                }
              }
            }

            if (!gheader) continue;

            tree block = gimple_block (stmt);
            tree header_block = gimple_block (gheader);
            while (block && block != header_block)
              block = BLOCK_SUPERCONTEXT (block);
            if (block != header_block)
              continue;

            
            //debug_tree (gimple_block (stmt));
            warning_at (gimple_location (stmt), 0,
                        "Division by loop-invariant value; consider calculating inverse outside of the loop and multiplying within the loop.");

            while (gheader && gimple_code (gheader) == GIMPLE_DEBUG)
#if GCC_VERSION >= 4009
              gheader = gheader->next;
#else
              gheader = gheader->gsbase.next;
#endif
            if (gheader)
              inform (gimple_location (gheader),
                      "Loop starts here.");
            CheckerGccPlugins::inform_url (gimple_location (stmt), url);
          }
        }
        else {
          // Not in a loop.
          // Search for another division by this value that dominates this.
          gimplePtr use_stmt;
          imm_use_iterator iter;
          FOR_EACH_IMM_USE_STMT (use_stmt, iter, val)
          {
            if (use_stmt == stmt) continue;

            if (is_unused (use_stmt)) continue;

            if (gimple_code (use_stmt) == GIMPLE_ASSIGN &&
                gimple_expr_code (use_stmt) == RDIV_EXPR &&
                gimple_num_ops (use_stmt) >= 3 &&
                gimple_op (use_stmt, 2) == val)
            {
              // Warn if use_stmt dominates stmt.
              bool dominated = false;

              if (bb == gimple_bb (use_stmt))
              {
                for (gimple_stmt_iterator si2 = gsi_start_bb (bb); 
                     !gsi_end_p (si2);
                     gsi_next (&si2))
                {
                  gimplePtr stmt2 = gsi_stmt (si2);
                  if (stmt == stmt2) break;
                  if (use_stmt == stmt2) {
                    dominated = true;
                    break;
                  }
                }
              }
              else {
                dominated = dominated_by_p (CDI_DOMINATORS, 
                                            gimple_bb (stmt),
                                            gimple_bb (use_stmt));
              }

              if (dominated) {
                warning_at (gimple_location (stmt), 0,
                            "Multiple divisions by the same value; consider calculating the inverse once and multiplying.");
                inform (gimple_location (use_stmt),
                        "Previous division is here.");
                CheckerGccPlugins::inform_url (gimple_location (stmt), url);
                BREAK_FROM_IMM_USE_STMT (iter);
                break;
              }
            }
          }
        }
      }
    }
  }

#if GCC_VERSION < 4009
  loop_optimizer_finalize();
#endif

  return 0;
}


} // anonymous namespace


void init_divcheck_checker (plugin_name_args* plugin_info)
{
  struct register_pass_info pass_info = {
#if GCC_VERSION >= 4009
    new divcheck_pass(g),
#else
    &divcheck_pass_data,
#endif
    //"optimized",
    "ssa",
    0,
    PASS_POS_INSERT_AFTER
  };
  
  register_callback (plugin_info->base_name,
                     PLUGIN_PASS_MANAGER_SETUP,
                     NULL,
                     &pass_info);
}
