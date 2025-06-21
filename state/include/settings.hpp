#pragma once
#include <optional>
#include <vector>
#include <string>
#include "polygon.cpp"


struct CalcSettings{
    std::string numeric_type, poly_type, container_type, overlapper, node_selector;
    Polygon::Transformations transformation;
    unsigned int num_threads;
    unsigned int print_options;
};

struct AnalyticsSettings{
};


namespace Input{
    struct PlaceHolder{};

    struct Input{
        Polygon::BarePoly frame;
        Polygon::Pool<Polygon::BarePoly> polygon_pool;
        PlaceHolder initializer;
        Input(const Polygon::BarePoly& frame, 
            const Polygon::Pool<Polygon::BarePoly>& polygon_pool, 
            const PlaceHolder& initializer): 
                frame{frame}, polygon_pool{polygon_pool}, initializer{initializer}{};
    };


    std::optional<Input> get_premade_input(const std::string& name){
        using P = Polygon::BarePoly;
        using R = Polygon::RestrictedPoly<P>;
        if(name == "Ostomini"){
            Polygon::BarePoly frame({{2,2}, {2, -2}, {-2, -2}, {-2, 2}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0}, {2,4}, {0,4}})).num_copies(2),
                R(P({{0,0}, {1,0}, {1,2}})).num_copies(2).transformation(Polygon::Transformations::RotFlip),
                R(P({{1,0}, {3,0}, {3,2}, {1,2}})),
                R(P({{1,2}, {3,2}, {2,4}}))
            });
            return Input(frame, polygon_pool, {});


        } else if(name == "OstominiRes"){
            Polygon::BarePoly frame({{2,2}, {2,-2}, {-2,-2}, {-2,2}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0}, {2,4}, {0,4}})).num_copies(2),
                R(P({{0,0}, {1,0}, {1,2}})).num_copies(2).transformation(Polygon::Transformations::RotFlip),
                R(P({{1,0}, {3,0}, {3,2}, {1,2}})),
                R(P({{1,2}, {3,2}, {2,4}})).restriction(P({{-2,-2}, {0,-2}, {0,0}}))
            });
            return Input(frame, polygon_pool, {});


        } else if(name == "Ostotiny"){
            Polygon::BarePoly frame({{2,2}, {2, -2}, {-2, -2}, {-2, 2}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,-2}, {2,2}, {0,2}})).num_copies(2),
                R(P({{-2,-2}, {0,-2}, {0,2}, {-2,2}})).transformation(Polygon::Transformations::R90)
            });
            return Input(frame, polygon_pool, {});

        } else if(name == "Ostomid") {
            Polygon::BarePoly frame({{6,6}, {6,-6}, {-6,-6}, {-6,6}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0},  {3,0},  {3,6 }})).restriction(P({{-6,-6}, {0,-6}, {0,0}})),
                R(P({{3,0},  {6,0},  {6,6 }, {4,8}, {3,6}})),
                R(P({{0,0},  {4,8},  {2,10}})),
                R(P({{0,0},  {2,10}, {0,12}})),
                R(P({{0,12}, {6,6},  {6,12}})).num_copies(2),
                R(P({{8,4},  {12,0}, {9,6 }})),
                R(P({{9,6},  {12,0}, {12,8}})),
                R(P({{6,6},  {8,4},  {9,6 },   {6,12}})),
                R(P({{9,6},  {12,8}, {12,12},  {6,12}}))
            });
            return Input(frame, polygon_pool, {});
        
        } else if(name == "Ostomid2") {
            Polygon::BarePoly frame({{6,6}, {6,-6}, {-6,-6}, {-6,6}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0},  {2,10}, {0,12}})),
                R(P({{0,0},  {4,8},  {2,10}})),
                R(P({{0,0},  {3,0},  {3,6 }})).restriction(P({{-6,-6}, {0,-6}, {0,0}})),
                R(P({{0,12}, {6,6},  {6,12}})),
                R(P({{3,0},  {6,0},  {6,6 }, {4,8}, {3,6}})),
                R(P({{0,12}, {6,6},  {6,12}})),
                R(P({{6,6},  {8,4},  {9,6 },   {6,12}})),
                R(P({{9,6},  {12,8}, {12,12},  {6,12}})),
                R(P({{8,4},  {12,0}, {9,6 }})),
                R(P({{9,6},  {12,0}, {12,8}}))
            });
            return Input(frame, polygon_pool, {});
        

        } else if(name == "Ostomed") {
            Polygon::BarePoly frame({{6,6}, {6,-6}, {-6,-6}, {-6,6}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0},  {3,0},  {3,6 }})).restriction(P({{-6,-6}, {0,-6}, {0,0}})),
                R(P({{3,0},  {6,0},  {6,6 }, {4,8}, {3,6}})),
                R(P({{0,0},  {4,8},  {2,10}})),
                R(P({{0,0},  {2,10}, {0,12}})),
                R(P({{0,12}, {4,8},  {6,12}})).num_copies(2),
                R(P({{4,8},  {6,6},  {6,12}})).num_copies(2),
                R(P({{8,4},  {12,0}, {9,6 }})),
                R(P({{9,6},  {12,0}, {12,8}})),
                R(P({{6,6},  {8,4},  {9,6 },   {6,12}})),
                R(P({{9,6},  {12,8}, {12,12},  {6,12}}))
            });
            return Input(frame, polygon_pool, {});
        
        } else if(name == "Ostomachion") {
            Polygon::BarePoly frame({{6,6}, {6,-6}, {-6,-6}, {-6,6}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0},  {3,0},  {2,4 }})).restriction(P({{-6,-6}, {0,-6}, {0,0}})),
                R(P({{3,0},  {3,6},  {2,4 }})),
                R(P({{3,0},  {6,0},  {6,6 }, {4,8}, {3,6}})),
                R(P({{0,0},  {4,8},  {2,10}})),
                R(P({{0,0},  {2,10}, {0,12}})),
                R(P({{0,12}, {4,8},  {6,12}})).num_copies(2),
                R(P({{4,8},  {6,6},  {6,12}})).num_copies(2),
                R(P({{8,4},  {12,0}, {9,6 }})),
                R(P({{9,6},  {12,0}, {12,6}})),
                R(P({{9,6},  {12,6}, {12,8}})),
                R(P({{6,6},  {8,4},  {9,6 },   {6,12}})),
                R(P({{9,6},  {12,8}, {12,12},  {6,12}}))
            });
            return Input(frame, polygon_pool, {});

        
        } else if(name == "OstomachionUnique") {
            Polygon::BarePoly frame({{6,6}, {6,-6}, {-6,-6}, {-6,6}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0},  {3,0},  {2,4 }})).restriction(P({{-6,-6}, {0,-6}, {0,0}})),
                R(P({{3,0},  {3,6},  {2,4 }})),
                R(P({{3,0},  {6,0},  {6,6 }, {4,8}, {3,6}})),
                R(P({{0,0},  {4,8},  {2,10}})),
                R(P({{0,0},  {2,10}, {0,12}})),
                R(P({{0,12}, {4,8},  {6,12}})),
                R(P({{0,12}, {4,8},  {6,12}})),
                R(P({{4,8},  {6,6},  {6,12}})),
                R(P({{4,8},  {6,6},  {6,12}})),
                R(P({{8,4},  {12,0}, {9,6 }})),
                R(P({{9,6},  {12,0}, {12,6}})),
                R(P({{9,6},  {12,6}, {12,8}})),
                R(P({{6,6},  {8,4},  {9,6 },   {6,12}})),
                R(P({{9,6},  {12,8}, {12,12},  {6,12}}))
            });
            return Input(frame, polygon_pool, {});
        
        } else if(name == "Rectangles") {
            Polygon::BarePoly frame({{-2,-2}, {-2,2}, {2,2}, {2,-2}});
            Polygon::Pool<Polygon::BarePoly> polygon_pool({
                R(P({{0,0}, {2,0}, {2,2}, {0,2}})).num_copies(3),
                R(P({{0,0}, {2,0}, {2,1}, {0,1}})).num_copies(2)
            });
            return Input(frame, polygon_pool, {});
        }
        
        
        return std::nullopt;
    }
}