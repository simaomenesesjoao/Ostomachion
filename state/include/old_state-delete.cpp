#pragma once
#include <memory>
#include <vector>
#include "vertex.cpp"
#include "polygon.cpp"
#include "settings.hpp"
#include "customFloat.cpp"
#include "analytics.hpp"

namespace State{
    
    class IState {
    public:
        virtual std::vector<std::shared_ptr<IState>> find_next_states(TimingBranch&) const = 0;
        virtual bool finalized() const = 0;
        virtual void print() const = 0;
        virtual void print_output() const = 0;
        virtual ~IState() = default;
        virtual bool equals(std::shared_ptr<IState>, Polygon::Transformations) const = 0;
        virtual void activate_history(const Polygon::BarePoly& starting_frame, 
            const Polygon::Pool<Polygon::BarePoly>& poly_pool, 
            const CalcSettings& settings) = 0;
    };

    template <typename Poly>
    class State: public IState {
    public:
        State(const Polygon::BarePoly& starting_frame, 
            const Polygon::Pool<Polygon::BarePoly>& poly_pool, 
            const CalcSettings& settings);

        // State(const State& state);
        State(const State& state, Poly&& poly, unsigned int, unsigned int, unsigned int, TimingBranch&);
        State& operator=(const State& state) = default;

        // std::vector<std::vector<Poly>>& get_used_polys();

        bool all_polys_match(const std::vector<Poly>& polyrow1, 
                             const std::vector<Poly>& polyrow2, 
                             std::function<Poly(const Poly&)> f) const;

        bool equals_derived(const State& other, Polygon::Transformations transf) const;
        bool equals(std::shared_ptr<IState> other, Polygon::Transformations transf) const override;
        bool equals_under_transform(const State& other, std::function<Poly(const Poly&)> f) const;
        std::vector<std::pair<unsigned int, const Polygon::RestrictedPoly<Poly>*>> get_remaining_poly_pool() const;
        std::vector<std::shared_ptr<IState>> find_next_states(TimingBranch&) const override;
        void insert_polygon(unsigned int, unsigned short, unsigned short, Poly&&);

        void iterate(const std::vector<std::pair<ushort, ushort>>&);
        void activate_history(const Polygon::BarePoly&, const Polygon::Pool<Polygon::BarePoly>&, const CalcSettings& settings) override;
        bool is_valid() const;
        bool finalized() const override;
        void print() const override;
        void print_output() const override;

    private:
        Poly _frame;
        std::shared_ptr<Polygon::Pool<Poly>> _poly_pool;
        std::vector<std::vector<Poly>> _current_polys;
        std::vector<unsigned short> _num_polys;
        std::vector<std::pair<unsigned short, unsigned short>> _history;
        std::function<bool(const Poly&, const Poly&, TimingBranch&)> _overlapper;
        std::function<unsigned int(const Poly&)> _selector;
        unsigned int _size;
    };

    // template <typename Poly>
    // std::vector<std::vector<Poly>>& State<Poly>::get_used_polys() {
    //     return _current_polys;
    // }
    

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
    State<Poly>::State(const Polygon::BarePoly& starting_frame, 
        const Polygon::Pool<Polygon::BarePoly>& poly_pool, 
        const CalcSettings& settings):
            _frame{starting_frame}, 
            _poly_pool{std::make_shared<Polygon::Pool<Poly>>(poly_pool.convert<Poly>())},
            _current_polys(std::vector<std::vector<Poly>>(_poly_pool->pool.size())),
            _num_polys(_poly_pool->pool.size()),
            _history{},
            _overlapper{Polygon::overlapper_factory<Poly>(settings.overlapper)},
            _selector{Polygon::selector_factory<Poly>(settings.node_selector)},
            _size{0}{};

    // template <typename Poly>
    // State<Poly>::State(const State& state):
    //     _frame{state._frame},
    //     _poly_pool{state._poly_pool},
    //     _current_polys{state._current_polys},
    //     _num_polys{state._num_polys},
    //     _history{state._history},
    //     _overlapper{state._overlapper}, 
    //     _selector{state._selector},
    //     _size{state._size}{};


    template <typename Poly>
    State<Poly>::State(const State& state, Poly&& poly, 
        unsigned int node_index, unsigned int poly_index, unsigned int variation_index, TimingBranch& timing):
        _frame{},
        _poly_pool{state._poly_pool},
        _current_polys{state._current_polys},
        _num_polys{state._num_polys},
        _history{state._history},
        _overlapper{state._overlapper}, 
        _selector{state._selector},
        _size{state._size}{
        timing.start();

        _num_polys.at(poly_index)++;
        _history.push_back(std::pair<unsigned short, unsigned short>(poly_index, variation_index));
        // SIMAO: history preallocate
        _size++;


        auto getter = [](unsigned int){return 0;}; // SIMAO: permitir seleccionar getter
        TimingBranch& timing1 = timing.builder->branch("copy merge");        
        timing1.start();
        auto modified = poly.copy_merge(state._frame, node_index);
        timing1.end();

        
        TimingBranch& timing2 = timing.builder->branch("prune");
        timing2.start();
        poly.prune_LL(modified, getter);
        timing2.end();
        _frame = std::move(poly);
        _frame.toggle_area_sign();


        // std::cout << "after move\n" << std::flush;
        // std::cout << "size ll: " << _frame.size() << "\n";
        // std::cout << "head: " << _frame.get_head() <<  "\n";
        
        // std::cout << "frame pointers\n";
        // _frame.print();
        // for(auto& vertex: _frame.get_vertices())
        //     std::cout << &vertex << " " << vertex.next << " " << vertex.prev << "\n";
        // std::cout << "\n";


        // std::cout << "linked list\n";
        // typename Poly::VertexType* c = _frame.get_head();
        // for(unsigned int i = 0; i < _frame.size(); i++){
        //     c->print();
        //     c = c->next;
        // }
        // std::cout << "ended move\n";


        timing.end();

        // auto insertion_vertex = _frame.vertex_from_index(node_index);
        // auto getter = [](unsigned int){return 0;}; // SIMAO: permitir seleccionar getter

        // typename Poly::VertexType* head = poly.get_head();
        // auto modified = _frame.merge(insertion_vertex, poly, head);
        // _frame.prune_LL(modified, getter);

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
    std::vector<std::pair<unsigned int, const Polygon::RestrictedPoly<Poly>*>> State<Poly>::get_remaining_poly_pool() const {
        std::vector<std::pair<unsigned int, const Polygon::RestrictedPoly<Poly>*>> remaining;

        for(unsigned int i=0; i<_poly_pool->pool.size(); i++){
            if(_num_polys.at(i) < _poly_pool->pool.at(i).get_amount()){
                remaining.push_back({i, &_poly_pool->pool.at(i)});
            }
        }
        return remaining;
    }

    template <typename Poly>
    void State<Poly>::insert_polygon(unsigned int node_index, 
        unsigned short poly_index, unsigned short variation_index, Poly&& poly){
        

        _num_polys.at(poly_index)++;
        _history.push_back(std::pair<unsigned short, unsigned short>(poly_index, variation_index));
        
        _size++;

        
        auto insertion_vertex = _frame.vertex_from_index(node_index);
        auto getter = [](unsigned int){return 0;}; // SIMAO: permitir seleccionar getter

        typename Poly::VertexType* head = poly.get_head();
        auto modified = _frame.merge(insertion_vertex, poly, head);
        _frame.prune_LL(modified, getter);
        
    }

    template <typename Poly>
    void State<Poly>::activate_history(const Polygon::BarePoly& starting_frame, 
                                        const Polygon::Pool<Polygon::BarePoly>& poly_pool, 
                                        const CalcSettings& settings){
        auto history = _history;
        *this = State<Poly>(starting_frame, poly_pool, settings); // reset
        iterate(history);
    }
    
    template <typename Poly>
    void State<Poly>::iterate(const std::vector<std::pair<unsigned short, unsigned short>>& history){
        AnalyticsThread analytics;
        // std::cout << "--------------------------------------------- entered iterate -----------------------------\n";
        // std::cout << "frame before merge:\n";
        // _frame.print();

        _current_polys = std::vector<std::vector<Poly>>(_poly_pool->pool.size());

        for(const auto& [poly_index, variation_index]: history){
            // std::cout << "poly and variation index: " << poly_index << " " << variation_index << "\n";

            unsigned short node_index = _selector(_frame);
            const auto& insertion_vertex = *(_frame.vertex_from_index(node_index));
            const auto& restricted_poly = _poly_pool->pool.at(poly_index);
            const auto& restriction = restricted_poly.get_restriction();
            const auto& transformed_poly = restricted_poly.get_variations().at(variation_index);
            
            if(!angles_compatible(transformed_poly.get_head()->angle_opening, insertion_vertex.angle_opening)){
                std::cout << "angles not compatible\n";
                assert(false);
            }
            

            Poly new_poly = transformed_poly.copy_into(insertion_vertex, 50); // SIMAO: corrigir


            if(restriction.is_valid() and !_overlapper(new_poly, restriction, analytics.branch("a"))){
                std::cout << "### error ### problem with restriction\n";
                restriction.print();
                new_poly.print();
                assert(false);
            }
                
            if(_overlapper(new_poly, _frame, analytics.branch("a"))){
                std::cout << "problem with overlap\n";
                new_poly.print();
                assert(false);
            }

            _current_polys.at(poly_index).push_back(new_poly);
            // new_poly.print();
            // unsigned int count = 0;
            // for(auto& polyrow: _current_polys)
            //     count += polyrow.size();
            // std::cout << "size: " << count << "\n";
            insert_polygon(node_index, poly_index, variation_index, std::move(new_poly));
            // std::cout << "frame after merge:\n";
            // _frame.print();
        }
        
    }


    template <typename Poly>
    std::vector<std::shared_ptr<IState>> State<Poly>::find_next_states(TimingBranch& timing) const {
        timing.start();
        
        // std::cout << "Inside find_next\n";
        // _frame.print();
        // for(auto& vertex: _frame.get_vertices())
        //     std::cout << &vertex << " " << vertex.next << " " << vertex.prev << "\n";
        // std::cout << "\n";


        // std::cout << "linked list\n";
        // auto c = _frame.get_head();
        // for(unsigned int i = 0; i < _frame.size(); i++){
        //     c->print();
        //     c = c->next;
        // }
        // std::cout << "\n";

        if(!is_valid() and finalized())
            std::cout << "impossible\n";

        if(!is_valid() or finalized())
            return {};

        unsigned int node_index = _selector(_frame);
        const auto& insertion_vertex = *_frame.vertex_from_index(node_index);
        
        std::vector<std::shared_ptr<IState>> next_states;
        next_states.reserve(50);
        for(auto [poly_index, restricted_poly]: get_remaining_poly_pool()){
            short variation_index = -1;
            for(const auto& transformed_poly: restricted_poly->get_variations()){
                variation_index++;

                TimingBranch& timing2 = timing.builder->branch("checking if angles compatible");
                timing2.start();
                bool compatible = angles_compatible(transformed_poly.get_head()->angle_opening, insertion_vertex.angle_opening);
                timing2.end();


                if(compatible){
                    // std::cout << "compatible\n";
                    TimingBranch& timing3 = timing.builder->branch("copy into");
                    timing3.start(); 
                    // SIMAO: if this next line becomes a memory bottleneck, I should implement thread-registers
                    auto new_poly = transformed_poly.copy_into(insertion_vertex, transformed_poly.size() + _frame.size());
                    timing3.end();

                    const auto& restriction = restricted_poly->get_restriction();
                    if(restriction.is_valid() and !_overlapper(new_poly, restriction, timing.builder->branch("overlap restriction")))
                        continue;

                    if(!_overlapper(new_poly, _frame, timing.builder->branch("overlap frame"))){

                        TimingBranch& timing1 = timing.builder->branch("creation wrapper");
                        timing1.start();
                        // std::cout << "doesnt overlap\n";
                        // timing1.start();
                        // std::shared_ptr<State> new_state = std::make_shared<State>(*this);
                        // timing1.end();

                        // TimingBranch& timing4 = timing.builder->branch("insertion");
                        // timing4.start();
                        // new_state->insert_polygon(node_index, poly_index, variation_index, std::move(new_poly));
                        // next_states.push_back(new_state);
                        // timing4.end();

                        std::shared_ptr<State> new_state = std::make_shared<State>(*this, 
                            std::move(new_poly), node_index, poly_index, variation_index, timing.builder->branch("state construction"));
                        
                        timing1.end();
                        next_states.push_back(new_state);
                    }

                }

            }
        }
        timing.end();
        // std::cout << "left find_next\n";
        return next_states;
    }

    template <typename Poly>
    bool State<Poly>::is_valid() const {
        return _frame.is_valid();   
    }

    template <typename Poly>
    bool State<Poly>::finalized() const {
        return _size == _poly_pool->size;
    }
    
    template <typename Poly>
    void State<Poly>::print() const {
        
        std::cout << "---- Printing state  -------\nFrame:\n";
        _frame.print();
        std::cout << "current polygon ids: ";
        for(const auto& n: _num_polys){
            std::cout << n << " ";
        }
        std::cout << "\nHistory:\n";
        for(const auto& pair: _history){
            std::cout << pair.first << " " << pair.second << " ";
        }
        std::cout << "\ncurrent polygons:\n";
        for(const auto& polyrow: _current_polys){
            for(const auto& poly: polyrow)
                poly.print();
        }
    }


    template <typename Poly>
    void State<Poly>::print_output() const {
        
        for(const auto& polyrow: _current_polys){
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
        }
        std::cout << "\n";
    }


    using namespace Polygon;
    using StateGenerator = std::function<std::shared_ptr<IState>(const Pool<BarePoly>&, const CalcSettings&)>;

    StateGenerator factory(const std::string& name){
        if(name == "LLPoly"){
            return [](const BarePoly& starting_frame, 
                    const Pool<BarePoly>& poly_pool, 
                    const CalcSettings& settings){

                    using Poly = LLPoly<Float<double>>; 
                    // using Poly = LLPoly<Float<float>>;
                return std::make_shared<State<Poly>>(starting_frame, poly_pool, settings);
            };

        } else if(name == "ContigPoly"){
            return [](const BarePoly& starting_frame, 
                    const Pool<BarePoly>& poly_pool, 
                    const CalcSettings& settings){

                    using Poly = ContigPoly<Float<double>>;
                    // using Poly = ContigPoly<Float<float>>;
                return std::make_shared<State<Poly>>(starting_frame, poly_pool, settings);
            };

        } else {
            std::cout << "Unsupported poly type chosen\n";
            exit(1);
        }
    };


    // StateGenerator factory(const std::string& name){
    //     if(name == "LLPoly"){
    //         return [](const BarePoly& starting_frame, 
    //                 const Pool<BarePoly>& poly_pool, 
    //                 const CalcSettings& settings){

    //                 using Poly = LLPoly<Float<double>>; 
    //                 // using Poly = LLPoly<Float<float>>;
    //             return std::make_shared<State<Poly>>(starting_frame, poly_pool, settings);
    //         };

    //     } else if(name == "ContigPoly"){
    //         return [](const BarePoly& starting_frame, 
    //                 const Pool<BarePoly>& poly_pool, 
    //                 const CalcSettings& settings){

    //                 using Poly = ContigPoly<Float<double>>;
    //                 // using Poly = ContigPoly<Float<float>>;
    //             return std::make_shared<State<Poly>>(starting_frame, poly_pool, settings);
    //         };

    //     } else {
    //         std::cout << "Unsupported poly type chosen\n";
    //         exit(1);
    //     }
    // };


} // State