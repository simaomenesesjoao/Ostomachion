#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include "BarePoly.cpp"
#include "LLPoly.cpp"
#include "ContigPoly.cpp"
#include "analytics.hpp"

namespace Polygon{


    template <typename Poly>
    Poly identity(const Poly& poly){
        return poly;
    };

    template <typename Poly>
    Poly rotate_90(const Poly& poly){
        Poly poly2 = poly;
        poly2.rotate({0,1},{0,0});
        return poly2;
    };

    template <typename Poly>
    Poly rotate_270(const Poly& poly){
        Poly poly2 = poly;
        poly2.rotate({0,-1},{0,0});
        return poly2;
    };

    template <typename Poly>
    Poly rotate_180(const Poly& poly){
        Poly poly2 = poly;
        poly2.rotate({-1,0},{0,0});
        return poly2;
    };

    template <typename Poly>
    Poly flip_x(const Poly& poly){
        Poly poly2 = poly;
        poly2.flip_x();
        return poly2;
    };

    template <typename Poly>
    Poly flip_y(const Poly& poly){
        Poly poly2 = poly;
        poly2.flip_y();
        return poly2;
    };

    template <typename Poly>
    Poly flip_D(const Poly& poly){
        return rotate_90(flip_x(poly));
    };

    template <typename Poly>
    Poly flip_AD(const Poly& poly){
        return rotate_270(flip_x(poly));
    };


    enum class Transformations {Id, Rot, Flip, RotFlip, R90};

    template <typename Poly>
    class RestrictedPoly {
        void apply_transformation(){
            // Applies all transformations to one of the polygons
            Poly poly = variations_.front();
            variations_.clear();

            switch(transformation_){
                case Transformations::Id:
                    break;

                case Transformations::Rot:

                    for(unsigned int i=0; i<poly.size(); i++){
                        variations_.push_back(poly);
                        poly.advance_head();                   
                    }
                    
                    break;

                case Transformations::Flip:
                    variations_.push_back(poly);
                    poly.flip_x();
                    variations_.push_back(poly);
                    break;

                case Transformations::RotFlip:

                    for(unsigned int i=0; i<poly.size(); i++){
                        variations_.push_back(poly);
                        poly.advance_head();                    
                    }
                    poly.flip_x();
                    for(unsigned int i=0; i<poly.size(); i++){
                        variations_.push_back(poly);
                        poly.advance_head();                      
                    }

                    break;

                case Transformations::R90:

                    for(unsigned int i=0; i<2; i++){
                        variations_.push_back(poly);
                        poly.advance_head();                    
                    }

                    break;

                
            }
        }

        // void sort_variations(){


        //     auto larger = [](Angle)

        // }

    public:
        RestrictedPoly(const std::vector<Poly>& variations): 
            variations_{variations}, num_copies_{1}, transformation_{Transformations::RotFlip}{
                apply_transformation();
            };

        RestrictedPoly(const Poly& poly): RestrictedPoly(std::vector<Poly>{poly}){};

        RestrictedPoly& num_copies(unsigned int n){
            num_copies_ = n;
            return *this;
        }

        RestrictedPoly& restriction(const Poly& poly){
            restriction_ = poly;
            return *this;
        }

        RestrictedPoly& transformation(Transformations t){
            transformation_ = t;
            apply_transformation();
            return *this;
        }

        Transformations get_transformation() const { return transformation_; }
        std::vector<Poly>& get_variations() { return variations_; }
        const std::vector<Poly>& get_variations() const { return variations_; }
        const Poly& get_restriction() const { return restriction_; }
        unsigned int get_amount() const { return num_copies_; }

        void print() const {
            std::cout << "Printing RestrictedPoly:\n";
            for(const auto& poly: variations_){
                poly.print();
            }
        }

    private:
        std::vector<Poly> variations_;
        unsigned int num_copies_;
        Poly restriction_;
        Transformations transformation_;
    };

    template <typename Poly>
    class Pool {
    public:       

        Pool(): pool{}, size{0}{}

        Pool(const std::vector<RestrictedPoly<Poly>>& init_pool): size{0}{
            for(auto& poly: init_pool){
                add_poly(poly);
            }
        }



        template <typename OtherPoly>
        Polygon::Pool<OtherPoly> convert_sort() const {
            // Converts a BarePoly into templated type OtherPoly
            
            Polygon::Pool<OtherPoly> converted_pool;
            for(const auto& restricted_poly: pool){

                std::vector<OtherPoly> variations;
                for(const auto& var: restricted_poly.get_variations()){
                    variations.push_back(OtherPoly(var));
                }

                // auto opp = [&lesser](const OtherPoly& poly1, const OtherPoly& poly2){
                //     return lesser(poly2, poly1);
                // };


                OtherPoly restriction(restricted_poly.get_restriction());
                unsigned int copies = restricted_poly.get_amount();
                Transformations t = restricted_poly.get_transformation();

                
                RestrictedPoly<OtherPoly> converted_rpoly(variations);
                converted_rpoly.num_copies(copies).restriction(restriction).transformation(t);

                auto& vars = converted_rpoly.get_variations();



                auto lesser = [](const OtherPoly& poly1, const OtherPoly& poly2){
                    auto a = poly1.get_head()->angle_opening;
                    auto b = poly2.get_head()->angle_opening;

                    bool semiplane_a = a.get_sin() > 0 ? 0 : 1;
                    bool semiplane_b = b.get_sin() > 0 ? 0 : 1;

                    if(semiplane_a != semiplane_b)
                        return semiplane_a < semiplane_b; // angle a lesser than b
                    else if(semiplane_a == 0)
                        return !(a.get_cos() < b.get_cos());
                    else
                        return !(a.get_cos() > b.get_cos());

                };


                // std::cout << "variations before sort\n";
                // for(auto const& v: variations)
                //     v.print();
                std::sort(vars.begin(), vars.end(), lesser);
                // std::cout << "variations after sort\n";
                // for(auto const& v: variations)
                //     v.print();

                converted_pool.add_poly(converted_rpoly);


            }
            return converted_pool;
        }

        void add_poly(const RestrictedPoly<Poly>& poly){
            pool.push_back(poly);
            size += poly.get_amount();
        }

        unsigned int num_distinct_polys() const {
            return pool.size();
        }

        unsigned int num_total_polys() const {
            return size;
        }


        void print() const {
            std::cout << "Printing pool. Size " << size << ":\n";
            for(const auto& poly: pool){
                poly.print();
            }
        }

    public:
        std::vector<RestrictedPoly<Poly>> pool;
    private:
        unsigned int size;
    };

        
    template <typename Poly>
    std::function<bool(const Poly&, const Poly&, TimingBranch&)> overlapper_factory(const std::string& name){
        
        if(name == "edge"){
            return [](const Poly& poly1, const Poly& poly2, TimingBranch& timing){ 
                timing.start();
                bool intersects = poly1.edge_edge_intersection(poly2);
                timing.end();
                return intersects;
            };
        } else if(name == "complete"){
            return [](const Poly& poly1, const Poly& poly2, TimingBranch& timing){ 
                timing.start();
                bool overlaps = poly1.overlaps(poly2);
                timing.end();
                return overlaps;
            };
        } else if(name == "no-op"){
            return [](const Poly&, const Poly&, [[maybe_unused]] TimingBranch& timing){ 
                return true;
            };
        } else {
            assert(false); // SIMAO: substituir por exception
        }
    }


    template <typename Poly>
    std::function<std::vector<unsigned int>(const Poly&)> selector_factory(const std::string& name){
        
        if(name == "farthest"){
            return [](const Poly& poly){ 
                return std::vector<unsigned int>{poly.get_farthest_node(0,0)};
            };
        } else if(name == "all") {
            return [](const Poly& poly){ 
                return poly.get_all_nodes();
            };
        } else if(name == "obtusest") {
            return [](const Poly& poly){ 
                return std::vector<unsigned int>{poly.get_obtusest_node()};
            };
        } else if(name == "leftest") {
            return [](const Poly& poly){ 
                return std::vector<unsigned int>{poly.get_leftest_node()};
            };
        } else {
            assert(false); // SIMAO: substituir por exception
        }
    }


}