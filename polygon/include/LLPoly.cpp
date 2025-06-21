#pragma once
#include <list>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include "points_vectors.cpp" // SIMAO: remover
#include "vertex.cpp"
#include "intersection_algorithms.cpp"
#include "BarePoly.cpp"

namespace Polygon {

    template <typename Num>
    class LLPoly{

    private:
        using Poin = Point<Num>;
        using V = Vertex<Num>;

        V *head;
        unsigned int size_ll;
        bool area_positive;

    public:
        using Ang = Angle<Num>;
        using VertexType = V;


        LLPoly():head{nullptr}, size_ll{0}, area_positive{false}{};
        LLPoly(unsigned int):LLPoly(){};
        LLPoly(const BarePoly& poly):LLPoly{poly.point_list}{};

        LLPoly(const std::vector<std::vector<int>>& points): head{nullptr}, size_ll{0}, area_positive{false}{
            // Turn the list of points into nodes, set them as the vertices of 
            // the polygon and calculate the angles between them

            if(points.size() == 0){
                return;
            }

            std::vector<int> last_point = points.back();
            int prev_x{last_point.at(0)}, prev_y{last_point.at(1)};
            V *current{};

            for(auto& point: points){
                
                int x{point.at(0)}, y{point.at(1)};
                Ang angle(x-prev_x, y-prev_y);
                
                V *ll_node = new V(Poin{prev_x,prev_y});
                ll_node->update_start(angle);

                if(size_ll == 0){
                    head = ll_node;
                    current = head;
                } else {
                    current->next = ll_node;
                    ll_node->prev = current;
                    current = ll_node;
                }
                size_ll++;

                prev_x = x;
                prev_y = y;
            }

            assert(current != nullptr);

            current->next = head;
            head->prev = current;
            head = head->next;

            current = head;
            V *prev = head->prev;
            for(unsigned i=0; i<size_ll; i++){
                current->update_end(-prev->angle_start);
                current->update_opening();
                prev = current;
                current = current->next;
            }

            area_positive = (get_area() > 0);
            
        }



        void build_this(const LLPoly& other_poly){

            area_positive = other_poly.area_positive;
            if(other_poly.size_ll == 0){
                size_ll = 0;
                head = nullptr;
                return;
            }

            head = new V(*(other_poly.head));
            V *node = head;        
            V *other_node = other_poly.head->next;

            for(unsigned i=1; i<other_poly.size_ll; i++){
                node->next = new V(*other_node);
                node->next->prev = node;

                other_node = other_node->next;
                node = node->next;

            }
            size_ll = other_poly.size_ll;
            node->next = head;
            head->prev = node;
        }

        void delete_this(){
            V *current, *next;
            current = head;
            for(unsigned i = 0; i < size_ll; i++){
                next = current->next;
                delete current;
                current = next;
            }
        }

        LLPoly(LLPoly const& other_poly){
            build_this(other_poly);
        }

        ~LLPoly(){
            delete_this();
        }

        void toggle_area_sign(){
            area_positive = !area_positive;
        }



        LLPoly& operator=(LLPoly const& other){
            if(&other == this)
                return *this;

            delete_this();
            build_this(other);
            return *this;
        }

        LLPoly(LLPoly&& other):
            head{other.head}, 
            size_ll{other.size_ll}, 
            area_positive{other.area_positive}{
        
            other.head = nullptr;
            other.size_ll = 0;
        
        }

        LLPoly& operator=(LLPoly&& other){
            if(this != &other){
                delete_this();

                head = other.head;
                size_ll = other.size_ll;
                area_positive = other.area_positive;

                other.head = nullptr;
                other.size_ll = 0;
        
            }
            return *this;
        }

        Num get_area(){
            std::vector<std::pair<Num, Num>> points;
            V *current = head;
            for(unsigned i=0; i<size_ll; i++){
                Num const& x = current->position.get_x();
                Num const& y = current->position.get_y();
                points.push_back({x,y});
                current = current->next;
            }

            return shoelace_area<Num>(points);
        }

        void translate(Poin const& dr){
            V *vertex = head;
            for(unsigned i=0; i<size_ll; i++){
                vertex->position = vertex->position + dr;
                vertex = vertex->next;
            }
        }

        void rotate(Ang const& angle, Poin const& rot_center){
            // Rotate the polygon by angle around the rotation center

            V *vertex = head;
            for(unsigned i=0; i<size_ll; i++){
                vertex->angle_end = vertex->angle_end + angle;
                vertex->angle_start = vertex->angle_start + angle;
                vertex->position = rot_center + (vertex->position - rot_center).rotate(angle);
                vertex = vertex->next;
            }
        }

        LLPoly copy_into(const V& vertex, unsigned int) const {
            // Copies the polygon into the specified vertex position

            LLPoly poly2(*this);
            poly2.translate(vertex.position - head->position);
            poly2.rotate(vertex.angle_start - head->angle_end, vertex.position);
            return poly2;
        }


        void move_into(const V& vertex) {
            // Moves the polygon into the specified vertex position

            translate(vertex.position - head->position);
            rotate(vertex.angle_start - head->angle_end, vertex.position);
        }



        void flip_x(){
            // Flip the polygon along x, but preserve orientation. In practice, 
            // this means changing the sign of every quantity which depends on x
            V *current{head}, *temp;

            for(unsigned i=0; i<size_ll; i++){
                temp = current->next;
                current->next = current->prev;
                current->prev = temp;
                current->position = {-current->position.get_x(), current->position.get_y()};

                auto start = current->angle_start;
                auto end = current->angle_end;
                current->angle_start = Ang{-end.get_cos(), end.get_sin()};
                current->angle_end = Ang{-start.get_cos(), start.get_sin()};
                
                current = temp;
            }
            
        }

        void advance_head() {
            head = head->next;
        }


        void flip_y(){
            // Flip the polygon along y, but preserve orientation. In practice, 
            // this means changing the sign of every quantity which depends on y
            V *current{head}, *temp;

            for(unsigned i=0; i<size_ll; i++){
                temp = current->next;
                current->next = current->prev;
                current->prev = temp;
                current->position = {current->position.get_x(), -current->position.get_y()};

                auto start = current->angle_start;
                auto end = current->angle_end;
                current->angle_start = Ang{end.get_cos(), -end.get_sin()};
                current->angle_end = Ang{start.get_cos(), -start.get_sin()};
                
                current = temp;
            }   
        }


        std::vector<V*> copy_merge(const LLPoly & other_poly, unsigned int anchor_index){
            
            LLPoly new_poly = other_poly;

            V* this_node = head;
            V* other_node = new_poly.vertex_from_index(anchor_index);

            // Connect the two linked lists
            V *temp = other_node->prev;
            
            other_node->prev = this_node->prev;
            other_node->prev->next = other_node;

            this_node->prev = temp;
            this_node->prev->next = this_node;

            // Delete the linked list from one of the polygons to 
            // avoid deleting twice in the destructor
            unsigned new_size = new_poly.size_ll + this->size_ll;
            this->size_ll = new_size;
            new_poly.size_ll = 0;
            new_poly.head = nullptr;

            // Update the angles
            this_node->angle_end = -this_node->prev->angle_start;
            other_node->angle_end = -other_node->prev->angle_start;

            this_node->update_opening();
            other_node->update_opening();
        
            
            return {this_node, other_node};
        }


        std::vector<V*> merge(V *this_node, LLPoly & other_poly, V *other_node){

            // Connect the two linked lists
            V *temp = other_node->prev;
            
            other_node->prev = this_node->prev;
            other_node->prev->next = other_node;

            this_node->prev = temp;
            this_node->prev->next = this_node;

            // Delete the linked list from one of the polygons to 
            // avoid deleting twice in the destructor
            unsigned new_size = other_poly.size_ll + this->size_ll;
            this->size_ll = new_size;
            other_poly.size_ll = 0;
            other_poly.head = nullptr;

            // Update the angles
            this_node->angle_end = -this_node->prev->angle_start;
            other_node->angle_end = -other_node->prev->angle_start;

            this_node->update_opening();
            other_node->update_opening();

            return {this_node, other_node};
        }

        void prune_LL(std::vector<V*> update, std::function<unsigned int(unsigned int)> getter){
            // Remove points from the LL which are coincident or have
            // an opening angle of 0. This is repeated until no more points
            // can be removed. It may happen that all points are removed, in
            // which case the head becomes a nullptr


            // std::cout << "Entering prune\nsize_ll: " << size_ll << "\n"; 
            // print();
            // V* c = head;
            // for(unsigned int i = 0; i < size_ll; i++){
            //     std::cout << c << " " << c->prev << " " << c->next << " " << c->position.get_x() << " " << c->position.get_y() << "\n";
            //     c = c->next;
            // }


            while(update.size()){
                
                if(size_ll == 1){
                    // std::cout << "-Mrking for removal ";
                    // head->print();
                    
                    delete head;
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

                    // std::cout << "marking for removal ";
                    // current->print();
                    
                    delete current;

                    size_ll--;
                    update.push_back(next);
                    update.push_back(prev);
                }   
            }
            // std::cout << "Left prune\nsize_ll: " << size_ll << "\n"; 
            // print();
            // c = head;
            // for(unsigned int i = 0; i < size_ll; i++){
            //     std::cout << c << " " << c->prev << " " << c->next << " " << c->position.get_x() << " " << c->position.get_y() << "\n";
            //     c = c->next;
            // }

        }



        // SIMAO: implementar isto?
        V& get_vertex([[maybe_unused]] unsigned int i) {
            return *head;
        }

        const V& get_vertex_not_mod([[maybe_unused]] unsigned int i) const {
            return *head;
        }

        std::vector<V> get_vertices() {
            return {*head};
        }

        const std::vector<V>& get_vertices() const {
            return {*head};
        }

        bool is_canonical() const {
            // SIMAO
            // Check if the polygon doesn't have 180 degrees, 0 degrees or coincident points
            // Optionally, I can also check if the angles make sense using doubles
            return false;
        }

        bool is_valid() const {
            return head != nullptr;
        }

        std::size_t get_hash() const {
            std::size_t h = 0;
            V *current = head;
            for(unsigned i=0; i<size_ll; i++){
                auto& pos = current->position;
                h += pos.get_x().get_hash()*4872191 + pos.get_y().get_hash()*991911983;
                h = h%230984193;
                current = current->next;
            }

            return h;
        }

        bool operator==(LLPoly const& other_poly) const{
            V *list1{this->head}, *list2{other_poly.head};

            if(size_ll != other_poly.size_ll)
                return false;

            // The polygons might be identical even though the heads are different
            for(unsigned i=0; i<size_ll; i++){

                if(list1->position == list2->position)
                    break;

                list2 = list2->next;
            }


            for(unsigned i=0; i<size_ll; i++){
                if(!(list1->position == list2->position))
                    return false;

                if(list1->angle_end != list2->angle_end)
                    return false;

                if(list1->angle_start != list2->angle_start)
                    return false;

                if(list1->angle_opening != list2->angle_opening)
                    return false;

                list1 = list1->next;
                list2 = list2->next;
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


        bool is_point_inside(Poin const& P, Poin const& Q = {100,101}) const{
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

        void connect_LL(){}

        bool edge_edge_intersection(const LLPoly& other) const {
            V *current{head};

            // Check for edge-edge intersections
            for(unsigned i=0; i<size_ll; i++){
                Poin& A = current->position;
                Poin& B = current->prev->position;
                V *other_current{other.head};

                for(unsigned j=0; j<other.size_ll; j++){
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

        bool edge_node_intersection(const LLPoly& other) const{
            // Check if an edge of 'this' polygon intersects a vertex of the 'other'

            V *current{head};
            for(unsigned i=0; i<size_ll; i++){

                Poin const& P = current->position;
                Poin const& Q = current->prev->position;

                V *other_current{other.head};
                for(unsigned j=0; j<other.size_ll; j++){

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

        bool node_node_intersection(const LLPoly& other) const{

            V *A{head};
            for(unsigned i=0; i<size_ll; i++){
                
                V *B{other.head};
                for(unsigned j=0; j<other.size_ll; j++){
                    
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

        bool points_inside(const LLPoly& other) const {
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

        bool are_separate_convex(const LLPoly& other) const {
            // Only strictly valid for convex polygons

            V* current = head;
            for(unsigned int i=0; i<size_ll; i++){
                V* next = current->next;
                const Point<Num>& C = current->position;
                const Point<Num>& N = next->position;

                Num dx = N.get_x() - C.get_x();
                Num dy = N.get_y() - C.get_y();
                
                V* other_current = other.head;
                bool separated = true;
                for(unsigned int j=0; j<other.size_ll; j++){
                    Point<Num> P = other_current->position - C;

                    // (dy, -dx) is (dx, dy) rotated 90º clockwise, 
                    // so it faces away from the polygon
                    Num proj = P.get_x()*dy - P.get_y()*dx;
                    
                    if(proj < 0){
                        separated = false;
                        break;
                    }

                    other_current = other_current->next;
                }
                
                if(separated)
                    return true;

                current = next;
            }
            // std::cout << count << "\n";
            return false;
        }

        bool convex_overlaps(const LLPoly& other) const {
            return !(are_separate_convex(other) or other.are_separate_convex(*this));
        }

        bool overlaps(const LLPoly& other) const{
            // std::cout << "inside overlaps\n";
            
            if(head==nullptr or other.head == nullptr)
                return false;

            // std::cout << "overlaps\nsize_ll: " << size_ll << "\n"; 
            // print();
            // V* c = head;
            // for(unsigned int i = 0; i < size_ll; i++){
            //     std::cout << c << " " << c->prev << " " << c->next << " " << c->position.get_x() << " " << c->position.get_y() << "\n";
            //     c = c->next;
            // }


            // std::cout << "overlaps\nsize_ll: " << other.size_ll << "\n"; 
            // other.print();
            // c = other.head;
            // for(unsigned int i = 0; i < other.size_ll; i++){
            //     std::cout << c << " " << c->prev << " " << c->next << " " << c->position.get_x() << " " << c->position.get_y() << "\n";
            //     c = c->next;
            // }

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
            bool has_overlap = edge_edge_intersection(other) or node_node_intersection(other) 
            or edge_node_intersection(other) or other.edge_node_intersection(*this) 
                or points_inside(other) or other.points_inside(*this);
            // std::cout << "left overlaps\n";
            return has_overlap;
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
            V *current = head;
            V *largest = head;
            unsigned index_largest = 0;
            for(unsigned i = 0; i < size_ll; i++){
                if(largest->angle_opening < current->angle_opening){
                    largest = current;
                    index_largest = i;
                }
                current = current->next;
            }
            return index_largest;
        }

        unsigned int get_leftest_node() const {
            // Get the left-most node, then bottom-most
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
            return index_leftest;
            
        }

        
        std::vector<unsigned int> get_all_nodes() const {
            
            std::vector<unsigned int> node_indices(size_ll);
            for(unsigned int i = 0; i < size_ll; i++){
                node_indices.at(i) = i;
            }
            return node_indices;
            
        }

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

        std::vector<std::vector<double>> as_vector() const {
            std::vector<std::vector<double>> vector;
            V* current{head}; 
            for(unsigned i=0; i<size_ll; i++){
                double x = (double)current->position.get_x();
                double y = (double)current->position.get_y();
                std::vector<double> point{x,y};
                vector.push_back(point);
                current = current->next;
            }
            
            return vector;
        }



        std::vector<std::pair<double, double>> as_T() const {
            std::vector<std::pair<double, double>> vector;
            V* current{head}; 
            for(unsigned i=0; i<size_ll; i++){
                double x = (double)current->position.get_x();
                double y = (double)current->position.get_y();
                vector.push_back({x,y});
                current = current->next;
            }
            
            return vector;
        }
    };

    template <typename Num>
    std::ostream& operator<<(std::ostream& stream, LLPoly<Num> const& poly){
        Vertex<Num> *ll_node = poly.get_head();
        
        for(unsigned i = 0; i < poly.size(); i++){
            stream << ll_node << "\n";
            ll_node = ll_node->next;
        }

        return stream;
    }


} // namespace Polygon