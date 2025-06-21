#include "solver.cpp"
#include "settings.hpp"
#include <string>
#include <iostream>


int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv){
    
    std::string poly_type = "ContigPoly";  
    std::string puzzle_type = "Ostomini";
    std::string selector_type = "leftest";
    unsigned int num_threads = 1;
    unsigned int print_option;
    if (argc > 5) {
        puzzle_type = argv[1];
        poly_type = argv[2];
        selector_type = argv[3];
        num_threads = atoi(argv[4]);
        print_option = atoi(argv[5]);

    } else {
        std::cout << "Input not valid.\n";
        exit(1);
    }

    // Outer-level function: set input, receive number of combinations
    auto input = Input::get_premade_input(puzzle_type);
    if(not input){
        std::cout << "Input not valid\n";
        exit(1);
    }

    CalcSettings calc_settings("Double", poly_type, "Stack", "complete", 
        selector_type, Polygon::Transformations::RotFlip, num_threads, print_option);
    AnalyticsSettings analytics_settings;

    auto [states, analytics] = get_combinations(*input, calc_settings, analytics_settings);

}