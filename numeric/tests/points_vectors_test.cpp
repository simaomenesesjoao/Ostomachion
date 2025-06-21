#include "points_vectors.cpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

TEST_CASE("Point rotation", "[custom]"){
    
    using Num = Number<int, 2, 3>;
        Point<Num> P{1,1};
        Angle<Num> a{1,-1};
        Point<Num> Q = P.rotate(a); 

        REQUIRE(Q.get_x() != Q.get_y());
}
