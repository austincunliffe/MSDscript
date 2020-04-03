//
// Created by Austin Cunliffe on 3/2/20.
//

#include "env.hpp"

PTR(Val) EmptyEnv::lookup(std::string find_name) {
    throw std::runtime_error("free variable: " + find_name);
}

ExtendedEnv::ExtendedEnv(PTR(Env) env, std::string var, PTR(Val) rhs_val) {
    this->rest = env;
    this->name = var;
    this->val = rhs_val;
}

PTR(Val) ExtendedEnv::lookup(std::string find_name) {
    if (name == find_name)
        return val;
    else
        return rest->lookup(find_name);
}