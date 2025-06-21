#pragma once
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include "analytics.hpp"

struct Analytics {
    std::chrono::duration<double> get_prog_lock, get_prog_wait, allocate_lock, send_prog_lock, send_final_lock, release_lock;
    std::chrono::duration<double> get_prog_dur, allocate_dur, send_prog_dur, send_final_dur, release_dur, init_dur;
    std::chrono::duration<double> get_prog_inner;
    unsigned int get_prog_count, allocate_count, send_prog_count, send_final_count, release_count, init_count;
    
    unsigned long max_items, total_items, total_final, num_misses;
    std::string info;
    
    Analytics():
        get_prog_lock{0}, get_prog_wait{0}, allocate_lock{0}, send_prog_lock{0}, send_final_lock{0}, release_lock{0},
        get_prog_dur{0}, allocate_dur{0}, send_prog_dur{0}, send_final_dur{0}, release_dur{0}, init_dur{0},
        get_prog_inner{0},
        get_prog_count{0}, allocate_count{0}, send_prog_count{0}, send_final_count{0}, release_count{0}, init_count{0},
        max_items{0}, total_items{0}, total_final{0}, num_misses{0}, info{""}{}


    // void clock_start(){
    //     start = std::chrono::high_resolution_clock::now();
    // }

    // void clock_end(){
    //     duration = std::chrono::high_resolution_clock::now() - start;
    // }

    void print() const {
        std::cout << "Allocator analytics " << info << ":\n";
        std::cout << "   get_prog:   " << get_prog_count << " " 
            << get_prog_dur.count() << " " << get_prog_lock.count() << " " << get_prog_wait.count() << " "
            << get_prog_inner.count() << "\n";
        
        std::cout << "   allocate:   " << allocate_count << " " 
            << allocate_dur.count() << " " << allocate_lock.count() << "\n";

        std::cout << "   send_prog:  " << send_prog_count << " " 
            << send_prog_dur.count() << " " << send_prog_lock.count() << "\n";
            
        std::cout << "   send_final: " << send_final_count << " "
            << send_final_dur.count() << " " << send_final_lock.count() << "\n";

        std::cout << "   release:    " << release_count << " "
            << release_dur.count() << " " << release_lock.count() << "\n";

        std::cout << "   init:       " << init_count << " " << init_dur.count() << "\n";
        std::cout << "\n";
    }

    void print_single_line() const {
        // std::cout << std::format("Analytics {} - max: {:<5} total: {:<6} final: {:<6} duration: {:.2f}\n", 
        //         info, max_items, total_items, total_final, duration.count());
    }
};

namespace Allocator {

    template <typename T>
    class IAllocator{

    protected:   
        std::mutex mtx;
        std::condition_variable cv;
        Analytics analytics;
        // AnalyticsThread analytics;
        

    public:
        using InnerType = T;

        virtual void release(const std::vector<T>&) = 0;
        virtual void send_final(const std::vector<T>&) = 0;
        virtual void send_prog(const std::vector<T>&) = 0;
        virtual std::vector<T> get_prog(unsigned int) = 0;
        virtual std::vector<T> allocate(unsigned int) = 0;
        virtual T allocate() = 0;

        virtual std::vector<T> get_prog_vec() const = 0;
        virtual std::vector<T> get_finalized_vec() const = 0;
        virtual std::vector<T> get_available_vec() const = 0;
        virtual Analytics get_analytics() const = 0;

        virtual void init(unsigned int, std::function<T(Allocator::IAllocator<T>*)>) = 0;
        virtual unsigned long size() const = 0;
        virtual void print() const = 0;
        
        virtual ~IAllocator() = default;

    };



    // SIMAO: substituir tudo por unique ptrs?
    template <typename T>
    class Stack : public IAllocator<T> {
    private:
    
    public:
        unsigned int _max_size, num_active;
        std::deque<T> available, prog, finalized;
        static std::string name;

        void init(unsigned int size, std::function<T(Allocator::IAllocator<T>*)> factory) override {
            auto s1 = std::chrono::high_resolution_clock::now();
            // Preallocate all the memory
            _max_size = size;
            num_active = 0;
            
            // available.reserve(size);
            for(unsigned int i = 0; i < size; i++)
                available.push_front(factory(this));

            // prog.reserve(size);
            // finalized.reserve(size);
            auto s2 = std::chrono::high_resolution_clock::now();
            this->analytics.init_dur += s2 - s1;
            this->analytics.init_count++;
        }

        void release(const std::vector<T>& states) override {

            // std::cout << "inside release\n";
            // print();

            auto s1 = std::chrono::high_resolution_clock::now();
            std::lock_guard<std::mutex> lock(this->mtx);
            auto s2 = std::chrono::high_resolution_clock::now();
            this->analytics.release_lock += s2 - s1;

            for(const auto& state: states){
                available.push_front(state);
                num_active--;
            }
            this->cv.notify_all();

            s1 = std::chrono::high_resolution_clock::now();
            this->analytics.release_dur += s1 - s2;
            this->analytics.release_count++;
        }

        void send_final(const std::vector<T>& states) override {
            auto s1 = std::chrono::high_resolution_clock::now();
            std::lock_guard<std::mutex> lock(this->mtx);
            auto s2 = std::chrono::high_resolution_clock::now();
            this->analytics.send_final_lock += s2 - s1;


            for(const auto& state: states){
                finalized.push_front(state);
                num_active--;
            }
            this->cv.notify_all();

            s1 = std::chrono::high_resolution_clock::now();
            this->analytics.send_final_dur += s1 - s2;
            this->analytics.send_final_count++;
        }

        void send_prog(const std::vector<T>& states) override {
            auto s1 = std::chrono::high_resolution_clock::now();
            std::lock_guard<std::mutex> lock(this->mtx);
            auto s2 = std::chrono::high_resolution_clock::now();
            this->analytics.send_prog_lock += s2 - s1;

            for(const auto& state: states){
                prog.push_back(state);
                num_active--;
            }
            this->cv.notify_all();

            s1 = std::chrono::high_resolution_clock::now();
            this->analytics.send_prog_dur += s1 - s2;
            this->analytics.send_prog_count++;
        }

        std::vector<T> allocate(unsigned int amount) override {
            // std::cout << "inside allocate\n";
            // print();
            auto s1 = std::chrono::high_resolution_clock::now();
            std::lock_guard<std::mutex> lock(this->mtx);
            auto s2 = std::chrono::high_resolution_clock::now();
            this->analytics.allocate_lock += s2 - s1;

            std::vector<T> allocated_states;
            allocated_states.reserve(amount);

            for(unsigned int i = 0; i < amount; i++){                
                if (available.empty()) 
                    throw std::runtime_error("No available states to allocate");

                T state = available.back();
                available.pop_back();
                allocated_states.push_back(state);
                num_active++;
            }
            this->cv.notify_all();

            s1 = std::chrono::high_resolution_clock::now();
            this->analytics.allocate_dur += s1 - s2;
            this->analytics.allocate_count++;

            return allocated_states;
        }

        T allocate() override {
            return allocate(1).at(0);
        }


        std::vector<T> get_prog(unsigned int n_threads) override {
            // std::cout << "inside get_prog\n";
            // print();

            auto s1 = std::chrono::high_resolution_clock::now();
            std::unique_lock<std::mutex> lock(this->mtx);
            auto s2 = std::chrono::high_resolution_clock::now();
            this->analytics.get_prog_lock += s2 - s1;

            auto s3 = std::chrono::high_resolution_clock::now();
            this->cv.wait(lock, [this]{ return this->num_active == 0 or this->prog.size() > 0;});
            auto s4 = std::chrono::high_resolution_clock::now();
            this->analytics.get_prog_wait += s4 - s3;

            if(num_active == 0 and prog.size() == 0){
                lock.unlock();
                // std::cout << "left get_prog with nullopt\n";
                return {};
            }
            
            unsigned int n_states = std::max((unsigned int)1, (unsigned int)prog.size()/(unsigned int)n_threads);
            n_states = std::min(n_states, (unsigned int)10);

            auto s5 = std::chrono::high_resolution_clock::now();

            std::vector<T> states;
            states.reserve(n_states);
            for(unsigned int i = 0; i < n_states; i++){
                states.push_back(prog.back());
                prog.pop_back();
                num_active++;
            }
            
            this->cv.notify_all();
            lock.unlock();

            auto s6 = std::chrono::high_resolution_clock::now();
            this->analytics.get_prog_inner += s6 - s5;
            // std::cout << "left get_prog\n";

            s1 = std::chrono::high_resolution_clock::now();
            this->analytics.get_prog_dur += s1 - s4;
            this->analytics.get_prog_count++;
            return states;

        }
        
    
        unsigned long size() const override{
            return this->num_active;
        }


        std::vector<T> get_prog_vec() const override {
            return std::vector<T>(prog.begin(), prog.end());
        }

        std::vector<T> get_available_vec() const override {
            return std::vector<T>(available.begin(), available.end());
        }

        std::vector<T> get_finalized_vec() const override {
            return std::vector<T>(finalized.begin(), finalized.end());
        }

        Analytics get_analytics() const override {
            return this->analytics;
        }

        void print() const override {
            std::cout << "active: " << num_active << " prog: " << prog.size() << " av: " << available.size() << " final: " << finalized.size() << "\n";
        }
    
    };
    
    template <typename T>
    std::string Stack<T>::name = "Stack";


    
    template <typename T>
    std::unique_ptr<IAllocator<T>> factory(const std::string& name){
        if(name == "Stack"){
            return std::make_unique<Stack<T>>();
        // } else if(name == "Hash"){
        //     return std::make_unique<Hash<T>>();
        // } else if(name == "HashLevel"){
        //     return std::make_unique<HashLevel<T>>();
        } else {
            return nullptr;
        }
    }

}