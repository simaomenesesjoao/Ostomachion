#include <cassert>
#include <gmpxx.h>
#include "polygon.cpp"
#include "customFloat.cpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_template_test_macros.hpp>



using N1 = Number<int, 2, 5, 13, 17>;
using N2 = Float<double>;
using N3 = Number<mpz_class, 2, 5, 13, 17>;

using P1 = Polygon::LLPoly<N1>;
using P2 = Polygon::LLPoly<N2>;
using P3 = Polygon::ContigPoly<N1>;
using P4 = Polygon::ContigPoly<N2>;

using P5 = Polygon::LLPoly<N3>;
using P6 = Polygon::ContigPoly<N3>;


TEMPLATE_TEST_CASE("Polygon insert", "-", P1, P2, P3, P4, P5, P6){

    TestType poly1({{6,6}, {6,-6}, {-6,-6}, {-6,6}});
    TestType poly2({{0,0}, {-12,12}, {0,12}});
    TestType poly3({{0,0}, {0,12}, {3,0}});
    poly1.connect_LL();
    poly2.connect_LL();
    poly3.connect_LL();

    unsigned index1 = poly1.get_leftest_node();
    unsigned index2 = poly2.get_leftest_node();
    unsigned index3 = poly3.get_leftest_node();

    const auto *node1 = poly1.vertex_from_index(index1);
    const auto *node2 = poly2.vertex_from_index(index2);
    const auto *node3 = poly3.vertex_from_index(index3);
    
    REQUIRE(node1 == poly1.get_head()->next->next);
    REQUIRE(node2 == poly2.get_head()->next);
    REQUIRE(node3 == poly3.get_head());
}