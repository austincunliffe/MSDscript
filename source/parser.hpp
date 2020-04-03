//
// Created by Austin Cunliffe on 1/13/20.
//

#pragma once

#include "Expr.hpp"
#include "pointer.hpp"
/**
 * Receives a istream representation of an Expr and parses it into an Expr.
 * @param in istream to parse.
 * @return Expr that has been parsed from the istream.
 */
PTR(Expr)parse(std::istream &in);
static PTR(Expr)parse_expr(std::istream &in);
static PTR(Expr)parse_comparg(std::istream &in);
static PTR(Expr)parse_addend(std::istream &in);
static PTR(Expr)parse_multicand(std::istream &in);
static PTR(Expr)parse_inner(std::istream &in);
static PTR(Expr)parse_number(std::istream &in);
static PTR(Expr)parse_variable(std::istream &in);
static PTR(Expr)parse_let(std::istream &in);
static PTR(Expr)parse_if(std::istream &in);
static PTR(Expr)parse_fun(std::istream &in);
static std::string parse_keyword(std::istream &in);
static std::string parse_alphabetic(std::istream &in, std::string prefix);
static char peek_after_spaces(std::istream &in);

/**
 * Receives a string representation of an Expr and parses it into an Expr.
 * @param s string to parse.
 * @return Expr that has been parsed from the string.
 */
PTR(Expr)parse_str(std::string s);
std::string parse_str_error(std::string s);