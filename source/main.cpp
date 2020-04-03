#include <iostream>
#include "parser.hpp"

#define CATCH_CONFIG_RUNNER

#include "catch.hpp"
#include "pointer.hpp"
#include "Step.hpp"

int main(int argc, char **argv) {
    try {
        PTR(Expr) e;
        bool optimize_mode = false;
        bool step_mode = false;

        if ((argc > 1) && !strcmp(argv[1], "--opt")) {
            optimize_mode = true;
            argc--;
            argv++;
        } else if ((argc > 1) && !strcmp(argv[1], "--step")) {
            step_mode = true;
            argc--;
            argv++;
        } else if ((argc > 1) && !strcmp(argv[1], "--test")) {
            std::cout << Catch::Session().run();
            return 0;
    }

    if (argc > 1) {
            std::ifstream prog_in(argv[1]);
            e = parse(prog_in);
        } else {
            e = parse(std::cin);
        }

        if (optimize_mode)
            std::cout << e->optimize()->expr_print() << "\n";
        else if (step_mode)
            std::cout << Step::interp_by_steps(e)->to_string();
        else
            std::cout << e->interp(Env::empty)->to_string() << "\n";
        return 0;
    } catch (std::runtime_error error) {
        std::cerr << error.what() << "\n";
        return 1;
    }
}