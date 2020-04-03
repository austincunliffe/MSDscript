//
// Created by Austin Cunliffe on 1/21/20.
//
#include "catch.hpp"
#include "Expr.hpp"
#include "parser.hpp"
#include "Cont.hpp"
#include "Step.hpp"

PTR(Env) Env::empty = NEW(EmptyEnv)();

NumExpr::NumExpr(int rep){
    this->rep = rep;
    this->val = NEW(NumVal)(rep);
}

bool NumExpr::equals(PTR(Expr) e) {
    PTR(NumExpr) n = CAST(NumExpr)(e);
    if (n == NULL)
        return false;
    else
        return rep == n->rep;
}

PTR(Val) NumExpr::interp(PTR(Env) env) {
    return val;
}

void NumExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(NumVal)(rep);
    Step::cont = Step::cont;
}


PTR(Expr) NumExpr::subst(std::string var, PTR(Val) val) {
    return NEW(NumExpr)(this->rep);
}

bool NumExpr::contains_var(){
    return false;
}

PTR(Expr) NumExpr::optimize(){
    return NEW(NumExpr)(rep);
}

std::string NumExpr::expr_print() {
    return std::to_string(rep);
}

AddExpr::AddExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool AddExpr::equals(PTR(Expr) e) {
    PTR(AddExpr) a = CAST(AddExpr)(e);
    if (a == NULL)
        return false;
    else
        return (lhs->equals(a->lhs)
                && rhs->equals(a->rhs));
}

PTR(Val) AddExpr::interp(PTR(Env) env) {
    return lhs->interp(env)->add_to(rhs->interp(env));
}

void AddExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenAddCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) AddExpr::subst(std::string var, PTR(Val) val) {
    return NEW(AddExpr)(lhs->subst(var, val), rhs->subst(var, val));
}

bool AddExpr::contains_var(){
    return lhs->contains_var() || rhs->contains_var();
}

PTR(Expr) AddExpr::optimize() {
    if (this->contains_var()) {
        PTR(Expr) e = (NEW(AddExpr)(lhs->optimize(), rhs->optimize()));
        if (!e->contains_var()) {
            return e->interp(Env::empty)->to_expr();
        } else {
            return e;
        }
    }
    return THIS->interp(Env::empty)->to_expr();
}

std::string AddExpr::expr_print() {
    std::string left = lhs->expr_print();
    std::string right = rhs->expr_print();

    std::string output = "(" + left + " + " + right + ")";
    return output;
}

MultExpr::MultExpr(PTR(Expr)lhs, PTR(Expr)rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool MultExpr::equals(PTR(Expr)e) {
    PTR(MultExpr) m = CAST(MultExpr)(e);
    if (m == NULL)
        return false;
    else
        return (lhs->equals(m->lhs)
                && rhs->equals(m->rhs));
}

PTR(Val) MultExpr::interp(PTR(Env) env) {
    return lhs->interp(env)->mult_with(rhs->interp(env));
}

void MultExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenMultCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) MultExpr::subst(std::string var, PTR(Val) val) {
    return NEW(MultExpr)(lhs->subst(var, val), rhs->subst(var, val));
}

bool MultExpr::contains_var(){
    return lhs->contains_var() || rhs->contains_var();
}

PTR(Expr) MultExpr::optimize() {
    if (this->contains_var()) {
        PTR(MultExpr) e = (NEW(MultExpr)(lhs->optimize(), rhs->optimize()));
        if (!e->contains_var()) {
            return e->optimize();
        } else {
            return e;
        }
    }
    return THIS->interp(Env::empty)->to_expr();
}

std::string MultExpr::expr_print() {
    std::string left = lhs->expr_print();
    std::string right = rhs->expr_print();

    std::string output = "(" + left + " * " + right + ")";
    return output;
}

VarExpr::VarExpr(std::string name) {
    this->name = name;
}

bool VarExpr::equals(PTR(Expr)e) {
    PTR(VarExpr) v = CAST(VarExpr)(e);
    if (v == NULL)
        return false;
    else
        return name == v->name;
}

PTR(Val) VarExpr::interp(PTR(Env) env) {
    return env->lookup(name);
}

void VarExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = Step::env->lookup(name);
    Step::cont = Step::cont;
}

PTR(Expr) VarExpr::subst(std::string var, PTR(Val) val) {
    if (var == name)
        return val->to_expr();
    else
        return NEW(VarExpr)(name);
}

bool VarExpr::contains_var(){
    return true;
}

PTR(Expr) VarExpr::optimize() {
    return NEW(VarExpr)(name);
}

std::string VarExpr::expr_print() {
    return name;
}

LetExpr::LetExpr(std::string name, PTR(Expr) var_val, PTR(Expr) in_expr) {
    this->name = name;
    this->var_val = var_val;
    this->in_expr = in_expr;
}

bool LetExpr::equals(PTR(Expr)e) {
    PTR(LetExpr) l = CAST(LetExpr)(e);
    if (l == NULL)
        return false;
    else
        return (this->name == l->name) && (this->var_val->equals(l->var_val)) && (this->in_expr->equals(l->in_expr));
}

PTR(Val) LetExpr::interp(PTR(Env) env) {
    PTR(Val) rhs_val = var_val->interp(env);
    PTR(Env) new_env = NEW(ExtendedEnv)(env, name, rhs_val);
    return in_expr->interp(new_env);
}

void LetExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = var_val;
    Step::env = Step::env;
    Step::cont = NEW(LetBodyCont)(name, in_expr, Step::env, Step::cont);
}

PTR(Expr) LetExpr::subst(std::string var, PTR(Val) val) {
    if (var == name) {
        return NEW(LetExpr)(name, var_val->subst(var, val), in_expr);
    }
    else {
        return NEW(LetExpr)(name, var_val->subst(var, val), in_expr->subst(var, val));
    }
}

bool LetExpr::contains_var(){
    return in_expr->contains_var();
}

PTR(Expr) LetExpr::optimize() {
    var_val = var_val->optimize();
    if (!(var_val->contains_var()))
        return in_expr->subst(name, var_val->interp(Env::empty))->optimize();
    return NEW(LetExpr)(name, var_val, in_expr->optimize());
}

std::string LetExpr::expr_print() {
    std::string output = "_let " + name;
    std::string variable_val = var_val->expr_print();
    std::string body = in_expr->expr_print();

    output = output + " = " + variable_val + " _in " + body;
    return output;
}

BoolExpr::BoolExpr(bool rep) {
    this->rep = rep;
}

bool BoolExpr::equals(PTR(Expr) e) {
    PTR(BoolExpr) b = CAST(BoolExpr)(e);
    if (b == NULL)
        return false;
    else
        return rep == b->rep;
}

PTR(Val) BoolExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(rep);
}

void BoolExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(BoolVal)(rep);
    Step::cont = Step::cont;
}

PTR(Expr) BoolExpr::subst(std::string var, PTR(Val) new_val) {
    return NEW(BoolExpr)(rep);
}

bool BoolExpr::contains_var() {
    return false;
}

PTR(Expr) BoolExpr::optimize() {
    return NEW(BoolExpr)(rep);
}

std::string BoolExpr::expr_print() {
    if (this->rep) return "_true";
    return "_false";
}

IfExpr::IfExpr(PTR(Expr) test_part, PTR(Expr) then_part, PTR(Expr) else_part) {
    this->test_part = test_part;
    this->then_part = then_part;
    this->else_part = else_part;
}

bool IfExpr::equals(PTR(Expr) e) {
    PTR(IfExpr) i = CAST(IfExpr)(e);
    if (i == NULL)
        return false;
    else
        return (this->test_part->equals(i->test_part)) && (this->then_part->equals(i->then_part)) &&
        (this->else_part->equals(i->else_part));
}

PTR(Val) IfExpr::interp(PTR(Env) env) {
    if (test_part->interp(env)->is_true())
        return then_part->interp(env);
    else
        return else_part->interp(env);
}

void IfExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = test_part;
    Step::env = Step::env;
    Step::cont = NEW(IfBranchCont)(then_part, else_part, Step::env, Step::cont);
}

PTR(Expr) IfExpr::subst(std::string var, PTR(Val) val) {
    return NEW(IfExpr)(test_part->subst(var, val), then_part->subst(var, val),
            else_part->subst(var, val));
}

bool IfExpr::contains_var() {
    return test_part->contains_var() || then_part->contains_var() || else_part->contains_var();
}

PTR(Expr) IfExpr::optimize() {
    if (!test_part->contains_var()) {
        if (test_part->interp(Env::empty)->is_true())
            return then_part->optimize();
        else
            return else_part->optimize();
    }
    return NEW(IfExpr)(test_part->optimize(), then_part->optimize(), else_part->optimize());
}

std::string IfExpr::expr_print() {
    return "_if " + test_part->expr_print() + " _then " + then_part->expr_print() + " _else " + else_part->expr_print();
}

EqualExpr::EqualExpr(PTR(Expr) lhs, PTR(Expr) rhs) {
    this->lhs = lhs;
    this->rhs = rhs;
}

bool EqualExpr::equals(PTR(Expr) e) {
    PTR(EqualExpr) eq = CAST(EqualExpr)(e);
    if (eq == NULL)
        return false;
    else
        return (this->lhs->equals(eq->lhs)) && (this->rhs->equals(eq->rhs));
}

PTR(Val) EqualExpr::interp(PTR(Env) env) {
    return NEW(BoolVal)(lhs->interp(env)->equals(rhs->interp(env)));
}

void EqualExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = lhs;
    Step::env = Step::env;
    Step::cont = NEW(RightThenCompCont)(rhs, Step::env, Step::cont);
}

PTR(Expr) EqualExpr::subst(std::string var, PTR(Val) val) {
    return NEW(EqualExpr)(lhs->subst(var, val), rhs->subst(var, val));
}

bool EqualExpr::contains_var() {
    return lhs->contains_var() || rhs->contains_var();
}

PTR(Expr) EqualExpr::optimize() {
    if (!(this->contains_var())) {
        return NEW(BoolExpr)(lhs->optimize()->equals(rhs->optimize()));
    }
    return NEW(EqualExpr)(lhs->optimize(), rhs->optimize());
}

std::string EqualExpr::expr_print() {
    return "(" + lhs->expr_print() + " == " + rhs->expr_print() + ")";
}

FunExpr::FunExpr(std::string formal_arg, PTR(Expr) actual_arg) {
    this->formal_arg = formal_arg;
    this->actual_arg = actual_arg;
}

bool FunExpr::equals(PTR(Expr) e) {
    PTR(FunExpr) f = CAST(FunExpr)(e);
    if (f == NULL)
        return false;
    else
        return (this->formal_arg == f->formal_arg) && (this->actual_arg->equals(f->actual_arg));
}

PTR(Val) FunExpr::interp(PTR(Env) env) {
    return NEW(FunVal)(formal_arg, actual_arg, env);
}

void FunExpr::step_interp() {
    Step::mode = Step::continue_mode;
    Step::val = NEW(FunVal)(formal_arg, actual_arg, Step::env);
    Step::cont = Step::cont;
}

PTR(Expr) FunExpr::subst(std::string var, PTR(Val) val) {
    if (formal_arg != var) return NEW(FunExpr)(formal_arg, actual_arg->subst(var, val));
    return NEW(FunExpr)(formal_arg, actual_arg);
}

bool FunExpr::contains_var() {
    return actual_arg->contains_var();
}

PTR(Expr) FunExpr::optimize() {
    return NEW(FunExpr)(formal_arg, actual_arg->optimize());
}

std::string FunExpr::expr_print() {
    return "(_fun (" + formal_arg + ") " + actual_arg->expr_print() + ")";
}

CallExpr::CallExpr(PTR(Expr) to_be_called, PTR(Expr) actual_argument) {
    this->to_be_called = to_be_called;
    this->actual_argument = actual_argument;
}

bool CallExpr::equals(PTR(Expr) e) {
    PTR(CallExpr) c = CAST(CallExpr)(e);
    if (c == NULL)
        return false;
    else
        return (this->to_be_called->equals(c->to_be_called) && (this->actual_argument->equals(c->actual_argument)));
}

PTR(Val) CallExpr::interp(PTR(Env) env) {
    return to_be_called->interp(env)->call(actual_argument->interp(env));
}

void CallExpr::step_interp() {
    Step::mode = Step::interp_mode;
    Step::expr = to_be_called;
    Step::cont = NEW(ArgThenCallCont)(actual_argument, Step::env, Step::cont);
}

PTR(Expr) CallExpr::subst(std::string var, PTR(Val) val) {
    return NEW(CallExpr)(to_be_called->subst(var, val), actual_argument->subst(var, val));
}

bool CallExpr::contains_var() {
    return to_be_called->contains_var() || actual_argument->contains_var();
}

PTR(Expr) CallExpr::optimize() {
    return NEW(CallExpr)(to_be_called->optimize(), actual_argument->optimize());
}

std::string CallExpr::expr_print() {
    return "(" + to_be_called->expr_print() + "(" + actual_argument->expr_print() + "))";
}

TEST_CASE("equals") {
    CHECK((NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->equals(
            NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y")))));
    }

TEST_CASE("interp") {
    CHECK(parse_str(" 3 ")->interp(Env::empty)->equals(NEW(NumVal)(3)));
    CHECK(parse_str(" 3+4 ")->interp(Env::empty)->equals(NEW(NumVal)(7)));
    CHECK(parse_str(" 3*4 ")->interp(Env::empty)->equals(NEW(NumVal)(12)));
    CHECK_THROWS(parse_str(" 3*x ")->interp(Env::empty)->equals(NEW(NumVal)(0)));
    CHECK_THROWS(parse_str(" 3+x ")->interp(Env::empty)->equals(NEW(NumVal)(3)));
    CHECK_THROWS(parse_str(" 3+x*12 ")->interp(Env::empty)->equals(NEW(NumVal)(3)));
    CHECK(parse_str(" _let x = 5 _in 3 * x + 3")->interp(Env::empty)->equals(NEW(NumVal)(18)));
    CHECK(parse_str(" _true")->interp(Env::empty)->equals(NEW(BoolVal)(true)));
    CHECK(parse_str(" _false")->interp(Env::empty)->equals(NEW(BoolVal)(false)));
    CHECK(parse_str("_if 10 == 5 _then 3 _else 4")->interp(Env::empty)->equals(NEW(NumVal)(4)));
    CHECK(parse_str("_if 10 == 10 _then 3 + 10 _else 4")->interp(Env::empty)->equals(NEW(NumVal)(13)));
    CHECK(parse_str("_if 10 == 11 _then 3 + 10 _else 4 * 30")->interp(Env::empty)->equals(NEW(NumVal)(120)));
    CHECK(parse_str("_if _true _then 1 _else 0")->interp(Env::empty)->equals(NEW(NumVal)(1)));
    CHECK(parse_str(" 2 == 1+1")->interp(Env::empty)->equals(NEW(BoolVal)(true)));
    CHECK(parse_str(" 1 == 1")->interp(Env::empty)->equals(NEW(BoolVal)(true)));
    CHECK(parse_str("_if 7 == _true _then 8 _else 1")->interp(Env::empty)->equals(NEW(NumVal)(1)));
    CHECK_THROWS_WITH(parse_str(" x == x")->interp(Env::empty), "free variable: x");
    CHECK(parse_str("_fun (x) x + 3")->interp(Env::empty)->equals(NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), Env::empty)));
    CHECK(parse_str("(_fun (x) x + 10)(1)")->interp(Env::empty)->equals(NEW(NumVal)(11)));
    CHECK(parse_str("(10)(1)")->interp(Env::empty)->equals(NEW(NumVal)(10)));
    CHECK_THROWS_WITH(parse_str("(10)(1 + y)")->interp(Env::empty)->equals(NEW(NumVal)(10)), "free variable: y");
    CHECK(parse_str("(10 + 5)(1)")->interp(Env::empty)->equals(NEW(NumVal)(15)));
    CHECK(parse_str("(_true)(1)")->interp(Env::empty)->equals(NEW(BoolVal)(true)));
    CHECK(parse_str("(_false)(1)")->interp(Env::empty)->equals(NEW(BoolVal)(false)));
    CHECK(parse_str("_let f = _fun (x) x*x _in f(2)")->interp(Env::empty)->equals(NEW(NumVal)(4)));
    CHECK(parse_str("_let y = 8 _in _let f = _fun (x) x*y _in f(2)")->interp(Env::empty)->equals(NEW(NumVal)(16)));
    CHECK(parse_str("_let f = (_fun (x) (_fun (y) x*x + y*y)) _in (f(2))(3)")->interp(Env::empty)->equals(NEW(NumVal)(13)));
    CHECK(parse_str("_let f = _fun (x) _fun (y) x*x + y*y _in f(2)(3)")->interp(Env::empty)->equals(NEW(NumVal)(13)));
    CHECK(parse_str("_let fib = _fun (fib) _fun (x)_if x == 0 _then 1 _else _if x == 2 + -1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2)_in fib(fib)(10)")->interp(Env::empty)
                  ->equals((NEW(NumVal)(89))));
    CHECK_THROWS_WITH(parse_str("_let f = _fun (x) 2 _in f(y)")->interp(Env::empty), "free variable: y");
}

TEST_CASE("step_interp") {
    CHECK(Step::interp_by_steps(parse_str(" 3 "))->equals(NEW(NumVal)(3)));
    CHECK(Step::interp_by_steps(parse_str(" 3+4 "))->equals(NEW(NumVal)(7)));
    CHECK(Step::interp_by_steps(parse_str(" 3*4 "))->equals(NEW(NumVal)(12)));
    CHECK_THROWS(Step::interp_by_steps(parse_str(" 3*x "))->equals(NEW(NumVal)(0)));
    CHECK_THROWS(Step::interp_by_steps(parse_str(" 3+x "))->equals(NEW(NumVal)(3)));
    CHECK_THROWS(Step::interp_by_steps(parse_str(" 3+x*12 "))->equals(NEW(NumVal)(3)));
    CHECK(Step::interp_by_steps(parse_str(" _let x = 5 _in 3 * x + 3"))->equals(NEW(NumVal)(18)));
    CHECK(Step::interp_by_steps(parse_str(" _true"))->equals(NEW(BoolVal)(true)));
    CHECK(Step::interp_by_steps(parse_str(" _false"))->equals(NEW(BoolVal)(false)));
    CHECK(Step::interp_by_steps(parse_str("_if 10 == 5 _then 3 _else 4"))->equals(NEW(NumVal)(4)));
    CHECK(Step::interp_by_steps(parse_str("_if 10 == 10 _then 3 + 10 _else 4"))->equals(NEW(NumVal)(13)));
    CHECK(Step::interp_by_steps(parse_str("_if 10 == 11 _then 3 + 10 _else 4 * 30"))->equals(NEW(NumVal)(120)));
    CHECK(Step::interp_by_steps(parse_str("_if _true _then 1 _else 0"))->equals(NEW(NumVal)(1)));
    CHECK(Step::interp_by_steps(parse_str(" 2 == 1+1"))->equals(NEW(BoolVal)(true)));
    CHECK(Step::interp_by_steps(parse_str(" 1 == 1"))->equals(NEW(BoolVal)(true)));
    CHECK(Step::interp_by_steps(parse_str("_if 7 == _true _then 8 _else 1"))->equals(NEW(NumVal)(1)));
    CHECK_THROWS_WITH(Step::interp_by_steps(parse_str(" x == x")), "free variable: x");
    CHECK(Step::interp_by_steps(parse_str("_fun (x) x + 3"))->equals(NEW(FunVal)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)), Env::empty)));
    CHECK(Step::interp_by_steps(parse_str("(_fun (x) x + 10)(1)"))->equals(NEW(NumVal)(11)));
    CHECK_THROWS_WITH(Step::interp_by_steps(parse_str("(10)(1)"))->equals(NEW(NumVal)(10)), "Cannot call call_step on a NumVal.");
    CHECK_THROWS_WITH(Step::interp_by_steps(parse_str("(10)(1 + y)"))->equals(NEW(NumVal)(10)), "free variable: y");
    CHECK_THROWS_WITH(Step::interp_by_steps(parse_str("(10 + 5)(1)"))->equals(NEW(NumVal)(15)), "Cannot call call_step on a NumVal.");
    CHECK_THROWS_WITH(Step::interp_by_steps(parse_str("(_true)(1)"))->equals(NEW(BoolVal)(true)), "Cannot call call_step on a BoolVal.");
    CHECK_THROWS_WITH(Step::interp_by_steps(parse_str("(_false)(1)"))->equals(NEW(BoolVal)(false)), "Cannot call call_step on a BoolVal.");
    CHECK(Step::interp_by_steps(parse_str("_let f = _fun (x) x*x _in f(2)"))->equals(NEW(NumVal)(4)));
    CHECK(Step::interp_by_steps(parse_str("_let y = 8 _in _let f = _fun (x) x*y _in f(2)"))->equals(NEW(NumVal)(16)));
    CHECK(Step::interp_by_steps(parse_str("_let f = (_fun (x) (_fun (y) x*x + y*y)) _in (f(2))(3)"))->equals(NEW(NumVal)(13)));
    CHECK(Step::interp_by_steps(parse_str("_let f = _fun (x) _fun (y) x*x + y*y _in f(2)(3)"))->equals(NEW(NumVal)(13)));
    CHECK(Step::interp_by_steps(parse_str("_let fib = _fun (fib) _fun (x)_if x == 0 _then 1 _else _if x == 2 + -1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2)_in fib(fib)(10)")
                  )->equals((NEW(NumVal)(89))));
    CHECK_THROWS_WITH(Step::interp_by_steps(parse_str("_let f = _fun (x) 2 _in f(y)")), "free variable: y");
}

TEST_CASE("subst") {
    CHECK((NEW(NumExpr)(10))->subst("dog", NEW(NumVal)(3))->equals(NEW(NumExpr)(10)));
    CHECK((NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))->equals(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))));
    CHECK((NEW(AddExpr)(NEW(NumExpr)(2), NEW(VarExpr)("dog")))->subst("dog", NEW(NumVal)(3))->interp(Env::empty)->equals(NEW(NumVal)(5)));
    CHECK((NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->subst("x", NEW(NumVal)(3))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3))));
    CHECK((NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x")))->subst("y", NEW(NumVal)(3))->equals(NEW(MultExpr)(NEW(NumExpr)(2), NEW(VarExpr)("x"))));
    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("y", NEW(NumVal)(3))->equals(
            (NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->subst("y", NEW(NumVal)(3))));
    CHECK((parse_str("_true")->subst("x", NEW(NumVal)(12)))->equals(NEW(BoolExpr)(true)));
    CHECK((parse_str(" 2 == 1+x"))->subst("x", NEW(NumVal)(5))->interp(Env::empty)->equals(NEW(BoolVal)(false)));
    CHECK(parse_str("_if 10 == 5 _then 3 _else x")->subst("x", NEW(NumVal)(100))->interp(Env::empty)->equals(NEW(NumVal)(100)));
    CHECK(parse_str("_if 10 == x _then 3 _else 15")->subst("x", NEW(NumVal)(10))->interp(Env::empty)->equals(NEW(NumVal)(3)));
    CHECK(parse_str("_if 10 == x _then x _else 15")->subst("x", NEW(NumVal)(10))->interp(Env::empty)->equals(NEW(NumVal)(10)));
    CHECK(parse_str("_if x _then 25 _else 15")->subst("x", NEW(BoolVal)(true))->interp(Env::empty)->equals(NEW(NumVal)(25)));
    CHECK(parse_str("_let x = 10 _in x")->subst("x", NEW(NumVal)(5))->interp(Env::empty)->equals(NEW(NumVal)(10)));
    CHECK((NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y"))))->subst("y", NEW(NumVal)(10))->equals(
            NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(10)))));
    CHECK((NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3))))->subst("x", NEW(NumVal)(10))->equals(
            NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))));
    CHECK(parse_str("(x)(x + y)")->subst("x", NEW(NumVal)(10))->equals(NEW(CallExpr)(NEW(NumExpr)(10),
            NEW(AddExpr)(NEW(NumExpr)(10), NEW(VarExpr)("y")))));
}

TEST_CASE("contains_var") {
    CHECK(!(NEW(NumExpr)(10))->contains_var());
    CHECK((NEW(VarExpr)("x"))->contains_var());
    CHECK((NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(10)))->contains_var());
    CHECK(!(NEW(AddExpr)(NEW(NumExpr)(1), NEW(NumExpr)(10)))->contains_var());
    CHECK((NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(10)))->contains_var());
    CHECK(!(NEW(MultExpr)(NEW(NumExpr)(10), NEW(NumExpr)(10)))->contains_var());
    CHECK((parse_str("5==x"))->contains_var());
    CHECK(!(parse_str("5==5"))->contains_var());
    CHECK((parse_str(" 2 == 1+x"))->contains_var());
    CHECK(!(parse_str("_if 10 == 5 _then 3 _else 4"))->contains_var());
    CHECK((parse_str("_if 10 == x _then 3 _else 4"))->contains_var());
    CHECK((parse_str("_if 10 == 5 _then x _else 4"))->contains_var());
    CHECK((parse_str("_if 10 == 5 _then 3 _else x"))->contains_var());
    CHECK((parse_str("_fun (x) x + 3"))->contains_var());
    CHECK(!(parse_str("_fun (x) 3 + 3"))->contains_var());
    CHECK((parse_str("(x + 3)(10)"))->contains_var());
    CHECK(!(parse_str("(10 + 3)(10)"))->contains_var());
    CHECK((parse_str("(10 + 3)(y)"))->contains_var());
}

TEST_CASE("embedded let") {
    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(1))))->interp(Env::empty)->equals(NEW(NumVal)(6)));
    CHECK_THROWS((NEW(LetExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)), NEW(AddExpr)(NEW(VarExpr)("x"),
            NEW(NumExpr)(3))))->interp(Env::empty)->equals(NEW(NumVal)(8)));
    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(LetExpr)("y", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(2)),
            NEW(AddExpr)(NEW(VarExpr)("y"), NEW(NumExpr)(3)))))->interp(Env::empty)->equals(NEW(NumVal)(10)));
}

TEST_CASE("optimize") {
    CHECK(parse_str("(_let x = 5 _in x) + 3")->optimize()->equals(NEW(NumExpr)(8)));
    CHECK(parse_str("(_let x = 5 _in x) + 3")->optimize()->expr_print() == "8");
    CHECK(parse_str("_let z =(_let x = 5 _in x) _in (z+1)")->optimize()->equals(NEW(NumExpr)(6)));
    CHECK(parse_str("(_let x = 5 _in x) * 3")->optimize()->equals(NEW(NumExpr)(15)));

    CHECK((NEW(NumExpr)(5))->optimize()->equals(NEW(NumExpr)(5)));
    CHECK((NEW(VarExpr)("x"))->optimize()->equals(NEW(VarExpr)("x")));

    CHECK((NEW(AddExpr)(NEW(NumExpr)(5), NEW(NumExpr)(4)))->optimize()->equals(NEW(NumExpr)(9)));
    CHECK((NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)))->optimize()->equals(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5))));

    CHECK((NEW(MultExpr)(NEW(NumExpr)(10), NEW(NumExpr)(5)))->optimize()->equals(NEW(NumExpr)(50)));
    CHECK((NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5)))->optimize()->equals(NEW(MultExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(5))));

    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(NumExpr)(10), NEW(VarExpr)("x"))))->optimize()->equals(NEW(NumExpr)(15)));
    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(2))))->optimize()->equals(NEW(NumExpr)(4)));

    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(5), NEW(LetExpr)("y", NEW(AddExpr)(NEW(VarExpr)("z"), NEW(NumExpr)(2)),
                                                           NEW(AddExpr)(
                                                                   NEW(AddExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("y")),
                                                                   NEW(MultExpr)(NEW(NumExpr)(2),
                                                                                 NEW(NumExpr)(3))))))->optimize()->equals(NEW(LetExpr)("y", NEW(AddExpr)(NEW(VarExpr)("z"),
                                                                                            NEW(NumExpr)(2)), NEW(AddExpr)(NEW(AddExpr)(NEW(NumExpr)(5), NEW(VarExpr)("y")), NEW(NumExpr)(6)))));
    CHECK((NEW(LetExpr)("x", NEW(NumExpr)(3), NEW(MultExpr)(NEW(NumExpr)(7), NEW(VarExpr)("y"))))->optimize()->equals(
            (NEW(MultExpr)(NEW(NumExpr)(7), NEW(VarExpr)("y")))));

    CHECK(parse_str("_let x = y + (3 * 4) _in z")->optimize()->equals(NEW(LetExpr)("x", NEW(AddExpr)(NEW(VarExpr)("y"),
                                                                                                     NEW(NumExpr)(12)), NEW(VarExpr)("z"))));
    CHECK(parse_str("_if 10 == 5 _then 3 _else 4")->optimize()->equals(NEW(NumExpr)(4)));
    CHECK(parse_str("_if 10 == 10 _then 3 _else 4")->optimize()->equals(NEW(NumExpr)(3)));
    CHECK(parse_str("_if 10 == 10 _then 3 * 12 _else 4")->optimize()->equals(NEW(NumExpr)(36)));
    CHECK(parse_str("_if 10 == 5 _then 3 _else 4 + 3 * 3")->optimize()->equals(NEW(NumExpr)(13)));
    CHECK(parse_str("_if 10 == y _then 3 _else 4")->optimize()->equals(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(10), NEW(VarExpr)("y")),
                                                                                   NEW(NumExpr)(3), NEW(NumExpr)(4))));
    CHECK(parse_str("_if _false _then 3 _else 4")->optimize()->equals(NEW(NumExpr)(4)));
    CHECK(parse_str("_if x == 1 + 3 _then 3 _else 4")->optimize()->equals(NEW(IfExpr)(NEW(EqualExpr)(NEW(VarExpr)("x"),
                                                                                                     NEW(NumExpr)(4)), NEW(NumExpr)(3), NEW(NumExpr)(4))));
    CHECK((NEW(IfExpr)(NEW(BoolExpr)(false), NEW(NumExpr)(3), NEW(NumExpr)(5)))->optimize()
                   ->equals(NEW(NumExpr)(5)) );
    CHECK(parse_str(" x == x")->optimize()->equals(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))));
    CHECK(parse_str(" (x) == x")->optimize()->equals(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x"))));
    CHECK(parse_str("_true")->optimize()->equals(NEW(BoolExpr)(true)));
    CHECK(parse_str("_fun (x) x + 3")->optimize()->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))));
    CHECK(parse_str("_fun (x) 3 + 3")->optimize()->equals(NEW(FunExpr)("x", NEW(NumExpr)(6))));
    CHECK(parse_str("(_fun (x) x + 10)(45)")->optimize()->equals(NEW(CallExpr)(NEW(FunExpr)("x",
                                                                                            NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(10))), NEW(NumExpr)(45))));
    CHECK(parse_str("(x + 10)(45)")->optimize()->equals(NEW(CallExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(10)),
                                                                      NEW(NumExpr)(45))));
    CHECK(parse_str("(x + 10)(45)")->optimize()->equals(NEW(CallExpr)(NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(10)),
                                                                      NEW(NumExpr)(45))));
    CHECK(parse_str(
            "_let fib = _fun (fib) _fun (x)_if x == 0 _then 1 _else _if x == 2 + -1 _then 1 _else fib(fib)(x + -1) + fib(fib)(x + -2)_in fib(fib)(10)")->optimize()
                  ->equals(parse_str(" _let fib = _fun (fib)\n"
                                     "              _fun (x)\n"
                                     "                 _if x == 0\n"
                                     "                 _then 1\n"
                                     "                 _else _if x == 1\n"
                                     "                 _then 1\n"
                                     "                 _else fib(fib)(x + -1)\n"
                                     "                       + fib(fib)(x + -2)\n"
                                     " _in fib(fib)(10)")));
}

TEST_CASE("expr_print") {
    PTR(NumExpr) numfive = NEW(NumExpr)(5);
    PTR(NumExpr) numten = NEW(NumExpr)(10);
    PTR(VarExpr) varX = NEW(VarExpr)("x");
    PTR(AddExpr) addTest = NEW(AddExpr)(numfive, numten);
    PTR(AddExpr) addVar = NEW(AddExpr)(numfive, varX);
    PTR(MultExpr) multTest = NEW(MultExpr)(numfive, numten);
    PTR(MultExpr) multAdd = NEW(MultExpr)(addTest, varX);
    PTR(LetExpr) letTest = NEW(LetExpr)("x", numfive, addVar);
    PTR(EqualExpr) equalTest = NEW(EqualExpr)(numfive, numten);
    PTR(IfExpr) ifTest = NEW(IfExpr)(equalTest, numfive, numten);
    PTR(BoolExpr) trueExpr = NEW(BoolExpr)(true);
    PTR(BoolExpr) falseExpr = NEW(BoolExpr)(false);
    PTR(FunExpr) funExpr = NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"),
            NEW(VarExpr)("y")));


    CHECK(numfive->expr_print() == "5");
    CHECK(varX->expr_print() == "x");

    CHECK(addTest->expr_print() == "(5 + 10)");
    CHECK(addVar->expr_print() == "(5 + x)");

    CHECK(multTest->expr_print() == "(5 * 10)");
    CHECK(multAdd->expr_print() == "((5 + 10) * x)");

    CHECK(letTest->expr_print() == "_let x = 5 _in (5 + x)");
    CHECK(equalTest->expr_print() == "(5 == 10)");
    CHECK(ifTest->expr_print() == "_if (5 == 10) _then 5 _else 10");
    CHECK(trueExpr->expr_print() == "_true");
    CHECK(falseExpr->expr_print() == "_false");
    CHECK(funExpr->expr_print() == "(_fun (x) (x + y))");
}



