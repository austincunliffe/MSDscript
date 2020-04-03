//
// Created by Austin Cunliffe on 1/13/20.
//

#pragma once

#include <string>
#include "value.hpp"
#include "pointer.hpp"
#include "env.hpp"


class Expr ENABLE_THIS(Expr){
public:
    virtual bool equals(PTR(Expr) e) = 0;
    virtual PTR(Val) interp(PTR(Env) env) = 0;
    virtual void step_interp() = 0;
    virtual PTR(Expr) subst(std::string, PTR(Val)) = 0;
    virtual bool contains_var() = 0;
    virtual PTR(Expr) optimize() = 0;
    virtual std::string expr_print() = 0;
};

/**
 * The <code>NumExpr</code> class represents an integer as an Expr.
 */
class NumExpr: public Expr {
public:
    int rep;
    PTR(Val) val;

    /**
     * Construct a NumExpr from an int.
     * @param rep int to be represented by this NumExpr.
     */
    NumExpr(int rep);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);
    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>VarExpr</code> class represents a variable as an Expr.
 */
class VarExpr: public Expr {
public:
    std::string name;

    /**
     * Construct a VarExpr from an string.
     * @param val string variable.
     */
    VarExpr(std::string val);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>AddExpr</code> class is used to represent the addition of two separate Expr's.
 */
class AddExpr: public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    /**
     * Construct an AddExpr from two Expr's.
     * @param lhs left hand side Expr.
     * @param rhs right hand side Expr.
     */
    AddExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);
    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>MultExpr</code> class is used to represent the multiplication of two separate Expr's.
 */
class MultExpr: public Expr {
public:
    PTR(Expr) lhs;
    PTR(Expr) rhs;

    /**
     * Construct an MultExpr from two Expr's.
     * @param lhs left hand side Expr.
     * @param rhs right hand side Expr.
     */
    MultExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>LetExpr</code> class is used to assign an Expr to a variable within another Expr.
 * For example, _let x = 5 _in x + 1
 */
class LetExpr: public Expr {
public:
    std::string name;
    PTR(Expr) var_val;
    PTR(Expr) in_expr;

    /**
     * Construct an LetExpr using a string to represent the variable, an Expr representing the value of that variable,
     * and another Expr representing the body of the LetExpr.
     * @param name string representing the variable.
     * @param var_val Expr representing the value of variable.
     * @param in_expr Expr representing the body of the LetExpr.
     */
    LetExpr(std::string name, PTR(Expr) var_val, PTR(Expr) in_expr);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>BoolExpr</code> class is used to represent a boolean as an Expr.
 */
class BoolExpr : public Expr {
public:
    bool rep;

    /**
     * Construct a BoolExpr from a bool.
     * @param rep bool to be represented by this BoolExpr.
     */
    BoolExpr(bool rep);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>IfExpr</code> class is used to represent an if else then statement.
 */
class IfExpr : public Expr {
public:
    PTR(Expr) test_part;
    PTR(Expr) then_part;
    PTR(Expr) else_part;

    /**
     * Construct an IfExpr consisting of three Expr's: test_part, then_part, and else_part.
     * @param test_part Expr that when evaluated determines if then_part or else_part is evaluated.
     * @param then_part Expr that can be evaluated if test_part is true.
     * @param else_part Expr that can be evaluated if else_part is true.
     */
    IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>EqualExpr</code> class is used to represent a comparison of two Expr's to determine equality.
 */
class EqualExpr : public Expr {
public:
    PTR(Expr) rhs;
    PTR(Expr) lhs;

    /**
     * Construct an EqualExpr with two Expr's.
     * @param lhs left hand side Expr.
     * @param rhs right hand side Expr.
     */
    EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>FunExpr</code> class is used to represent user defined functions.
 * This class can be used in conjunction with the <code>CallExpr</code> class to implement function calls.
 */
class FunExpr : public Expr {
public:
    std::string formal_arg;
    PTR(Expr) actual_arg;

    /**
     * Construct a FunExpr with the formal_arg represented by a string and the actual_arg represented by an Expr.
     * @param formal_arg string
     * @param actual_arg Expr representing the body of the FunExpr.
     */
    FunExpr(std::string formal_arg, PTR(Expr) actual_arg);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};

/**
 * The <code>CallExpr</code> class is used to represent the calling of one Expr and passing in another Expr.
 * This class is useful when paired with the <code>FunExpr</code> class.
 */
class CallExpr : public Expr {
public:
    PTR(Expr) to_be_called;
    PTR(Expr) actual_argument;

    /**
     * Construct a CallExpr with two Expr's.
     * @param to_be_called Expr
     * @param actual_argument Expr
     */
    CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_argument);

    /**
     * Return boolean specifying if this Expr is equal to the Expr passed in as parameter e.
     * @param e Expr to compare.
     * @return true if expressions are equal, false otherwise.
     */
    bool equals(PTR(Expr) e);

    /**
     * Evaluates the Expr and returns a Val. Takes an Env as parameter e.
     * /exception If the evaluation reaches a free variable, an error will be thrown.
     * @param env
     * @return Val representing the Expr solution or a semantically equivalent value.
     */
    PTR(Val) interp(PTR(Env) env);

    void step_interp();
    PTR(Expr) subst(std::string var, PTR(Val) val);

    /**
     * Determines if this Expr contains a variable and returns a boolean.
     * @return true if Expr contains variable, false otherwise.
     */
    bool contains_var();

    /**
     * Evaluates the Expr and returns a semantically equivalent Expr that is no larger than the input Expr.
     * Unlike the <code>interp</code> method, <code>optimize</code> will not throw an error if the evaluations reaches
     * a free variable.
     * @return Expr representing the most optimized solution or a semantically equivalent Expr.
     */
    PTR(Expr) optimize();

    /**
     * Returns the Expr in a human readable string.
     * @return String of the Expr.
     */
    std::string expr_print();
};