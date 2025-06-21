#include <vector>
#include <iostream>
#include <set>


template <typename T>
class Node{
public:
    T x, y, sq2;
    Node *prev, *next;

    Node(T x, T y): x{x}, y{y}, prev{nullptr}, next{nullptr}{};

    void append(Node* node){
        node->prev = this;
        next = node;
    }

    void update(){
        if(next){
            T dx = x - next->x;
            T dy = y - next->y;
            sq2 = dx*dx + dy*dy;
        }

        // if(next and prev){

        // }
    }
};

template <typename T>
class Shape{
public:
    Node<T> *head, *tail;

    Shape():head{nullptr}, tail{nullptr}{}

    template <typename... Args>
    Shape(Args... args):head{nullptr}, tail{nullptr}{
        (add(args), ...);
        update();
    }

    void add(std::vector<T> point){
        
        Node<T> *node = new Node<T>(point.at(0),point.at(1));
        if(!head){
            head = node;
        } else {
            tail->next = node;
            node->prev = tail;
            node->next = head;
        }
        tail = node;
    }

    void update(){
        Node<T> * node = head;
        head->update();
        node = node->next;
        
        while(node != head){
            node->update();
            node = node->next;   
        }

    }
};

int remove_perfect_squares(int n, int max=15){
    int prod_perf_sq = 1;
    for(int i = 2; i<=max; i++){

        while(n % (i*i) == 0){
            n /= i*i;
            prod_perf_sq *= i*i;
        }
        // std::cout << "i: " << i << " n" << n << "\n";

    }

    return n;

}

int main(){
    
    using T = int;
    using V = std::vector<T>;
    std::vector<Shape<T>> shapes;

    shapes.push_back({V{0,0},  V{3,0},  V{2,4}});
    shapes.push_back({V{3,0},  V{3,6},  V{2,4}});
    shapes.push_back({V{3,0},  V{6,0},  V{6,6},   V{4,8}, V{3,6}});
    shapes.push_back({V{0,0},  V{4,8},  V{2,10}});
    shapes.push_back({V{0,0},  V{2,10}, V{0,12}});
    shapes.push_back({V{0,12}, V{4,8},  V{6,6}});
    shapes.push_back({V{4,8},  V{6,6},  V{6,12}});
    shapes.push_back({V{6,0},  V{8,4},  V{6,6}});
    shapes.push_back({V{6,0},  V{12,0}, V{8,4}});
    shapes.push_back({V{8,4},  V{12,0}, V{9,6}});
    shapes.push_back({V{9,6},  V{12,0}, V{12,6}});
    shapes.push_back({V{9,6},  V{12,6}, V{12,8}});
    shapes.push_back({V{6,6},  V{8,4},  V{10,6},  V{6,12}});
    shapes.push_back({V{9,6},  V{12,8}, V{12,12}, V{6,12}});
    

    std::set<int> set;

    for(const auto& shape: shapes){
        Node<T> *node = shape.head;
        do{
            int sq2 = node->sq2;
            int root2 = remove_perfect_squares(sq2);
            set.insert(root2);
            // std::cout << sq2 << " " << root2 << "    ";

            std::cout << sq2 << " ";


            node = node->next;
        } while( node != shape.head);

        std::cout << std::endl;
    }

    // 2 5 13 17
    std::cout << "set: ";
    for(const auto& root2: set){
        std::cout << root2 << " ";
    }
    std::cout << std::endl;
    
}