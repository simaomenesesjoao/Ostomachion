#pragma once
#include <list>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <cassert>

#include "points_vectors.cpp" // SIMAO: remover
#include "vertex.cpp"
#include "intersection_algorithms.cpp"
#include "BarePoly.cpp"

namespace Polygon {

    template <typename Num>
    class ContigPoly{

    private:
        using Poin = Point<Num>;
        using V = Vertex<Num>;

        V* head;
        std::vector<V> vertices;
        bool area_positive;
        bool LL_active;
        unsigned int size_ll;

    public:
        using Ang = Angle<Num>;
        using VertexType = V;

        ContigPoly(): 
            head{nullptr}, 
            vertices{}, 
            area_positive{false},
            LL_active{false}, 
            size_ll{0}{}

        ContigPoly(unsigned int preallocate):
            head{nullptr}, 
            vertices{}, 
            area_positive{false},
            LL_active{false}, 
            size_ll{0}{
            vertices.reserve(preallocate);
        }

        ContigPoly(const BarePoly& poly):
            ContigPoly{poly.point_list}{};

        ContigPoly(const std::vector<std::vector<int>>& points): 
            head{nullptr}, 
            vertices{}, 
            area_positive{false}, 
            LL_active{false}, 
            size_ll{0}{
            // Turn the list of points into nodes, set them as the vertices of 
            // the polygon and calculate the angles between them

            if(points.size() == 0){
                return;
            }

            std::vector<int> last_point = points.back();
            int prev_x{last_point.at(0)}, prev_y{last_point.at(1)};

            for(auto& point: points){
                
                int x{point.at(0)}, y{point.at(1)};
                Ang angle(x-prev_x, y-prev_y);
                
                V vertex = V(Poin{prev_x,prev_y});
                vertex.update_start(angle);
                vertices.push_back(vertex);

                prev_x = x;
                prev_y = y;
            }
            
            V* prev = &vertices.back();

            for(auto& vertex: vertices){
                vertex.update_end(-prev->angle_start);
                vertex.update_opening();
                prev = &vertex;
            }

            area_positive = (get_area() > 0);

            if (!vertices.empty()) {
                auto first = vertices.front();
                vertices.erase(vertices.begin());
                vertices.push_back(first);
            }

            size_ll = vertices.size();
            connect_LL();
            head = &vertices.at(0); // SIMAO: garantir que head é actualizado depois de merge e prune
            
        }


        ContigPoly(ContigPoly const& other_poly):
            head{nullptr},
            vertices{other_poly.vertices},
            area_positive{other_poly.area_positive}, 
            LL_active{other_poly.LL_active},
            size_ll{other_poly.size_ll} {
                if(vertices.size() > 0)
                    head = &vertices.at(0);
                connect_LL();
            }



        ContigPoly(ContigPoly && other_poly):
            head{other_poly.head},
            vertices{std::move(other_poly.vertices)},
            area_positive{other_poly.area_positive}, 
            LL_active{other_poly.LL_active},
            size_ll{other_poly.size_ll}{}

            


        // ContigPoly(ContigPoly const& other_poly, unsigned int preallocate):
        //     head{nullptr},
        //     vertices{},
        //     area_positive{other_poly.area_positive}, 
        //     LL_active{other_poly.LL_active},
        //     size_ll{other_poly.size_ll} {
        //         vertices.reserve(preallocate);
        //         vertices = other_poly.vertices;
                
        //         if(vertices.size() > 0)
        //             head = &vertices.at(0);

        //         connect_LL();
                
        //     }


        ContigPoly& operator=(ContigPoly const& other){
            if(&other == this)
                return *this;

            // if(other.head == nullptr){
            //     size_ll = 0;
            //     head = nullptr;
            //     return *this;
            // }
                

            area_positive = other.area_positive;
            size_ll = other.size_ll;


            
            V* current = other.head;
            vertices.clear();
            for(unsigned int i = 0; i < size_ll; i++){
                vertices.push_back(*current);
                current = current->next;
            }

            head = nullptr;
            if(vertices.size() > 0)
                head = &vertices.at(0);

            for(unsigned int i = 0; i < size_ll; i++){
                vertices.at(i).next = &vertices.at((i+1)%size_ll);
                vertices.at(i).prev = &vertices.at((i-1+size_ll)%size_ll);
            }
            
            return *this;
        }
        

        ContigPoly& operator=(ContigPoly&& other){
            if(&other == this)
                return *this;

            head = other.head;
            area_positive = other.area_positive;
            size_ll = other.size_ll;
            vertices = std::move(other.vertices);
            
            return *this;
        }
        
        void toggle_area_sign(){
            area_positive = !area_positive;
        }

        Num get_area(){
            std::vector<std::pair<Num, Num>> points;
            
            for(const auto& vertex: vertices){
                points.push_back({vertex.position.get_x(),vertex.position.get_y()});
            }

            return shoelace_area<Num>(points);
        }

        void translate(Poin const& dr){
            
            for(unsigned int i = 0; i < vertices.size(); i++){
                V& vertex = vertices.at(i);
                vertex.position = vertex.position + dr;
            }

        }

        void rotate(Ang const& angle, Poin const& rot_center){
            // Rotate the polygon by angle around the rotation center

            for(unsigned int i = 0; i < vertices.size(); i++){
                V& vertex = vertices.at(i);
                vertex.angle_end = vertex.angle_end + angle;
                vertex.angle_start = vertex.angle_start + angle;
                vertex.position = rot_center + (vertex.position - rot_center).rotate(angle);
            }
        }

        void rotate(Ang const& angle){
            // Rotate the polygon by angle around the rotation center

            for(unsigned int i = 0; i < vertices.size(); i++){
                V& vertex = vertices.at(i);
                vertex.angle_end = vertex.angle_end + angle;
                vertex.angle_start = vertex.angle_start + angle;
                vertex.position = vertex.position.rotate(angle);
            }
        }

        // ContigPoly copy_into(const V& vertex, unsigned int preallocate) const {
        //     // Copies the polygon into the specified vertex position

        //     ContigPoly poly2(*this, preallocate);
        //     poly2.translate(vertex.position - head->position);
        //     poly2.rotate(vertex.angle_start - head->angle_end, vertex.position);
        //     return poly2;
        // }


        void move_into(const V& vertex) {
            // Moves the polygon into the specified vertex position
            rotate(vertex.angle_start - head->angle_end);
            translate(vertex.position - head->position);
        }



        void flip_x(){
            // Flip the polygon along x, but preserve orientation. In practice, 
            // this means changing the sign of every quantity which depends on x
            for(auto& vertex: vertices){
                vertex.position = {-vertex.position.get_x(), vertex.position.get_y()};
                auto start = vertex.angle_start;
                auto end = vertex.angle_end;
                vertex.angle_start = Ang{-end.get_cos(), end.get_sin()};
                vertex.angle_end = Ang{-start.get_cos(), start.get_sin()};
            }
            std::reverse(vertices.begin(), vertices.end());
            
        }

        void advance_head() {
            // SIMAO: corrigir caso em que não há elementos
            std::vector<V> new_vertices;
            for(unsigned int i = 1; i < vertices.size(); i++){
                new_vertices.push_back(vertices.at(i));
            }
            new_vertices.push_back(vertices.at(0));
            vertices = new_vertices;
        }

        void flip_y(){
            // Flip the polygon along y, but preserve orientation. In practice, 
            // this means changing the sign of every quantity which depends on y
            for(auto& vertex: vertices){
                vertex.position = {vertex.position.get_x(), -vertex.position.get_y()};
                auto start = vertex.angle_start;
                auto end = vertex.angle_end;
                vertex.angle_start = Ang{end.get_cos(), -end.get_sin()};
                vertex.angle_end = Ang{start.get_cos(), -start.get_sin()};
            }
            std::reverse(vertices.begin(), vertices.end());
        }

        // unsigned int index_from_pointer(V* pointer){

        //     for(unsigned int i = 0; i < vertices.size(); i++)
        //         if(&vertices.at(i) == pointer)
        //             return i;

        //     assert(false); // SIMAO: implementar exceptions
        // }

        std::vector<V>& get_vertices() {
            return vertices;
        }

        V& get_vertex(unsigned int i) {
            return vertices.at(i);
        }


        const V& get_vertex_not_mod(unsigned int i) const {
            return vertices.at(i);
        }

        const std::vector<V>& get_vertices() const {
            return vertices;
        }



        std::vector<V*> merge(V *this_node, ContigPoly & other_poly, V *other_node){

            size_ll += other_poly.size_ll;

            // Connect the two linked lists
            V *temp = other_node->prev;
            
            other_node->prev = this_node->prev;
            other_node->prev->next = other_node;

            this_node->prev = temp;
            this_node->prev->next = this_node;

            // Delete the linked list from one of the polygons to 
            // avoid deleting twice in the destructor
            other_poly.head = nullptr;
            other_poly.size_ll = 0;

            // Update the angles
            this_node->angle_end = -this_node->prev->angle_start;
            other_node->angle_end = -other_node->prev->angle_start;

            this_node->update_opening();
            other_node->update_opening();

            return {this_node, other_node};

        }


        // std::vector<V*> copy_merge(const ContigPoly & other_poly, unsigned int anchor_index){

        //     V* current = other_poly.head;
        //     for(unsigned int i = 0; i < other_poly.size_ll; i++){
        //         vertices.emplace_back(*current);         
        //         current = current->next;
        //     }

        //     for(unsigned int i = 0; i < other_poly.size_ll; i++){
        //         unsigned int j = i + size_ll;
        //         vertices.at(j).next = &vertices.at(size_ll + (i + 1)%other_poly.size_ll);
        //         vertices.at(j).prev = &vertices.at(size_ll + (i - 1 + other_poly.size_ll)%other_poly.size_ll);
        //     }

        //     V* other_node = &vertices.at(size_ll + anchor_index);
        //     // V* other_node = vertex_from_index(anchor_index);
        //     V* this_node = head;
            
            

        //     size_ll += other_poly.size_ll;

        //     // Connect the two linked lists
        //     V *temp = other_node->prev;
            
        //     other_node->prev = this_node->prev;
        //     other_node->prev->next = other_node;

        //     this_node->prev = temp;
        //     this_node->prev->next = this_node;

        //     // Update the angles
        //     this_node->angle_end = -this_node->prev->angle_start;
        //     other_node->angle_end = -other_node->prev->angle_start;

        //     this_node->update_opening();
        //     other_node->update_opening();

        //     return {this_node, other_node};

        // }


        void connect_LL(){
            // Iterates over the vertices and connects them in a linked-list

            if(size_ll == 0)
                return;

            V* last = &vertices.at(size_ll-1);

            for(auto& vertex: vertices){
                V* current = &vertex;
                current->prev = last;
                last->next = current;
                last = current;
            }    
            LL_active = true;
        }

        void prune_LL(std::vector<V*> update, std::function<unsigned int(unsigned int)> getter){
            // Remove points from the LL which are coincident or have
            // an opening angle of 0. This is repeated until no more points
            // can be removed. It may happen that all points are removed, in
            // which case the head becomes a nullptr

            // std::cout << "Entered pruneLL\n" << std::flush;
            // std::cout << "size ll: " << size_ll << "\n";
            // std::cout << "pointers that will be updated\n";
            // for(auto& v: update){
            //     std::cout << v << "\n";
            // }
            
            // std::cout << "vertices\n";
            // for(auto& vertex: vertices){
            //     std::cout << &vertex << " " << vertex.next << " " << vertex.prev << " ";
            //     vertex.print();
            // }
            // std::cout << "\n";


            // std::cout << "linked list\n";
            // V* c = head;
            // for(unsigned int i = 0; i < size_ll; i++){
            //     c->print();
            //     c = c->next;
            // }
            // std::cout << "\n";
            

            while(update.size()){
                
                if(size_ll == 1){
                    head = nullptr;
                    size_ll = 0;
                    break;
                }

                int i = getter(update.size());
                auto it = update.begin()+i;
                

                V *current = *it;
                update.erase(it);

                if(std::find(update.begin(), update.end(), current) != update.end())
                    continue;

                V *prev = current->prev;
                V *next = current->next;
                bool modified = true;

                if(current->position == prev->position){
                    prev->angle_start = current->angle_start;
                    prev->update_opening();


                } else if(current->position == next->position){
                    next->angle_end = current->angle_end;
                    next->update_opening();

                } else if(current->angle_opening.is_zero()){

                    // Check which neighbour is closest
                    Poin vec_prev = current->position - prev->position;
                    Num dist_prev2 = vec_prev.get_x()*vec_prev.get_x() + vec_prev.get_y()*vec_prev.get_y();

                    Poin vec_next = current->position - next->position;
                    Num dist_next2 = vec_next.get_x()*vec_next.get_x() + vec_next.get_y()*vec_next.get_y();

                    if(dist_prev2 < dist_next2){
                        prev->angle_start = -prev->angle_start;
                        prev->update_opening();

                    } else {
                        next->angle_end = -next->angle_end;
                        next->update_opening();
                    }
                } else if(current->angle_opening.is_180()){
                    // Nothing to do in this case
                } else {
                    modified = false;
                }

                if(modified){

                    next->prev = prev;
                    prev->next = next;
                    if(current == head){
                        head = current->next;
                    }

                    size_ll--;
                    update.push_back(next);
                    update.push_back(prev);
                }
            }

            // std::vector<V> new_vertices;
            // new_vertices.reserve(size_ll);
            
            // V* current = head;
            // for(unsigned int i = 0; i < size_ll; i++){
            //     new_vertices.push_back(*current);
            //     current = current->next;
            // }

            // vertices = new_vertices;
            // connect_LL();
            // if(vertices.size() > 0)
            //     head = &vertices.at(0);



            // std::cout << "After prune\n";
            // std::cout << "size ll: " << size_ll << "\n";
            // std::cout << "head: " << head <<  "\n";
            // for(auto& v: update){
            //     std::cout << v << "\n";
            // }
            
            // std::cout << "vertices\n";
            // for(auto& vertex: vertices){
            //     std::cout << &vertex << " " << vertex.next << " " << vertex.prev << " ";
            //     vertex.print();
            // }
            // std::cout << "\n";

            // std::cout << "linked list\n";
            // std::cout << "head: " << head << "\n";
            // c = head;
            // for(unsigned int i = 0; i < size_ll; i++){
            //     c->print();
            //     c = c->next;
            // }
            // std::cout << "Finished prune\n\n";
            
            // std::cout << "after prune\n";
            // print();

        }

    //     bool is_canonical() const {
    //         // SIMAO
    //         // Check if the polygon doesn't have 180 degrees, 0 degrees or coincident points
    //         // Optionally, I can also check if the angles make sense using doubles
    //         return false;
    //     }

        bool is_valid() const {
            return head != nullptr;
        }

    //     std::size_t get_hash() const {
    //         std::size_t h = 0;
    //         V *current = head;
    //         for(unsigned i=0; i<size_ll; i++){
    //             auto& pos = current->position;
    //             h += pos.get_x().get_hash()*4872191 + pos.get_y().get_hash()*991911983;
    //             h = h%230984193;
    //             current = current->next;
    //         }

    //         return h;
    //     }

        bool operator==(ContigPoly const& other_poly) const{            

            if(vertices.size() != other_poly.vertices.size())
                return false;

            // The polygons might be identical even though the heads are different
            unsigned int index = 0;
            for(unsigned int i = 0; i < size_ll; i++){

                if(vertices.at(i).position == other_poly.vertices.at(0).position){
                    index = i;
                    break;
                }
            }


            for(unsigned int i = 0; i < size_ll; i++){
                const V& v1 = vertices.at((index+i)%size_ll);
                const V& v2 = other_poly.vertices.at(i);

                if(!(v1.position == v2.position))
                    return false;

                if(v1.angle_end != v2.angle_end)
                    return false;

                if(v1.angle_start != v2.angle_start)
                    return false;

                if(v1.angle_opening != v2.angle_opening)
                    return false;

            }

            return true;
        }

        void print() const{
            std::cout << "Polygon of size " << size_ll << "\n";
            V *node = head;
            for(unsigned i=0; i<size_ll; i++){
                node->print();
                node = node->next;
            }
        }

        void print_ll() const{
            std::cout << "Polygon of size " << size_ll << "\n";
            V *node = head;
            for(unsigned i=0; i<size_ll; i++){
                node->print_ll();
                node = node->next;
            }
        }

        bool is_point_inside(Poin const& P, Poin const& Q = {100,101}) const {
            // Checks whether point P is inside the polygon. 
            // Returns false if it lies on the border or on a vertex
            // Check if the point P is part of a vertex or edge

            
            {           
                V *current = head;
                for(unsigned i = 0; i < size_ll; i++){
                    if(P == current->position)
                        return false;

                    if(point_on_edge(current->position, current->prev->position, P))
                        return false;

                    current = current->next;
                }
            }        
            
            
            V *current = head;
            int num_intersections{0};
            for(unsigned i = 0; i < size_ll; i++){

                if(edges_intersect(P, Q, current->position, current->prev->position)){
                    num_intersections++;
                } 
                    
                if(point_on_edge(P, Q, current->position)){

                    if(edge_splits_vertex(P, Q, current)){
                        num_intersections++;
                    }

                }


                if(edge1_includes_edge2(P, Q, current->position, current->prev->position)){
                    if(coincident_edges_diverge(P, Q, current, current->prev)){
                        num_intersections++;
                    }
                }
                
                current = current->next;            
            }

            // The number of intersections will be odd if it's clockwise
            // clockwise = negative area
            if(num_intersections%2 == area_positive)
                return true;

            return false;


        }

        bool edge_edge_intersection(const ContigPoly& other) const {
            V *current{head};
            unsigned int other_size_ll = other.size();

            // Check for edge-edge intersections
            for(unsigned i=0; i<size_ll; i++){
                Poin& A = current->position;
                Poin& B = current->prev->position;
                V *other_current{other.head};

                for(unsigned j=0; j<other_size_ll; j++){
                    Poin& C = other_current->position;
                    Poin& D = other_current->prev->position;

                    if(edges_intersect(A,B,C,D)){
                        return true;
                    }

                    other_current = other_current->next;
                }

                current = current->next;
            }

            return false;
        }

        bool edge_node_intersection(const ContigPoly& other) const{
            // Check if an edge of 'this' polygon intersects a vertex of the 'other'

            unsigned int other_size_ll = other.size();

            V *current{head};
            for(unsigned i=0; i<size_ll; i++){

                Poin const& P = current->position;
                Poin const& Q = current->prev->position;

                V *other_current{other.head};
                for(unsigned j=0; j<other_size_ll; j++){

                    if(point_on_edge(P, Q, other_current->position)){
                        if(is_inner_vertex(P, Q, other_current)){                            
                            return true;
                        }
                    }
                    other_current = other_current->next;
                }
                current = current->next;
            }

            return false;
        }

        bool node_node_intersection(const ContigPoly& other) const{

            unsigned int other_size_ll = other.size();
            V *A{head};
            for(unsigned i=0; i<size_ll; i++){
                
                V *B{other.head};
                for(unsigned j=0; j<other_size_ll; j++){
                    
                    if(A->position == B->position){
                        if(not nodes_compatible(*A, *B)){
                            return true;
                        }
                    }
                    B = B->next;
                }
                A = A->next;
            }

            return false;
        }

        bool points_inside(const ContigPoly& other) const {
            // Checks if any vertex from polygon 'other' is inside this polygon

            V *current{other.head};
            for(unsigned i=0; i<size_ll; i++){
                if(is_point_inside(current->position)){
                    return true;
                }

                current = current->next;
            }

            return false;
        }

    //     bool are_separate_convex(const ContigPoly& other) const {
    //         // Only strictly valid for convex polygons

    //         V* current = head;
    //         for(unsigned int i=0; i<size_ll; i++){
    //             V* next = current->next;
    //             const Point<Num>& C = current->position;
    //             const Point<Num>& N = next->position;

    //             Num dx = N.get_x() - C.get_x();
    //             Num dy = N.get_y() - C.get_y();
                
    //             V* other_current = other.head;
    //             bool separated = true;
    //             for(unsigned int j=0; j<other.size_ll; j++){
    //                 Point<Num> P = other_current->position - C;

    //                 // (dy, -dx) is (dx, dy) rotated 90º clockwise, 
    //                 // so it faces away from the polygon
    //                 Num proj = P.get_x()*dy - P.get_y()*dx;
                    
    //                 if(proj < 0){
    //                     separated = false;
    //                     break;
    //                 }

    //                 other_current = other_current->next;
    //             }
                
    //             if(separated)
    //                 return true;

    //             current = next;
    //         }
    //         // std::cout << count << "\n";
    //         return false;
    //     }

    //     bool convex_overlaps(const ContigPoly& other) const {
    //         return !(are_separate_convex(other) or other.are_separate_convex(*this));
    //     }

        bool overlaps(const ContigPoly& other) const{
            if(head==nullptr or other.head == nullptr)
                return false;

            // bool cond1 = edge_edge_intersection(other);
            // bool cond2 = node_node_intersection(other);
            // bool cond3 = edge_node_intersection(other);
            // bool cond4 = other.edge_node_intersection(*this);
            // bool cond5 = points_inside(other);
            // bool cond6 = other.points_inside(*this);

            // std::cout << "____\nedge edge: "  << cond1 << std::endl;
            // std::cout << "node node: "  << cond2 << std::endl;
            // std::cout << "edge node: "  << cond3 << std::endl;
            // std::cout << "node edge: "  << cond4 << std::endl;
            // std::cout << "points in1: " << cond5 << std::endl;
            // std::cout << "points in2: " << cond6 << std::endl;
            // return cond1 or cond2 or cond3 or cond4 or cond5 or cond6;

            // return edge_edge_intersection(other);
            return edge_edge_intersection(other) or node_node_intersection(other) 
            or edge_node_intersection(other) or other.edge_node_intersection(*this) 
                or points_inside(other) or other.points_inside(*this);
        }

        V* vertex_from_index(unsigned int index) {
            V *current = head;
            for(unsigned i=0; i<index; i++){
                current = current->next;
            }
            return current;
        }

        
        const V* vertex_from_index(unsigned int index) const {
            V *current = head;
            for(unsigned i=0; i<index; i++){
                current = current->next;
            }
            return current;
        }

        unsigned int get_obtusest_node() const {
            // Get the node with the largest internal opening
            
            // const V *largest = head;
            // unsigned index_largest = 0;
            

            // for(unsigned i = 0; i < size_ll; i++){
            //     const V* current = &vertices.at(i);
            //     if(largest->angle_opening < current->angle_opening){
            //         largest = current;
            //         index_largest = i;
            //     }
            // }
            // return index_largest;
            return 24346; // SIMAO fix
        }

        unsigned get_leftest_node() const {
            // Get the left-most node, then bottom-most
            // std::cout << "entered leftest\n";
            
            V *current = head;
            V *leftest = head;
            unsigned index_leftest = 0;
            for(unsigned i = 0; i < size_ll; i++){
                if(current->position.get_x() < leftest->position.get_x() or 
                    (current->position.get_x() == leftest->position.get_x() and 
                    current->position.get_y() < leftest->position.get_y())){

                        leftest = current;
                        index_leftest = i;
                }

                current = current->next;
            }
            // std::cout << "exited leftest\n";
            return index_leftest;
            
        }


        std::vector<unsigned int> get_all_nodes() const {
            
            std::vector<unsigned int> node_indices(size_ll);
            for(unsigned int i = 0; i < size_ll; i++){
                node_indices.at(i) = i;
            }
            return node_indices;
            
        }
        
        // SIMAO: pequeno refactor para aproveitar código de LLPoly aqui. Para agora está tudo repetido
        
        unsigned int get_farthest_node(int center_x, int center_y) const {
            // Get the node farthest from the center. If several nodes have the same distance
            // choose the bottom-most one
            
            V *current = head;
            V *farthest = head;
            unsigned int index_farthest = 0;
            Num farthest_d{-1};
            for(unsigned i = 0; i < size_ll; i++){
                Num dx = current->position.get_x() - Num(center_x);
                Num dy = current->position.get_y() - Num(center_y);
                Num d = dx*dx + dy*dy;

                if(d > farthest_d or 
                    (d == farthest_d and 
                    current->position.get_y() < farthest->position.get_y())){

                        farthest = current;
                        index_farthest = i;
                        farthest_d = d;
                }

                current = current->next;
            }
            return index_farthest;
        }

        V* get_head() const {
            return head;
        }

        unsigned int size() const {
            return size_ll;
        }

    //     std::vector<std::vector<double>> as_vector() const {
    //         std::vector<std::vector<double>> vector;
    //         V* current{head}; 
    //         for(unsigned i=0; i<size_ll; i++){
    //             double x = (double)current->position.get_x();
    //             double y = (double)current->position.get_y();
    //             std::vector<double> point{x,y};
    //             vector.push_back(point);
    //             current = current->next;
    //         }
            
    //         return vector;
    //     }



    //     std::vector<std::pair<double, double>> as_T() const {
    //         std::vector<std::pair<double, double>> vector;
    //         V* current{head}; 
    //         for(unsigned i=0; i<size_ll; i++){
    //             double x = (double)current->position.get_x();
    //             double y = (double)current->position.get_y();
    //             vector.push_back({x,y});
    //             current = current->next;
    //         }
            
    //         return vector;
    //     }
    };

    // template <typename Num>
    // std::ostream& operator<<(std::ostream& stream, ContigPoly<Num> const& poly){
    //     Vertex<Num> *ll_node = poly.get_head();
        
    //     for(unsigned i = 0; i < poly.size(); i++){
    //         stream << ll_node << "\n";
    //         ll_node = ll_node->next;
    //     }

    //     return stream;
    // }


} // namespace Polygon