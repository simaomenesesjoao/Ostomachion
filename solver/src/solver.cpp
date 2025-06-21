#pragma once
#include <thread>
#include <vector>
// #include <gmpxx.h>
#include <fstream>

//#include "polygon.cpp"
#include "state.hpp"
#include "settings.hpp"
#include "memory_pool.hpp"
#include "analytics.hpp"

std::vector<std::shared_ptr<State::IState>> find_uniques_brute(
                const std::vector<std::shared_ptr<State::IState>>& states, 
                Polygon::Transformations transformation){

    unsigned int N = states.size();
    std::vector<std::shared_ptr<State::IState>> uniques;
    for(unsigned int i = 0; i < N; i++){
        bool found_identical = false;
        for(unsigned int j = i+1; j < N; j++){
            if(states.at(i)->equals(states.at(j), transformation)){
                found_identical = true;
                break;
            }
        }
        if(!found_identical)
            uniques.push_back(states.at(i));
    }
    return uniques;
}

std::pair<std::vector<std::shared_ptr<State::IState>>, Analytics> get_combinations(
    const Input::Input& input,
    const CalcSettings& settings,
    [[maybe_unused]] const AnalyticsSettings& analytics_settings){ // SIMAO: implementar

    auto state_factory = State::factory(settings.poly_type, input.frame, input.polygon_pool, settings);
    auto allocator = Allocator::factory<std::shared_ptr<State::IState>>(settings.container_type);

    unsigned int memory_size = 10000;
    allocator->init(memory_size, state_factory);
    auto first_state = allocator->allocate();
    first_state->set_initial_state();
    allocator->send_prog({first_state});

    std::vector<std::thread> threads;

    
    for(unsigned int i=0; i<settings.num_threads; i++){
        
        threads.emplace_back([&allocator](){

            std::vector<std::shared_ptr<State::IState>> from_prog, finalized, to_prog, release;
            
            from_prog.reserve(100);
            finalized.reserve(100);
            to_prog.reserve(100);
            release.reserve(100);
            
            while(true){
                // allocator->print();

                from_prog = std::move(allocator->get_prog(8));
                finalized.clear();
                to_prog.clear();
                release.clear();

                
                if(from_prog.empty())
                    break;
                
                for(const auto& state: from_prog){
                    if(state->finalized()){
                        finalized.push_back(state);
                    } else {
                        auto next_states = state->find_next_states();
                        
                        for(const auto& st: next_states){
                            to_prog.push_back(st);
                        }
                        release.push_back(state);
                    }
                }

                allocator->send_final(finalized);
                allocator->send_prog(to_prog);
                allocator->release(release);
                
            }
        });
    }


    

    for(auto& thread: threads)
        thread.join();

    if(settings.print_options == 2){
        allocator->print();
    }
    std::vector<std::shared_ptr<State::IState>> finalized = allocator->get_finalized_vec();


    if(settings.print_options == 2){
        AnalyticsThread analytics;
        for(const auto& state: finalized)
            analytics.join(state->get_analytics());

        analytics.summary();

        allocator->get_analytics().print();
    }

    

    unsigned int total_states = finalized.size();

    
    for(auto& state: finalized){
        state->activate_history(true);
    }

    auto uniques = find_uniques_brute(finalized, settings.transformation);
    

    if(settings.print_options == 2){
        std::cout << "total states: " << total_states << "\n";
        std::cout << "total unique: " << uniques.size() << "\n";
    }

    if(settings.print_options == 1){
        for(const auto& state: uniques){
            state->print_output();
        }
    }


    if(settings.print_options == 0){
        for(const auto& state: uniques){
            state->print_history();
        }
    }

    return {};
}