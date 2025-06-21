#include "fracroot.cpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

TEST_CASE("FracRoot constructor", "[custom]"){
    using T = int;
    FracRoot<T,1,2,5> x{{6,3},40};
    REQUIRE(x.get_num() == 4);
    REQUIRE(x.get_den() == 1);
    REQUIRE(x.get_root() == 10);
}

TEST_CASE("FracRoot constructor, root becomes integer", "[custom]"){
    using T = int;
    FracRoot<T,1,2,5> x{{-1,7},100};
    REQUIRE(x.get_num() == -10);
    REQUIRE(x.get_den() == 7);
    REQUIRE(x.get_root() == 1);
}

TEST_CASE("FracRoot product and subtraction", "[custom]"){
    using T = int;
    FracRoot<T,1,2,5> x{{6,3},40};
    FracRoot<T,1,2,5> y{{3,6},5};
    FracRoot<T,1,2,5> z{{4,6},5};
    FracRoot<T,1,2,5> xy{{10,1},2};
    FracRoot<T,1,2,5> yz{{-1,6},5};

    REQUIRE(x*y - xy == 0);
    REQUIRE(y-z - yz == 0);
}

TEST_CASE("FracRoot product and addition", "[custom]"){
    using T = int;
    FracRoot<T> x{{6,3},1};
    FracRoot<T> y{{3,6},1};
    FracRoot<T> x_t_y{{1,1},1};
    FracRoot<T> x_p_y{{5,2},1};

    REQUIRE(x*y - x_t_y == 0);
    REQUIRE(x+y - x_p_y == 0);
}
