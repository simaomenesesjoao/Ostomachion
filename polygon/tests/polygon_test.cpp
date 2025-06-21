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

TEMPLATE_TEST_CASE("Polygon constructor", "-", P1, P2, P3, P4){

    TestType poly{Polygon::BarePoly{}};
    REQUIRE(poly.get_head() == nullptr);
}


TEST_CASE("LL Polygon copy Number"){

    using Num = Number<int, 2, 5, 13, 17>;
    using Poly = Polygon::LLPoly<Num>;
    
    Poly poly1({{0,0}, {0,12}, {12,12}});
    Poly poly2(poly1);

    REQUIRE(poly1 == poly2);
}


TEST_CASE("LL Polygon flip_x Number"){

    using Num = Number<int, 2, 5, 13, 17>;
    using Poly = Polygon::LLPoly<Num>;
    
    Poly poly1({{0,0}, {0,12}, {12,12}});
    Poly poly2({{0,0}, {-12,12}, {0,12}});
    poly1.flip_x();

    REQUIRE(poly1 == poly2);
}


TEMPLATE_TEST_CASE("Polygon is_point_inside details", "-", P1, P2, P3, P4){

    TestType poly({{0,0}, {12,0}, {12,12}, {6,6}, {0,12}}); // Anti-clockwise polygon
    TestType poly2({{0,12}, {6,6}, {12,12}, {12,0}, {0,0}}); // Clockwise polygon

    poly.connect_LL();
    poly2.connect_LL();
    REQUIRE(poly.is_point_inside({10,2 }) == true);  // inside
    REQUIRE(poly.is_point_inside({12,10}) == false); // on the border
    REQUIRE(poly.is_point_inside({13,10}) == false); // outside

    REQUIRE(poly2.is_point_inside({10,2 }) == false);  // inside
    REQUIRE(poly2.is_point_inside({12,10}) == false); // on the border
    REQUIRE(poly2.is_point_inside({13,10}) == true); // outside

    REQUIRE(poly.is_point_inside({1,1}, {-1,-1}) == true);  // inside, cross vertex
    REQUIRE(poly.is_point_inside({5,5}, {13,13}) == true);  // inside, coincident edge
    REQUIRE(poly.is_point_inside({-1,0}, {13,0}) == false); // outside, coincident edge
}

TEMPLATE_TEST_CASE("Polygon intersection", "-", P1, P2, P3, P4){

    TestType poly1({{0,0}, {3,0}, {3,3}, {0,3}});
    TestType poly2({{1,1}, {2,1}, {2,2}, {1,2}});
    TestType poly3({{2,2}, {4,2}, {4,4}, {2,4}});
    TestType poly4({{1,0}, {2,0}, {2,1}, {1,1}});
    TestType poly5({{3,0}, {6,3}, {3,6}, {0,3}});
    TestType poly6({{0,0}, {0,1}, {1,1}, {2,2}, {2,0}, {1,-1}});
    TestType poly7({{0,0}, {1,1}, {2,0}});
    TestType poly8({{1,0}, {3,1}, {2,3}, {0,2}});
    TestType poly9({{1,0}, {2,0}, {2,3}, {1,3}});
    TestType poly10({{5,0}, {6,0}, {6,1}, {5,1}});
    poly1.connect_LL();
    poly2.connect_LL();
    poly3.connect_LL();
    poly4.connect_LL();
    poly5.connect_LL();
    poly6.connect_LL();
    poly7.connect_LL();
    poly8.connect_LL();
    poly9.connect_LL();
    poly10.connect_LL();

    SECTION("edge-edge intersection"){
        REQUIRE(poly1.edge_edge_intersection(poly2) == false);
        REQUIRE(poly1.edge_edge_intersection(poly3) == true);
        REQUIRE(poly1.edge_edge_intersection(poly4) == false);    
    }

    SECTION("points inside"){
        REQUIRE(poly1.points_inside(poly2) == true);
        REQUIRE(poly2.points_inside(poly1) == false);
        REQUIRE(poly1.points_inside(poly3) == true);
        REQUIRE(poly3.points_inside(poly1) == true);
        
        REQUIRE(poly1.points_inside(poly1) == false);
        REQUIRE(poly4.points_inside(poly3) == false);
    }


    SECTION("node-node intersection"){
        REQUIRE(poly1.node_node_intersection(poly5) == true);
        REQUIRE(poly1.node_node_intersection(poly4) == false);
        REQUIRE(poly6.node_node_intersection(poly7) == true);
    }

    SECTION("edge-node intersection"){
        REQUIRE(poly1.edge_node_intersection(poly8) == true);
        REQUIRE(poly8.edge_node_intersection(poly1) == false);
        REQUIRE(poly1.edge_node_intersection(poly9) == true);
        REQUIRE(poly9.edge_node_intersection(poly1) == false);
    }

    SECTION("overlap"){
        REQUIRE(poly1.overlaps(poly2));
        REQUIRE(poly1.overlaps(poly3));
        REQUIRE(poly1.overlaps(poly1));
        REQUIRE(poly1.overlaps(poly5));
        REQUIRE(poly6.overlaps(poly7));
        REQUIRE(poly1.overlaps(poly8));
        REQUIRE(poly1.overlaps(poly9));
        REQUIRE(!poly1.overlaps(poly10));
    }
    
}





TEMPLATE_TEST_CASE("Polygon merge", "-", P1, P2, P3, P4){

    TestType poly1({{0,0}, {0,12}, {12,12}});
    TestType poly2({{0,0}, {0,6}, {6,6}});
    TestType poly3({{0,0}, {0,6}, {6,6}, {0,0}, {0,12}, {12,12}});

    poly2.merge(poly2.get_head(), poly1, poly1.get_head());
    REQUIRE(poly2 == poly3);
}

TEMPLATE_TEST_CASE("Polygon is circular LL preserved", "-", P1, P2, P3, P4, P5, P6){

    TestType poly1({{0,0}, {0,12}, {12,12}});
    TestType poly2({{0,0}, {0,6}, {6,6}});

    poly2.merge(poly2.get_head(), poly1, poly1.get_head());
    poly2.connect_LL();

    auto *node = poly2.get_head();
    for(unsigned i=0; i<poly2.size()*2-1; i++){
        node = node->next;
    }
    REQUIRE(node == poly2.get_head()->prev);

    node = poly2.get_head();
    for(unsigned i=0; i<poly2.size()*2+1; i++){
        node = node->prev;
    }
    REQUIRE(node == poly2.get_head()->prev);

}


TEMPLATE_TEST_CASE("Polygon prune square+triangle", "-", P1, P2, P3, P4, P5, P6){

    TestType poly1({{0,0}, {0,12}, {12,12}, {12,0}});
    TestType poly2({{0,0}, {3,0}, {3,3}, {0,3}});
    TestType poly3({{3,0}, {3,3}, {0,3}, {0,12}, {12,12}, {12,0}});

    auto *node1 = poly1.get_head();
    auto *node2 = poly2.get_head();
    auto getter = []([[maybe_unused]] unsigned int i){return 0;};
    auto modified = poly2.merge(node2, poly1, node1);
    poly2.prune_LL(modified, getter);
    REQUIRE(poly2 == poly3);

}

TEMPLATE_TEST_CASE("Polygon prune two rectangles", "-", P1, P2, P3, P4, P5, P6){

    TestType poly1({{0,0}, {0,12}, {12,12}, {12,0}});
    TestType poly2({{0,0}, {12,0}, {12,3}, {0,3}});
    TestType poly3({{12,3}, {0,3}, {0,12}, {12,12}});
    
    auto getter = []([[maybe_unused]] unsigned int i){return 0;};
    auto modified = poly2.merge(poly2.get_head(), poly1, poly1.get_head());
    poly2.prune_LL(modified, getter);

    REQUIRE(poly2 == poly3);
}

TEMPLATE_TEST_CASE("Polygon prune two identical rectangles", "-", P1, P2, P3, P4, P5, P6){

    TestType poly1({{0,0}, {0,12}, {12,12}, {12,0}});
    TestType poly2({{0,0}, {12,0}, {12,12}, {0,12}});

    auto *node1 = poly1.get_head();
    auto *node2 = poly2.get_head();

    auto getter = []([[maybe_unused]] unsigned int i){return 0;};
    auto modified = poly2.merge(node2, poly1, node1);
    poly2.prune_LL(modified, getter);
    REQUIRE(poly2.get_head() == nullptr);  

}


TEMPLATE_TEST_CASE("Polygon translation and rotation", "-", P1, P2, P3, P4, P5, P6){

    TestType poly1({{0,0}, {0,12}, {12,12}, {12,0}});
    TestType poly2({{8,0}, {12,0}, {12,3}});
    poly1.connect_LL();
    poly2.connect_LL();

    auto *R1 = poly1.get_head()->next;
    auto *R2 = poly2.get_head();
    poly2.translate(R1->position-R2->position);
    REQUIRE(poly1.overlaps(poly2) == true);        
    poly2.rotate(R1->angle_end - R2->angle_start, R1->position);
    REQUIRE(poly1.overlaps(poly2) == false);
}


TEMPLATE_TEST_CASE("Polygon get_obtusest", "-", P1, P2, P3, P4, P5, P6){


    TestType poly1({{0,0}, {0,12}, {12,12}});
    TestType poly2({{0,0}, {-12,12}, {0,12}});
    TestType poly3({{0,0}, {0,12}, {3,0}});
    poly1.connect_LL();
    poly2.connect_LL();
    poly3.connect_LL();

    unsigned index1 = poly1.get_obtusest_node();
    unsigned index2 = poly2.get_obtusest_node();
    unsigned index3 = poly3.get_obtusest_node();

    const auto *node1 = poly1.vertex_from_index(index1);
    const auto *node2 = poly2.vertex_from_index(index2);
    const auto *node3 = poly3.vertex_from_index(index3);
    
    REQUIRE(node1 == poly1.get_head());
    REQUIRE(node2 == poly2.get_head());
    REQUIRE(node3 == poly3.get_head()->next);
}

TEMPLATE_TEST_CASE("Polygon get_leftest", "-", P1, P2, P3, P4, P5, P6){

    TestType poly1({{0,12}, {12,12}, {0,0}});
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

