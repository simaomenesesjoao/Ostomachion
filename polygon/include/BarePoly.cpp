#pragma once
#include <vector>
#include <iostream>

namespace Polygon{


    class BarePoly {
    public:
        BarePoly(const std::vector<std::vector<int>>& points):point_list{points}{};
        BarePoly(){};

        void flip_x() {

            // SIMAO: esta função será mesmo necessária?
            // std::cout << "Entering flip_x\n";

            std::vector<std::vector<int>> new_point_list;
            for(auto it = point_list.rbegin(); it != point_list.rend(); ++it){
                
                int x = it->at(0);
                int y = it->at(1);
                // std::cout << "(x,y)=(" << x << "," << y << ") ";
                new_point_list.push_back({-x, y});
            }
            // std::cout << "\n";
            // std::cout << "outcome:\n";
            // for(auto p: new_point_list)
            //     std::cout << "(x,y)=(" << p.at(0) << "," << p.at(1) << ") ";
            point_list = new_point_list;

        }

        void print() const {
            std::cout << "BarePoly: ";
            for(const auto& point: point_list){
                std::cout << "(" << point.at(0) << "," << point.at(1) << ") ";
            }
            std::cout << "\n";
        }

        void advance_head(){
            std::vector<std::vector<int>> new_point_list;

            for(unsigned int i = 1; i<point_list.size(); i++){
                new_point_list.push_back(point_list.at(i));
            }

            new_point_list.push_back(point_list.at(0));
            point_list = new_point_list;

        }

        unsigned int size() const {
            return point_list.size();
        }

        std::vector<std::vector<int>> point_list;
    };

}