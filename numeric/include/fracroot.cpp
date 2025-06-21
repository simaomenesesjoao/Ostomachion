#pragma once
#include "fracroot.hpp"
#include <vector>

template <typename T, int... Ints>
std::pair<int, int> FracRoot<T, Ints...>::remove_perfect_squares(int r) const{
    // Check if there's any perfect square inside r
    std::vector<int> primes{2,3,5,7,11,13,17,19,23,29};

    int num{1}, root{1};

    for(auto& prime: primes){
        // std::cout << prime << " " << r << " " << num << " " << root <<  std::endl;

        if(prime > r) break;

        while(r % (prime*prime) == 0){
            r /= prime*prime;
            num *= prime;
        } 
        if(r % prime == 0){

            if(not ((prime == Ints) or ...)){
                throw std::invalid_argument("Square root argument cannot be represented with the factors provided.");                
            }

            root *= prime;
            r /= prime;
        }
    }

    if( r != 1){
        throw std::invalid_argument("Square root argument cannot be represented with the factors provided.");
    }
    return std::pair<int, int>(num, root);
}

template <typename T, int... Ints>
FracRoot<T, Ints...>::FracRoot():frac{0,1},root{1}{}

template <typename T, int... Ints>
FracRoot<T, Ints...>::FracRoot(Fraction<T> const& fraction, int r):frac{fraction},root{1}{

    auto [square, root2] = remove_perfect_squares(r);
    root = root2;
    T num = fraction.get_num()*(T)square;
    T den = fraction.get_den();
    
    frac = Fraction<T>(num, den);
}



template <typename T, int... Ints>
FracRoot<T, Ints...>::FracRoot(FracRoot&& other):
    frac{std::move(other.frac)}, root{other.root}{}

template <typename T, int... Ints>
FracRoot<T, Ints...>::FracRoot(const FracRoot& other):
    frac(other.frac), root{other.root}{}

template <typename T, int... Ints>
FracRoot<T, Ints...>& FracRoot<T, Ints...>::operator=(FracRoot&& other){
    if(this == &other)
        return *this;

    frac = std::move(other.frac);
    root = other.root;    
    return *this;

}

template <typename T, int... Ints>
FracRoot<T, Ints...>& FracRoot<T, Ints...>::operator=(const FracRoot& other){
    if(this == &other)
        return *this;
        
    frac = other.frac;
    root = other.root;  
    return *this;
}




template <typename T, int... Ints>
std::ostream& operator<<(std::ostream& os, FracRoot<T, Ints...> const & x){
    return os << x.frac.get_num() << "/" << x.frac.get_den() << "√" << x.root;
}

template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator*(FracRoot const & other) const{
    auto [square, root2] = remove_perfect_squares(root*other.root);
    T new_num = get_num()*other.get_num()*square;
    T new_den = get_den()*other.get_den();
    return FracRoot{{new_num, new_den}, root2};
}

template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator*(Fraction<T> const & x) const{
    return FracRoot{{get_num()*x.get_num(), get_den()*x.get_den()}, root};
}

template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator*(T x) const{
    return FracRoot{{get_num()*x, get_den()}, root};
}

template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator-() const{
    return FracRoot{-frac, root};
}


template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator/(FracRoot const & other) const{
    auto [square, root2] = remove_perfect_squares(root*other.root);
    int new_num = get_num()*other.get_den()*square;
    int new_den = get_den()*other.get_num()*other.get_root();
    return FracRoot{{new_num, new_den}, root2};
}

template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator+(FracRoot const& b) const{
    if(this->get_root() != b.get_root())
        throw std::invalid_argument("Square root argument must be identical when summing.");
    
    return FracRoot{{this->frac + b.frac}, this->get_root()};
}



template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator+(T b) const{
    if(this->get_root() != 1)
        throw std::invalid_argument("Square root argument must be identical when summing.");
    
    return FracRoot{{this->frac + Fraction<T>(b,1)}, this->get_root()};
}


template <typename T, int... Ints>
FracRoot<T, Ints...> FracRoot<T, Ints...>::operator-(FracRoot const & other) const{
    if(other.get_root() != get_root())
        throw std::invalid_argument("Square root argument must be identical when subtracting.");
    
    return FracRoot{{frac - other.frac}, get_root()};
}

// bool operator==(FracRoot x) const;
//     bool operator!=(FracRoot x) const;

// template <typename T, int... Ints>
// bool FracRoot<T, Ints...>::operator==(FracRoot x) const{
//     return x.get_num() == num and x.get_den() == den and x.get_root() == root;
// }

template <typename T, int... Ints>
bool FracRoot<T, Ints...>::operator==(T x) const{
    if(x==0){
        return x == get_num();
    } else {
        return x == get_num() and get_den()==1 and get_root()==1;
    }
}

template <typename T, int... Ints>
bool FracRoot<T, Ints...>::operator!=(T x) const{
    return !(*this==x);
}



template <typename T, int... Ints>
T FracRoot<T, Ints...>::get_num() const{ 
    return frac.get_num();
}

template <typename T, int... Ints>
T FracRoot<T, Ints...>::get_den() const{ 
    return frac.get_den(); 
}

template <typename T, int... Ints>
int FracRoot<T, Ints...>::get_root() const{
    return root;
}

