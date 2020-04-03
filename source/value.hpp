//
// Created by Austin Cunliffe on 1/29/20.
//

#pragma once

#include <string>
#include "pointer.hpp"

/* A forward declaration, so `Val` can refer to `Expr` and 'Env', while
   `Expr` still needs to refer to `Val`. */
class Expr;
class Env;
class Cont;

class Val ENABLE_THIS(Val){
public:
    virtual bool equals(PTR(Val) val) = 0;
    virtual PTR(Val) add_to(PTR(Val) other_val) = 0;
    virtual PTR(Val) mult_with(PTR(Val) other_val) = 0;
    virtual PTR(Expr) to_expr() = 0;
    virtual std::string to_string() = 0;
    virtual bool is_true() = 0;
    virtual PTR(Val) call(PTR(Val) actual_arg) = 0;
    virtual void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) = 0;
};


/**
 * Stores an int. Can be returned from the <code>interp()</code> method of an Expr.
 * The actual int can be accessed via the <code>rep</code> member variable.
 */
class NumVal : public Val {
public:
    int rep;

    /**
     * Constructs a NumVal with the provided int.
     * @param rep int to be stored in NumVal.
     */
    NumVal(int rep);
    bool equals(PTR(Val) val);

    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    PTR(Expr) to_expr();
    std::string to_string();
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};

/**
 * Stores a bool. Can be returned from the <code>interp()</code> method of an Expr.
 * The actual bool can be accessed via the <code>rep</code> member variable.
 */
class BoolVal : public Val {
public:
    bool rep;

    /**
     * Constructs a BoolVal with the provided bool.
     * @param rep bool to be stored in BoolVal.
     */
    BoolVal(bool rep);

    bool equals(PTR(Val) val);

    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    PTR(Expr) to_expr();
    std::string to_string();
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};

/**
 * Stores the components of a function. Can be returned from the <code>interp()</code> method of an Expr.
 */
class FunVal : public Val {
public:
    std::string formal_arg;
    PTR(Expr) body;
    PTR(Env) env;

    /**
     * Constructs a FunVal from a string formal_arg, Expr body, and Env env.
     * @param formal_arg string representing the formal_arg.
     * @param body Expr representing the actual function.
     * @param env Env to pass along into the FunVal.
     */
    FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env);

    bool equals(PTR(Val) val);

    PTR(Val) add_to(PTR(Val) other_val);
    PTR(Val) mult_with(PTR(Val) other_val);
    PTR(Expr) to_expr();
    std::string to_string();
    bool is_true();
    PTR(Val) call(PTR(Val) actual_arg);
    void call_step(PTR(Val) actual_arg_val, PTR(Cont) rest);
};


