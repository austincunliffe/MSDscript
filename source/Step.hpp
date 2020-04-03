//
// Created by Austin Cunliffe on 3/24/20.
//

#pragma once

#include "pointer.hpp"
#include "Expr.hpp"

class Cont;

/**
 * The <code>Step</code> introduces the static method <code>interp_by_steps</code>.
 */
class Step {
public:
    typedef enum {
        interp_mode,
        continue_mode
    } mode_t;

    static mode_t mode;

    static PTR(Expr) expr;
    static PTR(Env) env;

    static PTR(Val) val;

    static PTR(Cont) cont;

/**
 * Evaluates the Expr and returns a Val.
 * Differs from the standard interp as this method allows the solving of deeply recursive functions without causing a
 * stack overflow.
 * /exception If the evaluation reaches a free variable, an error will be thrown.
 * @param e Expr to be evaluated.
 * @return Val representing the Expr solution or a semantically equivalent value.
 */
    static PTR(Val) interp_by_steps(PTR(Expr) e);
};

