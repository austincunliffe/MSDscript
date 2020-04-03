//
// Created by Austin Cunliffe on 1/13/20.
//

#include <string>
#include <iostream>
#include <sstream>
#include "parser.hpp"
#include "catch.hpp"

// Take an input stream that contains an expression,
// and returns the parsed representation of that expression.
// Throws `runtime_error` for parse_expr errors.
PTR(Expr)parse(std::istream &in) {
    PTR(Expr)e = parse_expr(in);

    // This peek is currently redundant, since we would have
    // consumed whitespace to decide that the expression
    // doesn't continue.
    char c = peek_after_spaces(in);
    if (!in.eof())
        throw std::runtime_error((std::string)"expected end of file at " + c);

    return e;

}

// Take an input stream that starts with an expression.
// Checks whether we have an EqualExpr.
PTR(Expr)parse_expr(std::istream &in) {
    PTR(Expr)e = parse_comparg(in);

    char c = peek_after_spaces(in);
    if (c == '=') {
        c = in.get();
        c = in.get();
        if (c == '=') {
            PTR(Expr)rhs = parse_expr(in);
            return NEW(EqualExpr)(e, rhs);
        }
    }

    return e;
}

// Takes an input stream that starts with an expression,
// consuming the largest initial expression possible.
static PTR(Expr)parse_comparg(std::istream &in) {
    PTR(Expr)e = parse_addend(in);

    char c = peek_after_spaces(in);

    if (c == '+') {
        in >> c;
        PTR(Expr)rhs = parse_comparg(in);
        e = NEW(AddExpr)(e, rhs);
    }

    return e;
}

// Takes an input stream that starts with an addend,
// consuming the largest initial addend possible, where
// an addend is an expression that does not have `+`
// except within nested expressions (like parentheses).
static PTR(Expr)parse_addend(std::istream &in) {
    PTR(Expr)e = parse_multicand(in);

    char c = peek_after_spaces(in);
    if (c == '*') {
        c = in.get();
        PTR(Expr)rhs = parse_addend(in);
        e = NEW(MultExpr)(e, rhs);
    }

    return e;
}

static PTR(Expr)parse_multicand(std::istream &in) {
    PTR(Expr)expr = parse_inner(in);

    while (in.peek() == '(') {
        in.get();
        PTR(Expr)actual_arg = parse_expr(in);
        expr = NEW(CallExpr)(expr, actual_arg);
        in.get();
    }
    return expr;
}

// Parses something with no immediate `+` or `*` from `in`.
static PTR(Expr)parse_inner(std::istream &in) {
    PTR(Expr)e;

    char c = peek_after_spaces(in);

    if (c == '(') {
        c = in.get();
        e = parse_comparg(in);
        c = peek_after_spaces(in);
        if (c == ')')
            c = in.get();
        else
            throw std::runtime_error("expected a close parenthesis");
    } else if (isdigit(c) || c == '-') {
        e = parse_number(in);
    } else if (isalpha(c)) {
        e = parse_variable(in);
    } else if (c == '_') {
        std::string keyword = parse_keyword(in);
        if (keyword == "_true")
            return NEW(BoolExpr)(true);
        else if (keyword == "_false")
            return NEW(BoolExpr)(false);
        else if (keyword == "_let")
            e = parse_let(in);
        else if (keyword == "_if")
            e = parse_if(in);
        else if (keyword == "_fun")
            e = parse_fun(in);
        else
            throw std::runtime_error((std::string)"unexpected keyword " + keyword);
    } else {
        throw std::runtime_error((std::string)"expected a digit or open parenthesis at " + c);
    }

    return e;
}

// Parses a number, assuming that `in` starts with a digit.
static PTR(Expr)parse_number(std::istream &in) {
    char c = peek_after_spaces(in);

    if (c == '-') c = in.get();

    if (!isnumber(peek_after_spaces(in))) {
        throw std::runtime_error((std::string)"expected number after -");
    }

    int num = 0;
    in >> num;
    if (c == '-') num *= -1;

    return NEW(NumExpr)(num);
}

// Parses an expression, assuming that `in` starts with a
// letter.
static PTR(Expr)parse_variable(std::istream &in) {
    return NEW(VarExpr)(parse_alphabetic(in, ""));
}

// Parses an expression, assuming that `in` starts with a
// letter.
static std::string parse_keyword(std::istream &in) {
    in.get(); // consume `_`
    return parse_alphabetic(in, "_");
}

// Parses an expression, assuming that 'in' starts with an
// underscore.
static PTR(Expr)parse_let(std::istream &in) {

    peek_after_spaces(in);
    std::string var_name = "";
    while (1) {
        char c = in.peek();
        if (!isalpha(c))
            break;
        var_name += in.get();
    }
    // Peek next char, should be '=', then consume.
    if (peek_after_spaces(in) != '=') {
        throw std::runtime_error((std::string)"expected =");
    }
    char c = in.get();

    PTR(Expr)lhs = parse_comparg(in);

    c = peek_after_spaces(in);

    // Check for syntax error in _in.
    std::string _in = "";
    while (!isspace(in.peek())) {
        if (_in.length() >= 3) {
            throw std::runtime_error((std::string)"invalid _in syntax");
        }
        _in += in.get();
    }
    if (_in != "_in") {
        throw std::runtime_error((std::string)"invalid _in syntax");
    }

    PTR(Expr)rhs = parse_comparg(in);

    return NEW(LetExpr)(var_name, lhs, rhs);
}

static PTR(Expr)parse_if(std::istream &in) {
    char c = peek_after_spaces(in);
    PTR(Expr)test_part = parse_expr(in);

    std::string keyword;
    if (peek_after_spaces(in) != '_')
        throw std::runtime_error((std::string) "expected _then");

    keyword = parse_keyword(in);
    if (keyword != "_then")
        throw std::runtime_error((std::string) "expected _then");


    PTR(Expr)then_part = parse_comparg(in);

    if (peek_after_spaces(in) != '_')
        throw std::runtime_error((std::string)"expected _else");

    keyword = parse_keyword(in);
    if (keyword != "_else")
        throw std::runtime_error((std::string)"expected _else");

    PTR(Expr)else_part = parse_comparg(in);

    return NEW(IfExpr)(test_part, then_part, else_part);
}

static PTR(Expr)parse_fun(std::istream &in) {
    char c = peek_after_spaces(in);
    if (c != '(') throw std::runtime_error((std::string) "expected ( in function");
    in.get();
    std::string formal_arg = parse_alphabetic(in, "");

    c = peek_after_spaces(in);
    if (c != ')') throw std::runtime_error((std::string) "expected ) in function");
    in.get();

    PTR(Expr)actual_arg = parse_expr(in);

    return NEW(FunExpr)(formal_arg, actual_arg);
}

// Parses an expression, assuming that `in` starts with a
// letter.
static std::string parse_alphabetic(std::istream &in, std::string prefix) {
    std::string name = prefix;
    while (1) {
        char c = in.peek();
        if (!isalpha(c))
            break;
        name += in.get();
    }
    return name;
}

// Like in.peek(), but consume an whitespace at the
// start of `in`
static char peek_after_spaces(std::istream &in) {
    char c;
    while (1) {
        c = in.peek();
        if (!isspace(c))
            break;
        c = in.get();
    }
    return c;
}

/* for tests */
PTR(Expr)parse_str(std::string s) {
    std::istringstream in(s);
    return parse(in);
}

/* for tests */
std::string parse_str_error(std::string s) {
    std::istringstream in(s);
    try {
        (void) parse(in);
        return "";
    } catch (std::runtime_error exn) {
        return exn.what();
    }
}

TEST_CASE( "See if this works" ) {
    PTR(Expr)ten_plus_one = NEW(AddExpr)(NEW(NumExpr)(10), NEW(NumExpr)(1));

    CHECK ( parse_str_error(" ( 1 ") == "expected a close parenthesis" );

    CHECK( parse_str("10")->equals(NEW(NumExpr)(10)) );
    CHECK( parse_str("(10)")->equals(NEW(NumExpr)(10)) );
    CHECK( parse_str("10+1")->equals(ten_plus_one) );
    CHECK( parse_str("(10+1)")->equals(ten_plus_one) );
    CHECK( parse_str("(10)+1")->equals(ten_plus_one) );
    CHECK( parse_str("10+(1)")->equals(ten_plus_one) );
    CHECK( parse_str("1+2*3")->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                                                  NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("1*2+3")->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(1), NEW(NumExpr)(2)),
                                                  NEW(NumExpr)(3))) );
    CHECK( parse_str("4*2*3")->equals(NEW(MultExpr)(NEW(NumExpr)(4),
                                                   NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("4+2+3")->equals(NEW(AddExpr)(NEW(NumExpr)(4),
                                                  NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("4*(2+3)")->equals(NEW(MultExpr)(NEW(NumExpr)(4),
                                                     NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );
    CHECK( parse_str("(2+3)*4")->equals(NEW(MultExpr)(NEW(AddExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)),
                                                     NEW(NumExpr)(4))) );
    CHECK( parse_str("xyz")->equals(NEW(VarExpr)("xyz")) );
    CHECK( parse_str("xyz+1")->equals(NEW(AddExpr)(NEW(VarExpr)("xyz"), NEW(NumExpr)(1))) );

    CHECK ( parse_str_error("!") == "expected a digit or open parenthesis at !" );
    CHECK ( parse_str_error("(1") == "expected a close parenthesis" );

    CHECK( parse_str(" 10 ")->equals(NEW(NumExpr)(10)) );
    CHECK( parse_str(" (  10 ) ")->equals(NEW(NumExpr)(10)) );
    CHECK( parse_str(" 10  + 1")->equals(ten_plus_one) );
    CHECK( parse_str(" ( 10 + 1 ) ")->equals(ten_plus_one) );
    CHECK( parse_str(" 11 * ( 10 + 1 ) ")->equals(NEW(MultExpr)(NEW(NumExpr)(11),
                                                               ten_plus_one)) );
    CHECK( parse_str(" ( 11 * 10 ) + 1 ")
                   ->equals(NEW(AddExpr)(NEW(MultExpr)(NEW(NumExpr)(11), NEW(NumExpr)(10)),
                                        NEW(NumExpr) (1))) );
    CHECK( parse_str(" 1 + 2 * 3 ")
                   ->equals(NEW(AddExpr)(NEW(NumExpr)(1),
                                        NEW(MultExpr)(NEW(NumExpr)(2), NEW(NumExpr)(3)))) );

    CHECK( parse_str_error(" ! ") == "expected a digit or open parenthesis at !" );
    CHECK( parse_str_error(" ( 1 ") == "expected a close parenthesis" );
    CHECK( parse_str_error(" 1 )") == "expected end of file at )" );

    CHECK( parse_str("_true")->equals(NEW(BoolExpr)(true)) );
    CHECK( parse_str("_false")->equals(NEW(BoolExpr)(false)) );

    CHECK( parse_str_error("_maybe ") == "unexpected keyword _maybe" );

    CHECK( parse_str("10 == 5")->equals(NEW(EqualExpr)(NEW(NumExpr)(10), NEW(NumExpr)(5))));
    CHECK( parse_str("x == 8")->equals(NEW(EqualExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(8))));

    CHECK( parse_str("_if 10 == 5 _then 3 _else 4")->equals(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(10), NEW(NumExpr)(5)),
            NEW(NumExpr)(3), NEW(NumExpr)(4))));
    CHECK( parse_str("_let x = (_if 10 == 5 _then 3 _else 4) + 5 _in x + 3")->equals(NEW(LetExpr)("x",
            NEW(AddExpr)(NEW(IfExpr)(NEW(EqualExpr)(NEW(NumExpr)(10), NEW(NumExpr)(5)), NEW(NumExpr)(3), NEW(NumExpr)(4)),
                    NEW(NumExpr)(5)), NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))));
    CHECK( parse_str("_if _true _then 1 _else 0")->equals(NEW(IfExpr)(NEW(BoolExpr)(true), NEW(NumExpr)(1), NEW(NumExpr)(0))));
    CHECK( parse_str(" -5 ")->equals(NEW(NumExpr)(-5)));
    CHECK( parse_str(" -5 * 1")->equals(NEW(MultExpr)(NEW(NumExpr)(-5), NEW(NumExpr)(1))));
    CHECK( parse_str(" (-5)")->equals(NEW(NumExpr)(-5)));
    CHECK( parse_str(" (-5) == (-5)")->equals(NEW(EqualExpr)(NEW(NumExpr)(-5), NEW(NumExpr)(-5))));
    CHECK( parse_str("_if x _then 25 _else 15")->equals(NEW(IfExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(25), NEW(NumExpr)(15))));
    CHECK( parse_str("_if _true == _true _then 25 _else 15")->equals(NEW(IfExpr)(NEW(EqualExpr)((NEW(BoolVal)(true))->to_expr(),
           (NEW(BoolVal)(true))->to_expr()), NEW(NumExpr)(25), NEW(NumExpr)(15))));
    CHECK( parse_str("_fun (x) x + 3")->equals(NEW(FunExpr)("x", NEW(AddExpr)(NEW(VarExpr)("x"), NEW(NumExpr)(3)))));
    CHECK( parse_str("(10)(4)")->equals(NEW(CallExpr)(NEW(NumExpr)(10), NEW(NumExpr)(4))));
    CHECK( parse_str("_let f = _fun (x) _fun (y) x*x + y*y _in f(2)(3)")->equals(NEW(LetExpr)("f", NEW(FunExpr)("x",
            NEW(FunExpr)("y", NEW(AddExpr)(NEW(MultExpr)(NEW(VarExpr)("x"), NEW(VarExpr)("x")), NEW(MultExpr)(NEW(VarExpr)("y"),
                    NEW(VarExpr)("y"))))), NEW(CallExpr)(NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(2)),
                            NEW(NumExpr)(3)))));
    CHECK( parse_str("(f(10))")->equals(NEW(CallExpr)(NEW(VarExpr)("f"), NEW(NumExpr)(10))));
}