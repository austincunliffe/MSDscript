//
// Created by Austin Cunliffe on 1/29/20.
//

#include "value.hpp"

#include "Expr.hpp"
#include <stdexcept>
#include "catch.hpp"
#include "Step.hpp"

NumVal::NumVal(int rep) {
    this->rep = rep;
}

bool NumVal::equals(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        return false;
    else
        return rep == other_num_val->rep;
}

PTR(Val) NumVal::add_to(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        throw std::runtime_error("not a number");
    else
        return NEW(NumVal)(rep + other_num_val->rep);
}

PTR(Val) NumVal::mult_with(PTR(Val) other_val) {
    PTR(NumVal) other_num_val = CAST(NumVal)(other_val);
    if (other_num_val == nullptr)
        throw std::runtime_error("not a number");
    else
        return NEW(NumVal)(rep * other_num_val->rep);
}

PTR(Expr) NumVal::to_expr() {
    return NEW(NumExpr)(rep);
}

std::string NumVal::to_string() {
    return std::to_string(rep);
}

bool NumVal::is_true() {
    throw std::runtime_error("a number cannot be interpreted as a boolean");
}

PTR(Val) NumVal::call(PTR(Val) actual_arg) {
    return NEW(NumVal)(rep);
}

void NumVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("Cannot call call_step on a NumVal.");
}

BoolVal::BoolVal(bool rep) {
    this->rep = rep;
}

bool BoolVal::equals(PTR(Val) other_val) {
    PTR(BoolVal) other_bool_val = CAST(BoolVal)(other_val);
    if (other_bool_val == nullptr)
        return false;
    else
        return rep == other_bool_val->rep;
}

PTR(Val) BoolVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("no adding booleans");
}

PTR(Val) BoolVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("no multiplying booleans");
}

PTR(Expr) BoolVal::to_expr() {
    return NEW(BoolExpr)(rep);
}

std::string BoolVal::to_string() {
    if (rep)
        return "_true";
    else
        return "_false";
}

bool BoolVal::is_true() {
    return rep == true;
}

PTR(Val) BoolVal::call(PTR(Val) actual_arg) {
    return NEW(BoolVal)(rep);
}

void BoolVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    throw std::runtime_error("Cannot call call_step on a BoolVal.");
}

FunVal::FunVal(std::string formal_arg, PTR(Expr) body, PTR(Env) env) {
    this->formal_arg = formal_arg;
    this->body = body;
    this->env = env;
}

bool FunVal::equals(PTR(Val) other_val) {
    PTR(FunVal) f = CAST(FunVal)(other_val);
    if (f == nullptr)
        return false;
    else
        return formal_arg == f->formal_arg && body->equals(f->body);
}

PTR(Val) FunVal::add_to(PTR(Val) other_val) {
    throw std::runtime_error("no adding functions");
}

PTR(Val) FunVal::mult_with(PTR(Val) other_val) {
    throw std::runtime_error("no multiplying functions");
}

PTR(Expr) FunVal::to_expr() {
    return NEW(FunExpr)(formal_arg, body);
}

std::string FunVal::to_string() {
    return "[function]";
}

bool FunVal::is_true() {
    return false;
}

PTR(Val) FunVal::call(PTR(Val) actual_arg) {
    return this->body->interp(NEW(ExtendedEnv)(env, formal_arg, actual_arg));
}

void FunVal::call_step(PTR(Val) actual_arg_val, PTR(Cont) rest) {
    Step::mode = Step::interp_mode;
    Step::expr = body;
    Step::env = NEW(ExtendedEnv)(env, formal_arg, actual_arg_val);
    Step::cont = rest;
}

TEST_CASE( "values equals" ) {
    CHECK( (NEW(NumVal)(5))->equals(NEW(NumVal)(5)) );
    CHECK( ! (NEW(NumVal)(7))->equals(NEW(NumVal)(5)) );

    CHECK( (NEW(BoolVal)(true))->equals(NEW(BoolVal)(true)) );
    CHECK( ! (NEW(BoolVal)(true))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(BoolVal)(true)) );

    CHECK( ! (NEW(NumVal)(7))->equals(NEW(BoolVal)(false)) );
    CHECK( ! (NEW(BoolVal)(false))->equals(NEW(NumVal)(8)) );
}

TEST_CASE( "add_to" ) {

    CHECK ( (NEW(NumVal)(5))->add_to(NEW(NumVal)(8))->equals(NEW(NumVal)(13)) );

    CHECK_THROWS_WITH ( (NEW(NumVal)(5))->add_to(NEW(BoolVal)(false)), "not a number" );
    CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->add_to(NEW(BoolVal)(false)),
                        "no adding booleans" );
}

TEST_CASE( "mult_with" ) {

    CHECK ( (NEW(NumVal)(5))->mult_with(NEW(NumVal)(8))->equals(NEW(NumVal)(40)) );

    CHECK_THROWS_WITH ( (NEW(NumVal)(5))->mult_with(NEW(BoolVal)(false)), "not a number" );
    CHECK_THROWS_WITH ( (NEW(BoolVal)(false))->mult_with(NEW(BoolVal)(false)),
                        "no multiplying booleans" );
}

TEST_CASE( "interp to_expr" ) {
    CHECK( (NEW(NumVal)(5))->to_expr()->equals(NEW(NumExpr)(5)) );
    CHECK( (NEW(BoolVal)(true))->to_expr()->equals(NEW(BoolExpr)(true)) );
    CHECK( (NEW(BoolVal)(false))->to_expr()->equals(NEW(BoolExpr)(false)) );
}

TEST_CASE( "interp to_string" ) {
    CHECK( (NEW(NumVal)(5))->to_string() == "5" );
    CHECK( (NEW(BoolVal)(true))->to_string() == "_true" );
    CHECK( (NEW(BoolVal)(false))->to_string() == "_false" );
    CHECK( (NEW(FunVal)("x", NEW(NumExpr)(10), Env::empty))->to_string() == "[function]");
}

TEST_CASE("is_true") {
    CHECK_THROWS_WITH( (NEW(NumVal)(5))->is_true() == false , "a number cannot be interpreted as a boolean");
    CHECK( (NEW(BoolVal)(true))->is_true() == true );
    CHECK( (NEW(BoolVal)(false))->is_true() == false );
    CHECK( (NEW(FunVal)("x", NEW(NumExpr)(10), Env::empty))->is_true() == false);
}