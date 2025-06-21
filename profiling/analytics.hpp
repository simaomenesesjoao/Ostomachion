#pragma once
#include <chrono>
#include <string>
#include <iomanip>
#include <unordered_map>
#include <sstream>

class AnalyticsThread;

class TimingBranch {
private:
    unsigned long counter;
    std::chrono::high_resolution_clock::duration duration;
    std::chrono::high_resolution_clock::time_point t;

public:

    TimingBranch():
    counter{0},
    duration{0},
    builder{nullptr}{
        std::cout << "Error\n";
        exit(1);
    };

    TimingBranch(AnalyticsThread* pointer):
        counter{0},
        duration{0},
        builder{pointer}{};

    void join(const TimingBranch& other){
        counter += other.counter;
        duration += other.duration;
    }

    void start(){
        t = std::chrono::high_resolution_clock::now();
    };
    void end(){
        duration += std::chrono::high_resolution_clock::now() - t;
        counter++;
    };

    double get_duration() const {
        return duration.count();
    }
    unsigned long get_counter() const {
        return counter;
    }
    
    AnalyticsThread* builder;

};

class AnalyticsThread {
public:
    TimingBranch& branch(const std::string& name){
        auto it = branches.find(name);
        if (it != branches.end())
            return it->second;
        else {
            TimingBranch new_branch(this);
            branches.insert({name, new_branch});
            return branches[name];
        }
    }
    


    void summary() const {

        std::size_t max_str_size{0}, max_count{0};

        for(const auto& [name, obj]: branches){
            max_str_size = std::max(max_str_size, name.size());
            max_count = std::max(max_count, obj.get_counter());
        }


        std::ostringstream oss;
        for(const auto& [name, obj]: branches){
            //std::cout << name << " " << obj.get_counter() << " " << obj.get_duration() << "\n";
            oss << std::setw(max_str_size+2) << std::left << name;
            oss << std::setw(10) << std::left << obj.get_counter();
            oss << std::setw(15) << std::left << obj.get_duration() << "\n";
        }
        std::cout << oss.str();
    }

    void join(const AnalyticsThread& other) {
        for(const auto& [name, obj]: other.branches){
            auto it = branches.find(name);

            if (it == branches.end()){
                TimingBranch new_branch(this);
                branches.insert({name, new_branch});
            }
            branches[name].join(obj);
        }
    }


private:
    std::unordered_map<std::string, TimingBranch> branches;
};