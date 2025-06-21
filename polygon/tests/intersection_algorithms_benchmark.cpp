#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "polygon.cpp"
#include "customFloat.cpp"
#include "intersection_algorithms.cpp"

template <typename T>
bool bench_edges_intersect(){
    using P = Point<T>;
    P A{0,0}, B{1,2}, C{0,1}, D{1,0}, E{1,-1};

    //   | B
    //   C
    // --AD---->
    //   |E
    bool i1 = edges_intersect(A, B, C, D);
    bool i2 = edges_intersect(A, E, C, D);
    return i1 and i2;
}

TEST_CASE("edges_intersect", "[Number vs Float]"){

    BENCHMARK("edges_intersect 1"){
        return bench_edges_intersect<Number<mpz_class, 2, 3>>();
    };
    BENCHMARK("edges_intersect 2"){
        return bench_edges_intersect<Number<mpz_class, 2, 5, 13, 17>>();
    };
    BENCHMARK("edges_intersect 3"){
        return bench_edges_intersect<Number<int, 2, 3>>();
    };
    BENCHMARK("edges_intersect 4"){
        return bench_edges_intersect<Float<double>>();
    };
    BENCHMARK("edges_intersect 5"){
        return bench_edges_intersect<Float<float>>();
    };
}

template <typename T>
bool bench_point_on_edge(){
    using P = Point<T>;
    
    P A{0,0}, B{1,1}, C{2,2}, D{1,0}, E{1,-1};

    //   | C
    //   |B
    // --AD---->
    //   |E

    bool i1 = point_on_edge(A, B, C);
    bool i2 = point_on_edge(A, C, B);
    return i1 and i2;
}

TEST_CASE("point_on_edge", "[Number vs Float]"){

    BENCHMARK("point_on_edge 1"){
        return bench_point_on_edge<Number<mpz_class, 2, 3>>();
    };
    BENCHMARK("point_on_edge 2"){
        return bench_point_on_edge<Number<mpz_class, 2, 5, 13, 17>>();
    };
    BENCHMARK("point_on_edge 3"){
        return bench_point_on_edge<Number<int, 2, 3>>();
    };
    BENCHMARK("point_on_edge 4"){
        return bench_point_on_edge<Float<double>>();
    };
    BENCHMARK("point_on_edge 5"){
        return bench_point_on_edge<Float<float>>();
    };
}






template <typename T>
bool bench_point_inside(){
    Polygon::LLPoly<T> poly({{0,0}, {12,0}, {12,12}, {6,6}, {0,12}}); // Anti-clockwise polygon
    bool i1 = poly.is_point_inside({10,2 });  // inside
    bool i2 = poly.is_point_inside({12,10}); // on the border
    bool i3 = poly.is_point_inside({13,10}); // outside
    return i1 and i2 and i3;
}

TEST_CASE("point_inside", "[Number vs Float]"){

    BENCHMARK("point_inside 1"){
        return bench_point_inside<Number<mpz_class, 2, 3>>();
    };
    BENCHMARK("point_inside 2"){
        return bench_point_inside<Number<mpz_class, 2, 5, 13, 17>>();
    };
    BENCHMARK("point_inside 3"){
        return bench_point_inside<Number<int, 2, 3>>();
    };
    BENCHMARK("point_inside 4"){
        return bench_point_inside<Float<double>>();
    };
    BENCHMARK("point_inside 5"){
        return bench_point_inside<Float<float>>();
    };
}





