#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "intersection_algorithms.cpp"
#include "polygon.cpp"

TEST_CASE("edges_intersect", "[custom]"){
    using P = Point<Number<int, 2, 3>>;
    P A{0,0}, B{1,2}, C{0,1}, D{1,0}, E{1,-1};

    //   | B
    //   C
    // --AD---->
    //   |E

    REQUIRE(edges_intersect(A, B, C, D) == true);
    REQUIRE(edges_intersect(A, E, C, D) == false);
    REQUIRE(edges_intersect(A, B, A, B) == false);
    REQUIRE(edges_intersect(A, B, D, E) == false);
    REQUIRE(edges_intersect(A, C, A, D) == false);
    REQUIRE(edges_intersect(C,-C, A, D) == false);
}


TEST_CASE("point_on_edge", "[custom]"){
    using P = Point<Number<int, 2, 3>>;
    
    P A{0,0}, B{1,1}, C{2,2}, D{1,0}, E{1,-1};

    //   | C
    //   |B
    // --AD---->
    //   |E

    REQUIRE(point_on_edge(A, B, C) == false);
    REQUIRE(point_on_edge(A, C, B) == true);
    REQUIRE(point_on_edge(A, B, D) == false);
    REQUIRE(point_on_edge(A, B, B) == false);
}

TEST_CASE("edge_intersects_vertex", "[custom]"){
    Polygon::LLPoly<Number<int, 2, 3>> poly({{0,0}, {5,0}, {5,5}, {0,5}});
    REQUIRE(is_inner_vertex({-1,-1}, {1,1}, poly.get_head()) == false);
    REQUIRE(is_inner_vertex({-1,1}, {1,-1}, poly.get_head()) == false);
    REQUIRE(is_inner_vertex({1,-1}, {-1,1}, poly.get_head()) == true);
}


TEST_CASE("edge_splits_vertex", "[custom]"){
    Polygon::LLPoly<Number<int, 2, 3>> poly({{0,0}, {5,0}, {5,5}, {0,5}});
    REQUIRE(edge_splits_vertex({-1,-1}, {1,1}, poly.get_head()) == true);
    REQUIRE(edge_splits_vertex({-1,1}, {1,-1}, poly.get_head()) == false);
    REQUIRE(edge_splits_vertex({1,-1}, {-1,1}, poly.get_head()) == false);
    
}

TEST_CASE("shoelace area formula", "[custom]"){
    using Num = Number<int, 2, 3>;
    Point<Num> A{0,0}, B{1,0}, C{1,1}, D{0,1}; 
    REQUIRE((shoelace_area<Num>({A,B,C,D})) == 1); // anticlockwise polygon > 0
    REQUIRE((shoelace_area<Num>({D,C,B,A})) == -1); // clockwise polygon < 0
}


TEST_CASE("nodes_compatible", "[custom]"){
    Vertex<Number<int, 2, 3, 5>> M({0,0}), N({0,0}), P({0,0}), A({0,0}), B({0,0});

    // /_
    A.update_start({1,1});
    A.update_end({1,0});


    // //
    B.update_start({1,2});
    B.update_end({2,1});

    // /_
    M.update_start({1,0});
    M.update_end({1,1});

    // _|
    N.update_start({0,1});
    N.update_end({-1,0});

    // |_
    P.update_start({1,0});
    P.update_end({0,1});

    REQUIRE(nodes_compatible(M,N) == true);
    REQUIRE(nodes_compatible(M,P) == false);
    REQUIRE(nodes_compatible(N,P) == true);
    REQUIRE(nodes_compatible(A,M) == true);
    REQUIRE(nodes_compatible(A,P) == false);
    REQUIRE(nodes_compatible(B,P) == false);
}

TEST_CASE("angles_compatible", "[custom]"){
    Angle<Number<int, 2, 3>> A{0,1}, B{-1,1}, C{-1,-1};
    REQUIRE(angles_compatible(A,B) == true);
    REQUIRE(angles_compatible(C,C) == false);
    REQUIRE(angles_compatible(A,C) == true);
    REQUIRE(angles_compatible(B,C) == true);
}