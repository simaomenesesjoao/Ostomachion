#include "points_vectors.cpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

TEST_CASE("Angle equality", "[custom]"){
    using Num = Number<int,2,3>;
    Angle<Num> angle1(1,1);
    Angle<Num> angle2(2,2);

    REQUIRE(angle1 == angle1);
    REQUIRE(angle1 == angle2);
}

TEST_CASE("Angle sines and cosines", "[custom]"){
    using Num = Number<int,2>;
    Angle<Num> angle1(2,0);
    Angle<Num> angle2(0,1);
    Angle<Num> angle3(1,1);
    REQUIRE(angle1.get_cos() == 1);
    REQUIRE(angle1.get_sin() == 0);
    REQUIRE(angle2.get_cos() == 0);
    REQUIRE(angle2.get_sin() == 1);
    REQUIRE(angle3.get_cos() == angle3.get_sin());
    REQUIRE(!angle3.get_cos().is_zero());
}

TEST_CASE("Angle sum and subtraction", "[custom]"){
    using Num = Number<int,2,3>;
    Angle<Num> angle1(1,1); // 45
    Angle<Num> angle2(-1,1); // 135
    auto angle3 = angle2 + angle1; // 180
    auto angle4 = angle2 - angle1; // 90
    REQUIRE(angle3.get_sin() == 0);
    REQUIRE(angle3.get_cos() == -1);
    REQUIRE(angle4.get_sin() == 1);
    REQUIRE(angle4.get_cos() == 0);
}

TEST_CASE("Angle comparison <", "[custom]"){
    using Num = Number<int,2,5>;
    Angle<Num> A{0,1}; //  90º 
    Angle<Num> B{1,1}; //  45º
    Angle<Num> C{1,2}; // ~66º?
    Angle<Num> D{-1,-1}; // 225º
    Angle<Num> E{1,-1}; // 315º

    REQUIRE(A.is_larger_than_180() == false);
    REQUIRE(B.is_larger_than_180() == false);
    REQUIRE(C.is_larger_than_180() == false);
    REQUIRE(D.is_larger_than_180() == true);
    REQUIRE(E.is_larger_than_180() == true);

    REQUIRE((A<B) == false);
    REQUIRE((B<C) == true);
    REQUIRE((B<B) == false);
    REQUIRE((D<B) == false);
    REQUIRE((D<E) == true);
    REQUIRE((A<E) == true);
}