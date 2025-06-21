// #pragma once
#include <string>
#include <gmpxx.h>
#include <iostream>

#include "polygon.cpp"
#include "state.hpp"
#include "settings.hpp"

int main(int argc, char** argv){
    
    std::string puzzle_type = argv[1];
    auto input = Input::get_premade_input(puzzle_type);
    if(not input){
        std::cout << "Puzzle type inpue not valid\n";
        exit(1);
    }


    std::string selector_type = argv[2];

    
    std::string respect_restrictions = argv[3];
    bool bool_respect_restrictions = false;
    if(respect_restrictions == "IgnoreRestrictions"){
        bool_respect_restrictions = false;
    } else if(respect_restrictions == "RespectRestrictions"){
        bool_respect_restrictions = true;
    } else {
        std::cout << "Restriction input not valid\n";
        exit(1);
    }



    std::vector<std::tuple<unsigned short, unsigned short, unsigned short>> history;    
    for(int i = 4; i < argc; i += 3){
        int vertex_index = atoi(argv[i]);
        int polygon_index = atoi(argv[i+1]);
        int variation_index = atoi(argv[i+2]);
        history.push_back({vertex_index, polygon_index, variation_index});
    }

    std::string poly_type = "ContigPoly";
    CalcSettings calc_settings("Double", poly_type, 
        "Stack", "complete", selector_type, Polygon::Transformations::RotFlip, 1, 0);


    auto state_factory = State::factory(poly_type, input->frame, input->polygon_pool, calc_settings);
    auto allocator = Allocator::factory<std::shared_ptr<State::IState>>(calc_settings.container_type);
        
    unsigned int memory_size = 1000;
    allocator->init(memory_size, state_factory);
    auto state = allocator->allocate();

    
    state->set_history(history);
    state->activate_history(bool_respect_restrictions);
    state->print_output();

    return 0;
}