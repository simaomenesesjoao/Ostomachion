#pragma once
#include "points_vectors.cpp"


template <typename Num>
class Vertex{
    using Ang = Angle<Num>;
    using Poin = Point<Num>;

public:
    Vertex(Poin const& P): position{P}, prev{nullptr}, next{nullptr}, end_init{false}, start_init{false}{};

    Vertex(const Vertex& v): position{v.position}, angle_start{v.angle_start}, angle_end{v.angle_end}, 
                       angle_opening{v.angle_opening}, prev{nullptr}, next{nullptr}, end_init{v.end_init}, 
                       start_init{v.start_init}{};


    void update_opening(){
        // assert(end_init == true and start_init == true);
        if(end_init and start_init)
            angle_opening = angle_end - angle_start;
    }

    void update_end(Ang const& A){
        angle_end = A;
        end_init = true;
        update_opening();
    }

    void update_start(Ang const& A){
        angle_start = A;
        start_init = true;
        update_opening();
    }


    const Ang& get_opening() const {
        return angle_opening;
    }

    void print() const {
        Poin pos = position;
        std::cout << "(x,y)=(" << (float)pos.get_x() << "," << (float)pos.get_y() << ") " 
                    << (float)angle_start << " " << (float)angle_end << " " << (float)angle_opening << std::endl;

    }

    void print_ll() const {
        Poin pos = position;
        float x = (float)pos.get_x();
        float y = (float)pos.get_y();

        std::cout << this << " " << next << " " << prev
                    << "(x,y)=(" << x << "," << y << ") " 
                    << (float)angle_start << " " << (float)angle_end << " " << (float)angle_opening << std::endl;

    }

    template <typename N>
    friend std::ostream& operator<<(std::ostream& os, Vertex<N> const& node);

    Poin position;
    Ang angle_start, angle_end, angle_opening;
    Vertex *prev, *next;
    bool end_init, start_init;
};


template <typename Num>
std::ostream & operator<<(std::ostream& stream, Vertex<Num> const& node){
    stream <<    "( " << node.position << ", " 
                      << node.angle_start << ", "
                      << node.angle_end << ", "
                      << node.angle_opening << ")";
    return stream;
}
