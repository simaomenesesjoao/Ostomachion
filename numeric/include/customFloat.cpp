#include <ostream>
#include <iostream>
#include <cmath>

#define tol 1e-6

template <typename T>
class Float{
    
public:
    using Type = T;

// Constructors and assignment
    Float():x{0}{};
    template <typename... U> explicit Float(U... ys){
        x = (ys + ...);
    }

    Float(int n, int d, int r){ 
        x = (T)n/(T)d*sqrt(T(r));
    }

    Float(Float const& f):x{f.x}{};

    Float& operator=(Float const& f){ 
        x = f.x;
        return *this;
    }

    // Binary and unary operators
    template <typename U>
    Float  operator+(U other) const { 
        Float ff{*this};
        ff.x += (T)other;
        return ff;
    }
    template <typename U>
    Float& operator+=(U other){
        x += (U)other;
        return *this;
    }

    Float  operator+(Float const& other) const {
        Float ff{*this};
        ff.x += other.x;
        return ff;
    } 
    Float& operator+=(Float const& other){
        x += other.x;
        return *this;
    }
    Float  operator-() const{
        Float ff{*this};
        ff.x = -ff.x;
        return ff;
    }
    Float  operator-(Float const& other) const {
        Float ff{0};
        ff.x = x - other.x;
        return ff;
    }

    Float& operator-=(Float const& other){
        x -= other.x;
        return *this;
    }

    Float  operator*(Float const& other) const {
        Float ff;
        ff.x = x*other.x;
        return ff;
    }

    Float  operator/(Float const& other) const {
        Float ff;
        ff.x = x/other.x;
        return ff;        
    }
    Float  operator/(int other) const{
        Float ff{*this};
        ff.x /= (T)other;
        return ff;
    }

    template <typename U> Float operator*(U const& other) const {
        Float ff;
        ff.x = x*other;
        return ff;
    }

    // Comparison operators
    bool operator==(Float const& other) const {
        return fabs(x - other.x) < tol;
    }

    bool operator==(int other) const {
        return fabs(x - other) < tol;
    }

    template <typename U> bool operator!=(U const& other) const {
        return fabs(x - other) > tol;
    }

    bool operator>(Float const& other) const {
        return x > other.x + tol;
    }

    template <typename U> bool operator>(U other) const {
        return x > other + tol;
    }

    bool operator<(Float const& other) const {
        return x + tol < other.x;
    }

    template <typename U> bool operator<(U const& other) const {
        return x + tol < other;
    }

    // template <typename U> bool operator>=(U const& other) const {
    //     // x >= other 
    //     return !(other > x);
    // }

    // template <typename U> bool operator<=(U const& other) const {
    //     return !(other < x);
    // }


    template <typename U> bool operator>=(U const& other) const {
        return !(x + tol < other);
    }

    template <typename U> bool operator<=(U const& other) const {
        return !(x > other + tol);
    }


    template <typename U> explicit operator U() const {
        return (U)x;
    };
    

    template <typename U, int...Args>
    friend std::ostream& operator<<(std::ostream & os, Float<U> const& Float);

    T get(){
        return x;
    }

    std::size_t get_hash(){
        return (std::size_t)((x + 1e-6)*29318397);
    };

private:
    T x;
};

template <typename T, int... Ints>
std::ostream& operator<<(std::ostream & os, Float<T> const& ff){
    os << ff.x;
    return os;
}