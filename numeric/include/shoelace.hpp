#pragma once
#include <utility>
#include <vector>

template <typename N>
N shoelace_area(std::vector<std::pair<N,N>> const& points){
    // If the polygon is oriented clockwise, the area is negative

    unsigned size = points.size();
    N area{0};

    for(unsigned i=0; i<size; i++){
        N xi  = points.at(i).first;
        N yi  = points.at(i).second;
        N xip = points.at((i+1)%size).first;
        N yip = points.at((i+1)%size).second;
        area = area + (xi - xip)*(yi + yip);
    }

    return area/2;
}


// template <typename N>
// N shoelace_area(std::vector<std::pair<N,N>> const& points){
//     // If the polygon is oriented clockwise, the area is negative

//     unsigned size = points.size();
//     N area{0};

//     for(unsigned i=0; i<size; i++){
//         N xi  = points.at(i).first;
//         N yi  = points.at(i).second;
//         N xip = points.at((i+1)%size).first;
//         N yip = points.at((i+1)%size).second;
//         area = area + (xi - xip)*(yi + yip);
//     }

//     return area/2;
// }






// template <int ...Ints>
// Number<Ints...> shoelace_area(std::vector<Point<Ints...>> const& points){
//     // If the polygon is oriented clockwise, the area is negative

//     using Num = Number<Ints...>;
//     unsigned N = points.size();
//     Number<Ints...> area{0};

//     for(unsigned i=0; i<N; i++){
//         Num xi  = points.at(i).get_x();
//         Num yi  = points.at(i).get_y();
//         Num xip = points.at((i+1)%N).get_x();
//         Num yip = points.at((i+1)%N).get_y();
//         area = area + (xi - xip)*(yi + yip);
//     }

//     return area*Fraction<int>{1,2};
// }

