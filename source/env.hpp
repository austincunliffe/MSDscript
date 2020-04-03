//
// Created by Austin Cunliffe on 3/2/20.
//

#pragma once

#include "pointer.hpp"
#include "value.hpp"

class Env ENABLE_THIS(Env){
public:
    virtual PTR(Val) lookup(std::string find_name) = 0;
    static PTR(Env) empty;
};


class EmptyEnv: public Env {
public:
    PTR(Val) lookup(std::string find_name);
};


class ExtendedEnv: public Env {
public:
    std::string name;
    PTR(Val) val;
    PTR(Env) rest;

    ExtendedEnv(PTR(Env) env, std::string var, PTR(Val) rhs_val);

    PTR(Val) lookup(std::string find_name);
};