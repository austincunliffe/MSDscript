//
// Created by Austin Cunliffe on 3/24/20.
//

#pragma once

#include "pointer.hpp"
#include "Expr.hpp"

class Cont;

class Cont ENABLE_THIS(Cont) {
public:
    virtual void step_continue() = 0;

    static PTR(Cont) done;
};

class DoneCont : public Cont {
public:
    DoneCont();
    void step_continue();
};

class RightThenAddCont : public Cont {
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;

    RightThenAddCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class AddCont : public Cont {
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;

    AddCont(PTR(Val) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class RightThenMultCont : public Cont {
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;

    RightThenMultCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class MultCont : public Cont {
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;

    MultCont(PTR(Val) lhs_val, PTR(Cont) rest);
    void step_continue();
};

class IfBranchCont : public Cont {
public:
    PTR(Expr) then_part;
    PTR(Expr) else_part;
    PTR(Env) env;
    PTR(Cont) rest;

    IfBranchCont(PTR(Expr) then_part, PTR(Expr) else_part, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class LetBodyCont : public Cont {
public:
    std::string var;
    PTR(Expr) body;
    PTR(Env) env;
    PTR(Cont) rest;

    LetBodyCont(std::string var, PTR(Expr) body, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class ArgThenCallCont : public Cont {
public:
    PTR(Expr) actual_arg;
    PTR(Env) env;
    PTR(Cont) rest;

    ArgThenCallCont(PTR(Expr) actual_arg, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class CallCont : public Cont {
public:
    PTR(Val) to_be_called_val;
    PTR(Cont) rest;

    CallCont(PTR(Val) to_be_called_val, PTR(Cont) rest);
    void step_continue();
};

class RightThenCompCont : public Cont {
public:
    PTR(Expr) rhs;
    PTR(Env) env;
    PTR(Cont) rest;

    RightThenCompCont(PTR(Expr) rhs, PTR(Env) env, PTR(Cont) rest);
    void step_continue();
};

class CompCont : public Cont {
public:
    PTR(Val) lhs_val;
    PTR(Cont) rest;

    CompCont(PTR(Val) lhs_val, PTR(Cont) rest);
    void step_continue();
};

