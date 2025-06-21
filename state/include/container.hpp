// #pragma once
// #include <memory>
// #include <string>
// #include <iostream>
// #include <chrono>
// #include <condition_variable>
// #include <list>

// struct Analytics {
//     std::chrono::duration<double> lock_insert, duration_insert, lock_next, duration_next, duration;
//     std::chrono::duration<double> d1, d2, d3, d4;
//     int count1, count2;
//     std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> start;
//     unsigned long max_items, total_items, total_final, num_misses;
//     std::string info;
    
//     Analytics():
//         lock_insert{0}, duration_insert{0}, lock_next{0}, duration_next{0}, duration{0}, 
//         d1{0}, d2{0}, d3{0}, d4{0},
//         count1{0}, count2{0}, start{},
//         max_items{0}, total_items{0}, total_final{0}, num_misses{0}, info{""}{}


//     void clock_start(){
//         start = std::chrono::high_resolution_clock::now();
//     }

//     void clock_end(){
//         duration = std::chrono::high_resolution_clock::now() - start;
//     }

//     void print() const {
//         std::cout << "Analytics " << info << ":\n";
//         std::cout << "   lock next: " << lock_next.count() << "\n";
//         std::cout << "   duration next: " << duration_next.count() << "\n";
//         std::cout << "   lock insert: " << lock_insert.count() << "\n";
//         std::cout << "   duration insert: " << duration_insert.count() << "\n";
//         std::cout << "   duration: " << duration.count() << "\n";
//         std::cout << "   max_items: " << max_items << "\n";
//         std::cout << "   total_items: " << total_items << "\n";
//         std::cout << "   total_final: " << total_final << "\n";
//         std::cout << "   num_misses: " << num_misses << "\n";
//         std::cout << "   d1: " << d1.count() << "\n";
//         std::cout << "   d2: " << d2.count() << "\n";
//         std::cout << "   count1: " << count1 << "\n";
//         std::cout << "   count2: " << count2 << "\n";
//         std::cout << "\n";
//     }

//     void print_single_line() const {
//         // std::cout << std::format("Analytics {} - max: {:<5} total: {:<6} final: {:<6} duration: {:.2f}\n", 
//         //         info, max_items, total_items, total_final, duration.count());
//     }
// };

// namespace Container{

//     template <typename T>
//     class IContainer{

//     protected:   
//         std::mutex mtx;
//         std::condition_variable cv;
//         std::atomic<int> num_processes;

//     public:
//         using InnerType = T;

//         Analytics analytics;
//         virtual void insert(const std::vector<T>& states) = 0;
//         virtual void decrease_num_processes() = 0;
//         virtual std::optional<T> pop() = 0;
//         virtual std::vector<T> get_data() = 0;
//         virtual void clear() = 0;
//         virtual unsigned long size() const = 0;
//         virtual void print() const = 0;
//         virtual int get_num_processes() const {};

//         IContainer(): num_processes{1}, analytics{}{}

//         IContainer(IContainer&& other){}

//         Analytics get_analytics() const {
//             return analytics;
//         }
//     };



//     template <typename T>
//     class Stack : public IContainer<T> {
//     private:
//         std::list<T> container;
    
//     public:
//         static std::string name;
        
//         Stack(){}
//         Stack(Stack&& other):
//             container{std::move(other.container)}{}

//         void print() const override {
//             std::cout << "Printing container\n";
//             for(const auto& state: container){
//                 state->print();
//             }
//         }
    
//         void insert(const std::vector<T>& states) override {
//             // std::cout << "Entered container insert\n";
            
//             auto s1 = std::chrono::high_resolution_clock::now();
//             std::lock_guard<std::mutex> lock(this->mtx);
//             auto s2 = std::chrono::high_resolution_clock::now();
//             this->analytics.lock_insert += s2 - s1;
    
//             for(auto& state: states){            
    
//                 auto p1 = std::chrono::high_resolution_clock::now();
//                 container.push_back(state);
//                 auto p2 = std::chrono::high_resolution_clock::now();
//                 this->analytics.d2 += p2-p1;
//                 this->analytics.count2++;
//             }
    
//             this->num_processes--;
//             this->cv.notify_all();
    
//             this->analytics.max_items = std::max(this->analytics.max_items, container.size());
//             this->analytics.total_items += states.size();
    
//             auto s3 = std::chrono::high_resolution_clock::now();
//             this->analytics.duration_insert += s3 - s2;
    
//         }

//         void decrease_num_processes() override {
//             std::lock_guard<std::mutex> lock(this->mtx);
//             this->num_processes--;
//             this->cv.notify_all();
//         }

//         int get_num_processes() const override {
//             return this->num_processes;
//         }
    
//         unsigned long size() const override{
//             return container.size();
//         }
    
//         std::optional<T> pop() override {
//             auto s1 = std::chrono::high_resolution_clock::now();
//             std::unique_lock<std::mutex> lock(this->mtx);
            
//             while(true){
//                 if(container.size() == 0 and this->num_processes != 0)
//                     this->cv.wait(lock);
//                 else
//                     break;
//             }
            
//             auto s2 = std::chrono::high_resolution_clock::now();
//             this->analytics.lock_next += s2 - s1;
    
//             if(container.size() == 0 and this->num_processes == 0)
//                 return std::nullopt;
    
//             auto state = std::move(container.back());
//             container.pop_back();
    
//             this->num_processes++;
//             lock.unlock();
//             auto s3 = std::chrono::high_resolution_clock::now();
//             this->analytics.duration_next += s3 - s2;
//             return state;
//         }


    
//         std::vector<T> get_data()  {
//             return std::vector<T>(container.begin(), container.end());
//         }
    
//         void clear() override {
//             container.clear();
//         }


    
//     };
    
//     template <typename T>
//     std::string Stack<T>::name = "Stack";



//     template <typename T>
//     class Hash : public IContainer<T> {
//     private:
//         struct HashStruct {
//             size_t operator()(const T& x) const {
//                 return x.get_hash();
//             }
//         };
        
//         std::unordered_set<T, HashStruct> visited;
//         std::list<T> container;

//     public:
//         static std::string name;
        
//         Hash(){};
//         Hash(Hash&& other):
//             container{std::move(other.container)}{}

//         void insert(std::vector<T>& states) override {
//             auto s1 = std::chrono::high_resolution_clock::now();
//             std::lock_guard<std::mutex> lock(this->mtx);
//             auto s2 = std::chrono::high_resolution_clock::now();
//             this->analytics.lock_insert += s2 - s1;

//             int count = 0;
//             for(auto& state: states){
//                 auto [it, is_new_state] = visited.insert(state);
                
//                 if(is_new_state){
//                     count++;
//                     container.push_back(std::move(state));
//                 }
//             }

//             this->num_processes--;
//             this->cv.notify_all();

//             this->analytics.total_items += count;
//             this->analytics.max_items = std::max(this->analytics.max_items, size());

//             auto s3 = std::chrono::high_resolution_clock::now();
//             this->analytics.duration_insert += s3 - s2;
//         }

//         unsigned long size() const {
//             return container.size() + visited.size();
//         }

//         std::optional<T> get_next() override {
//             auto s1 = std::chrono::high_resolution_clock::now();
//             std::unique_lock<std::mutex> lock(this->mtx);
            
//             while(true){
//                 if(container.size() == 0 and this->num_processes != 0)
//                     this->cv.wait(lock);
//                 else
//                     break;
//             }
//             auto s2 = std::chrono::high_resolution_clock::now();
//             this->analytics.lock_next += s2 - s1;
            
//             if(container.size() == 0 and this->num_processes == 0)
//                 return std::nullopt;

//             auto it = container.begin();
//             auto state = std::move(*it);
//             container.erase(it);

//             this->num_processes++;
//             lock.unlock();
//             auto s3 = std::chrono::high_resolution_clock::now();
//             this->analytics.duration_next += s3 - s2;
//             return state;
//         }

//         std::vector<T> get_data() override{
//             return std::vector<T>(container.begin(), container.end());
//         }

//         void clear() override {
//             container.clear();
//         }

//         int get_num_processes() const override {
//             return this->num_processes;
//         }

//     };

//     template <typename T>
//     std::string Hash<T>::name = "Hash";






//     template <typename T>
//     class HashLevel : public IContainer<T> {
//     private:
//         struct HashStruct {
//             size_t operator()(const T& x) const {
//                 return x->get_hash();
//             }
//         };
//         struct EqualStruct {
//             bool operator()(const T& x, const T& y) const {
//                 return *x == *y;
//             }
//         };

//         std::unordered_set<T, HashStruct, EqualStruct> container_in, container_out;
        

//     public:
//         static std::string name;
        
//         HashLevel(){};

//         HashLevel(HashLevel&& other):
//             container_in{std::move(other.container_in)},
//             container_out{std::move(other.container_out)}{}

//         void insert(std::vector<T>& states) override {
//             auto s1 = std::chrono::high_resolution_clock::now();
//             std::lock_guard<std::mutex> lock(this->mtx);
//             auto s2 = std::chrono::high_resolution_clock::now();
//             this->analytics.lock_insert += s2 - s1;

//             int count = 0;
//             for(auto& state: states){
//                 auto [it, success] = container_out.insert(state);
//                 if(success){
//                     count++;
//                 }
//             }

//             this->num_processes--;
//             this->cv.notify_all();

//             this->analytics.total_items += count;
//             this->analytics.max_items = std::max(this->analytics.max_items, container_in.size() + container_out.size());

//             auto s3 = std::chrono::high_resolution_clock::now();
//             this->analytics.duration_insert += s3 - s2;
//         }

//         unsigned long size() const {
//             return container_in.size() + container_out.size();
//         }

//         std::optional<T> get_next() override {
//             auto s1 = std::chrono::high_resolution_clock::now();
//             std::unique_lock<std::mutex> lock(this->mtx);


//             while(true){
//                 if(container_in.size() == 0 and container_out.size() == 0 and this->num_processes != 0)
//                     this->cv.wait(lock);
//                 else
//                     break;
//             }

//             auto s2 = std::chrono::high_resolution_clock::now();
//             this->analytics.lock_next += s2 - s1;
            
            
//             if(container_in.size() == 0 and container_out.size() == 0 and this->num_processes == 0)
//                 return std::nullopt;


//             if(container_in.size() == 0){
//                 container_in = std::move(container_out);
//                 container_out.clear();
//             }

//             auto it = container_in.begin();
//             auto state = std::move(*it);
//             container_in.erase(it);
            
//             auto s3 = std::chrono::high_resolution_clock::now();
//             this->num_processes++;
//             lock.unlock();
//             this->analytics.duration_next += s3 - s2;

//             return state;
//         }

//         std::vector<T> get_data() override{
//             return std::vector<T>(container_out.begin(), container_out.end()); // SIMAO: provavelmente está errado. uso in ou out?
            
//         }

//         void clear() override {
//             container_in.clear();
//             container_out.clear();
//         }

//         int get_num_processes() const override {
//             return this->num_processes;
//         }

//     };

//     template <typename T>
//     std::string HashLevel<T>::name = "HashL";


    
//     template <typename T>
//     std::unique_ptr<IContainer<T>> factory(const std::string& name){
//         if(name == "Stack"){
//             return std::make_unique<Stack<T>>();
//         // } else if(name == "Hash"){
//         //     return std::make_unique<Hash<T>>();
//         // } else if(name == "HashLevel"){
//         //     return std::make_unique<HashLevel<T>>();
//         } else {
//             return nullptr;
//         }
//     }
// }