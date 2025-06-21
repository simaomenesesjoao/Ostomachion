#pragma once
#include <memory>
#include <vector>
#include "vertex.cpp"
#include "polygon.cpp"
#include "settings.hpp"
#include "customFloat.cpp"
#include "analytics.hpp"
#include "memory_pool.hpp"

namespace State{

    class IState {
    public:
        virtual std::vector<std::shared_ptr<IState>> find_next_states() = 0;
        virtual void set_initial_state() = 0;
        virtual bool finalized() const = 0;
        virtual unsigned int size() const = 0;
        virtual void print() const = 0;
        virtual void print_output() const = 0;
        virtual void print_history() const = 0;
        virtual AnalyticsThread& get_analytics() = 0;
        virtual bool equals(std::shared_ptr<IState>, Polygon::Transformations) const = 0;
        virtual void activate_history(bool) = 0;
        virtual void set_history(const std::vector<std::tuple<unsigned short, unsigned short, unsigned short>> &) = 0;
        virtual ~IState();

    };
    

    IState::~IState(){};

    template <typename Poly>
    class State: public IState {
    public:

        State(const Polygon::BarePoly& puzzle_frame,
              const Polygon::Pool<Polygon::BarePoly>& poly_pool, 
              const CalcSettings&, 
              Allocator::IAllocator<std::shared_ptr<IState>>*);

        void set_initial_state() override;
        void create_new_state(const State&, const Poly&, unsigned short, unsigned short, unsigned short);
        std::vector<std::vector<Poly>>& get_used_polys();

        void fast_copy(Poly&, const Poly&, const Poly::VertexType&);
        bool all_polys_match(const std::vector<Poly>& polyrow1, 
                             const std::vector<Poly>& polyrow2, 
                             std::function<Poly(const Poly&)> f) const;
        bool equals_derived(const State& other, Polygon::Transformations transf) const;
        bool equals(std::shared_ptr<IState> other, Polygon::Transformations transf) const override;
        bool equals_under_transform(const State& other, std::function<Poly(const Poly&)> f) const;

        std::vector<std::pair<unsigned int, Polygon::RestrictedPoly<Poly>*>> get_remaining_poly_pool();
        std::vector<std::shared_ptr<IState>> find_next_states() override;
        void iterate(const std::vector<std::tuple<unsigned short, unsigned short, unsigned short>>&, bool);
        void activate_history(bool) override;
        void set_history(const std::vector<std::tuple<unsigned short, unsigned short, unsigned short>> &) override;

        bool is_valid() const;
        bool finalized() const override;
        unsigned int size() const override;
        void print() const override;
        void print_history() const override;
        void print_output() const override;
        AnalyticsThread& get_analytics() override;

        void get_data_from(const State& other);
        
    private:
        const unsigned int _preallocate;
        unsigned int _size;
        Poly _frame;
        Poly _poly_temp;
        const Poly _default_frame;
        Polygon::Pool<Poly> _poly_pool;
        const Polygon::Pool<Poly> _const_poly_pool;
        std::vector<std::vector<Poly>> _current_polys;
        std::vector<unsigned short> _num_polys;
        std::vector<std::tuple<unsigned short, unsigned short, unsigned short>> _history;
        const std::function<bool(const Poly&, const Poly&, TimingBranch&)> _overlapper;
        const std::function<std::vector<unsigned int>(const Poly&)> _selector;
        std::function<unsigned int(unsigned int)> _getter;
        AnalyticsThread _analytics;
        Allocator::IAllocator<std::shared_ptr<IState>>* const _allocator;
    };

    template <typename Poly>
    void State<Poly>::get_data_from(const State& other){
        _size = other._size;
        _frame = other._frame;
        _poly_temp = other._poly_temp;
        _current_polys = other._current_polys;
        _num_polys = other._num_polys;
        _history = other._history;
        _getter = other._getter;
    }

    template <typename Poly>
    std::vector<std::vector<Poly>>& State<Poly>::get_used_polys() {
        return _current_polys;
    }
    
    template <typename Poly>
    void State<Poly>::set_history(const std::vector<std::tuple<unsigned short, unsigned short, unsigned short>> & history) {
        _history = history;
    }

    template <typename Poly>
    bool State<Poly>::all_polys_match(const std::vector<Poly>& polyrow1, 
                            const std::vector<Poly>& polyrow2, 
                            std::function<Poly(const Poly&)> f) const {

        for(const auto& poly1: polyrow1){
            bool found_match = false;
            Poly transformed_poly = f(poly1);
            for(const auto& poly2: polyrow2){
                if(transformed_poly == poly2){
                    found_match = true;
                    break;
                }
            }
            if(!found_match)
                return false;
        }
        return true;
    }

    template <typename Poly>
    State<Poly>::State(const Polygon::BarePoly& puzzle_frame, 
                       const Polygon::Pool<Polygon::BarePoly>& poly_pool, 
                       const CalcSettings& settings, 
                       Allocator::IAllocator<std::shared_ptr<IState>>* allocator):
        _preallocate{20},
        _size{0},
        _frame{Poly(_preallocate)},
        _poly_temp{Poly(_preallocate)},
        _default_frame{Poly(puzzle_frame)},
        _poly_pool{Polygon::Pool<Poly>(poly_pool.convert_sort<Poly>())},
        _const_poly_pool{Polygon::Pool<Poly>(poly_pool.convert_sort<Poly>())},
        _current_polys{_poly_pool.num_distinct_polys()},
        _num_polys(_poly_pool.num_distinct_polys()),
        _history{},
        _overlapper{Polygon::overlapper_factory<Poly>(settings.overlapper)},
        _selector{Polygon::selector_factory<Poly>(settings.node_selector)},
        _allocator{allocator}{
        
        // Default allocation, preparing State to do calculations with 
        // as few memory allocations as possible
        
        _history.reserve(20); // SIMAO: usar tamanho programático. (qual?)
        _getter = [](unsigned int){return 0;}; // SIMAO: permitir seleccionar getter

    }

    template <typename Poly>
    void State<Poly>::set_initial_state(){
        _frame = _default_frame;
        _poly_pool = _const_poly_pool;
        _size = 0;
        for(auto& n: _num_polys){
            n = 0;
        }
        _history.clear();

        // analytics.reset(); // SIMAO: implementar
    }

    template <typename Poly>
    void State<Poly>::fast_copy(Poly& to, const Poly& from, const Poly::VertexType& vertex){
        // "to" already has most of the structure in place: the linked list and opening angle.
        // the only thing that needs to change is start/end angles and positions
        

        // use one of the "to" angles as a placeholder
        auto& angle = to.get_vertex(0).angle_opening;
        const auto& a = from.get_head()->angle_end;
        const auto& b = vertex.angle_start;

        angle.get_cos_mod() = b.get_cos_mod_const()*a.get_cos_mod_const() + b.get_sin_mod_const()*a.get_sin_mod_const();
        angle.get_sin_mod() = b.get_sin_mod_const()*a.get_cos_mod_const() - b.get_cos_mod_const()*a.get_sin_mod_const();

        unsigned int size_ll = from.size();

        // Copy the head into place first, so it can be used for reference later
        const auto& vertex_from = from.get_vertex_not_mod(0);
        auto& vertex_to = to.get_vertex(0);

        vertex_to.position = vertex_from.position.rotate(angle);
        auto dr = vertex.position - vertex_to.position;
        vertex_to.position = vertex.position;
        vertex_to.angle_end = vertex_from.angle_end + angle;
        vertex_to.angle_start = vertex_from.angle_start + angle;
        
        for(unsigned int i = 1; i < size_ll; i++){
            const auto& vertex_from = from.get_vertex_not_mod(i);
            auto& vertex_to = to.get_vertex(i);
            vertex_to.position = vertex_from.position.rotate(angle);
            vertex_to.position = vertex_to.position + dr;
            vertex_to.angle_end = vertex_from.angle_end + angle;
            vertex_to.angle_start = vertex_from.angle_start + angle;
        }
        
        to.get_vertex(0).angle_opening = from.get_vertex_not_mod(0).get_opening();
        
    }

    template <typename Poly>
    void State<Poly>::create_new_state(const State& state, 
                                       const Poly& transformed_poly,
                                       unsigned short node_index,
                                       unsigned short poly_index, 
                                       unsigned short variation_index){
        
        TimingBranch& timing1 = _analytics.branch("create new state");
        timing1.start();

        _num_polys = state._num_polys;
        _num_polys.at(poly_index)++; 

        _size = state._size;
        _size++; // SIMAO: substituir _size por nome melhor

        _history = state._history;
        _history.push_back({node_index, poly_index, variation_index});

        _frame = state._frame;
        _poly_temp = transformed_poly;

        auto insertion_vertex = _frame.vertex_from_index(node_index);
        auto head = _poly_temp.get_head();

        auto modified = _frame.merge(insertion_vertex, _poly_temp, head);
        _frame.prune_LL(modified, _getter);

        timing1.end();
    }


    template <typename Poly>
    bool State<Poly>::equals_derived(const State& other, Polygon::Transformations transf) const {
            
        switch(transf){
            case Polygon::Transformations::RotFlip:
                return equals_under_transform(other, Polygon::identity<Poly>) or
                        equals_under_transform(other, Polygon::rotate_90<Poly>) or
                        equals_under_transform(other, Polygon::rotate_270<Poly>) or
                        equals_under_transform(other, Polygon::rotate_180<Poly>) or
                        equals_under_transform(other, Polygon::flip_x<Poly>) or
                        equals_under_transform(other, Polygon::flip_y<Poly>) or
                        equals_under_transform(other, Polygon::flip_D<Poly>) or 
                        equals_under_transform(other, Polygon::flip_AD<Poly>);

            case Polygon::Transformations::Id:
                return equals_under_transform(other, Polygon::identity<Poly>);
    
            default:
                assert(false); // SIMAO: implementar outros casos
        }

    }


    template <typename Poly>
    bool State<Poly>::equals(std::shared_ptr<IState> other, Polygon::Transformations transf) const {
        if(typeid(*other) != typeid(*this))
            return false;
        
        std::shared_ptr<State> derivedPtr = std::static_pointer_cast<State>(other);
        return equals_derived(*derivedPtr, transf);
    }


    template <typename Poly>
    bool State<Poly>::equals_under_transform(const State& other, std::function<Poly(const Poly&)> f) const {
        if(other._size != _size)
            return false;

        if(_frame.is_valid() and other.is_valid() and !(other._frame == _frame))
            return false;


        // SIMAO: arranjar isto
        for(unsigned int i=0; i<_current_polys.size(); i++){
            const auto& polyrow = _current_polys.at(i);
            const auto& other_polyrow = other._current_polys.at(i);
            if(!all_polys_match(polyrow, other_polyrow, f)){
                return false;
            }
        }
        return true;
    }

    template <typename Poly>
    std::vector<std::pair<unsigned int, Polygon::RestrictedPoly<Poly>*>> State<Poly>::get_remaining_poly_pool() {
        using Pair = std::pair<unsigned int, Polygon::RestrictedPoly<Poly>*>;
        std::vector<Pair> remaining;

        for(unsigned int i=0; i<_poly_pool.pool.size(); i++){
            if(_num_polys.at(i) < _poly_pool.pool.at(i).get_amount()){
                remaining.push_back(Pair(i, &_poly_pool.pool.at(i)));
            }
        }
        return remaining;
    }


    template <typename Poly>
    void State<Poly>::activate_history(bool respect_restrictions){
        auto history = _history;
        set_initial_state();
        iterate(history, respect_restrictions);
    }
    
    template <typename Poly>
    void State<Poly>::iterate(const std::vector<std::tuple<unsigned short, unsigned short, unsigned short>>& history, bool respect_restrictions){

        AnalyticsThread analytics;

        _current_polys = std::vector<std::vector<Poly>>(_poly_pool.num_distinct_polys());

        for(const auto& [node_index, poly_index, variation_index]: history){

            const auto& insertion_vertex = *_frame.vertex_from_index(node_index);
            auto& restricted_poly = _poly_pool.pool.at(poly_index);
            const auto& restriction = restricted_poly.get_restriction();
            auto& transformed_poly = restricted_poly.get_variations().at(variation_index);
            
            if(respect_restrictions and !angles_compatible(transformed_poly.get_head()->angle_opening, insertion_vertex.angle_opening)){
                std::cout << "angles not compatible\n";
                assert(false);
            }
            
            transformed_poly.move_into(insertion_vertex);

            if(respect_restrictions and (restriction.is_valid() and !_overlapper(transformed_poly, restriction, analytics.branch("a")))){
                std::cout << "restriction not respected\n";
                restriction.print();
                transformed_poly.print();
                assert(false);
            }
                
            if(respect_restrictions and _overlapper(transformed_poly, _frame, analytics.branch("a"))){
                std::cout << "overlap not respected\n";
                _frame.print();
                transformed_poly.print();
                assert(false);
            }

            auto new_state = _allocator->allocate();
            std::shared_ptr<State> new_state_cast = std::dynamic_pointer_cast<State>(new_state); // retains ref counting

            _current_polys.at(poly_index).push_back(transformed_poly);
            auto polys = _current_polys;

            new_state_cast->create_new_state(*this, transformed_poly, node_index, poly_index, variation_index);
            get_data_from(*new_state_cast);
            _current_polys = polys;
            _allocator->release({new_state});
        }
        
    }


    template <typename Poly>
    std::vector<std::shared_ptr<IState>> State<Poly>::find_next_states() {
        TimingBranch& timing  = _analytics.branch("find_next_states");
        TimingBranch& timing2 = _analytics.branch("angles compatible");
        TimingBranch& timing3 = _analytics.branch("move polygon");
        TimingBranch& timing4 = _analytics.branch("allocate");
        timing.start();
        
        if(!is_valid() and finalized())
            throw;

        if(!is_valid() or finalized())
            return {};

        unsigned int num_allocated = 5;
        std::vector<std::shared_ptr<IState>> allocated;
        allocated.reserve(num_allocated);

        std::vector<std::shared_ptr<IState>> next_states;
        next_states.reserve(100);

        std::vector<unsigned int> node_indices = _selector(_frame);
        for(unsigned int node_index: node_indices){
                
            const auto& insertion_vertex = *_frame.vertex_from_index(node_index);
            
            for(auto [poly_index, restricted_poly]: get_remaining_poly_pool()){
                short variation_index = -1;
                for(auto& transformed_poly: restricted_poly->get_variations()){
                    
                    variation_index++;
                    
                    timing2.start();
                    bool compatible = angles_compatible(transformed_poly.get_head()->angle_opening, insertion_vertex.angle_opening);
                    timing2.end();
                    
                    if(!compatible)
                        break;
                        
                    timing3.start(); 
                    const auto& const_poly = _const_poly_pool.pool.at(poly_index).get_variations().at(variation_index);
                    fast_copy(transformed_poly, const_poly, insertion_vertex);
                    timing3.end();

                    const auto& restriction = restricted_poly->get_restriction();
                    if(restriction.is_valid() and !_overlapper(transformed_poly, restriction, _analytics.branch("overlap restriction")))
                        continue;

                    if(!_overlapper(transformed_poly, _frame, _analytics.branch("overlap frame"))){
                        timing4.start();
                        
                        if(allocated.size() == 0){
                            allocated = std::move(_allocator->allocate(num_allocated));
                        }
                        std::shared_ptr<IState> new_state = allocated.back();
                        allocated.pop_back();

                        std::shared_ptr<State> new_state_cast = std::dynamic_pointer_cast<State>(new_state); // retains ref counting
                        if(!new_state_cast)
                            throw;
                        timing4.end();

                        new_state_cast->create_new_state(*this, transformed_poly, node_index, poly_index, variation_index);
                        next_states.push_back(new_state);
                    }
                }
            }
        }

        timing.end();
        _allocator->release(allocated);

        return next_states;
    }

    template <typename Poly>
    bool State<Poly>::is_valid() const {
        return _frame.is_valid();   
    }

    template <typename Poly>
    bool State<Poly>::finalized() const {
        return _size == _poly_pool.num_total_polys();
    }


    template <typename Poly>
    AnalyticsThread& State<Poly>::get_analytics() {
        return _analytics;
    }

    template <typename Poly>
    unsigned int State<Poly>::size() const {
        return _size;
    }

    
    template <typename Poly>
    void State<Poly>::print() const {
        std::cout << "-- Printing state -- Size: " << _size << "\n Frame: ";
        _frame.print();

        std::cout << "current polygon ids: ";
        for(const auto& n: _num_polys){
            std::cout << n << " ";
        }

        std::cout << "\nHistory:";
        for(const auto& triple: _history){
            std::cout << std::get<0>(triple) << " " << std::get<1>(triple) << " " << std::get<2>(triple) << " ";
        }

        std::cout << "\ncurrent polygons:\n";
        for(const auto& polyrow: _current_polys){
            for(const auto& poly: polyrow)
                poly.print();
        }
    }


    template <typename Poly>
    void State<Poly>::print_history() const {
        
        
        for(const auto& triple: _history){
            std::cout << std::get<0>(triple) << " " << std::get<1>(triple) << " " << std::get<2>(triple) << " ";
        }

        std::cout << "\n";
    }



    template <typename Poly>
    void State<Poly>::print_output() const {
        
        std::cout << "History:";
        for(const auto& triple: _history){
            std::cout << std::get<0>(triple) << " " << std::get<1>(triple) << " " << std::get<2>(triple) << " ";
        }

        {
            auto v = _frame.get_head();
            std::cout << "{";
            for(unsigned int i=0; i<_frame.size(); i++){
                auto x = v->position.get_x();
                auto y = v->position.get_y();
                v = v->next;

                std::cout << "(" << x << "," << y << ")";
            }
            std::cout << "}";
        }
        
        for(const auto& polyrow: _current_polys){
            std::cout << "<";
            for(const auto& poly: polyrow){
                auto v = poly.get_head();
                std::cout << "[";
                for(unsigned int i=0; i<poly.size(); i++){
                    auto x = v->position.get_x();
                    auto y = v->position.get_y();
                    v = v->next;

                    std::cout << "(" << x << "," << y << ")";
                }
                std::cout << "]";
            }
            std::cout << ">";
        }
        std::cout << "\n";
    }


    using namespace Polygon;
    using StateGenerator = std::function<std::shared_ptr<IState>(Allocator::IAllocator<std::shared_ptr<IState>>*)>;

    StateGenerator factory(const std::string& name, const BarePoly& starting_frame, 
                           const Pool<BarePoly>& poly_pool, const CalcSettings& settings){
        if(name == "LLPoly"){
            return [&](Allocator::IAllocator<std::shared_ptr<IState>>* memory_pool){

                using Poly = LLPoly<Float<double>>; 
                // using Poly = LLPoly<Float<float>>; // SIMAO: verificar se depois de tratar da alocação de memoria, isto torna o código mais rápido
                return std::make_shared<State<Poly>>(starting_frame, poly_pool, settings, memory_pool);
            };

        } else if(name == "ContigPoly"){
            return [&](Allocator::IAllocator<std::shared_ptr<IState>>* memory_pool){

                using Poly = ContigPoly<Float<double>>;
                // using Poly = ContigPoly<double>;
                return std::make_shared<State<Poly>>(starting_frame, poly_pool, settings, memory_pool);
            };

        } else {
            std::cout << "Unsupported poly type chosen\n";
            exit(1);
        }
    }

} // State