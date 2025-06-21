#include "fraction.cpp"
#include <gmpxx.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

TEST_CASE("Fraction subtraction", "[custom]"){
    Fraction<int> xi{4,3}, yi{2,5}, ri{14,15};
    Fraction<long> xl{4,3}, yl{2,5}, rl{14,15};
    Fraction<mpz_class> xm{4,3}, ym{2,5}, rm{14,15};
    
    REQUIRE(xi-yi == ri);
    REQUIRE(xl-yl == rl);
    REQUIRE(xm-ym == rm);
}

TEST_CASE("Negative fractions", "[custom]"){
    using T = int;
    REQUIRE(Fraction<T>{ 2,-5} == Fraction<T>{-2,5});
    REQUIRE(Fraction<T>{-1,-1} == Fraction<T>{ 1,1});
}

TEST_CASE("Fraction comparison", "[custom]"){
    using T = int;
    Fraction<T> x{4,3}, y{2,5};
    REQUIRE(y<x);
}