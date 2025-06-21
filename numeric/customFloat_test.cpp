#include "include/customFloat.cpp"
#include <iostream>
#include <iomanip>

template <typename T>
T operations(T const& x){
    T y = x;
    T z{-1.6};
    y = y + x;
    y += 1.3;
    z = z*y;
    y = y/x;
    y = y/2.0;
    y = y*z;
    y = y - x*0.999;

    if(y>0){
        y += 1;
    } else if(y<=1){
        y = y - x;
    } else {
        y = y*4;
    }


    return y;

}

int main(){

    {
        using F = Float<double>;
        F x{0}, y{0};
        x = x + 0.89442719099991597;
        y = y + -0.44721359549995798;


        std::cout << x << " " << (x == 1) << "\n";
        std::cout << y << " " << (y == 0) << "\n";
    }

    {
        using F = Float<float>;
        F x(0,1,2,3,4), y{3};
        auto out1 = operations(x).get();
        auto out2 = operations(x.get());

        std::cout << "identical? " << out1 << "  " << out2 << " " << out1-out2 << " " << (out1 == out2) << "\n";
    }


    // std::cout << "1: " << x << " " << y << "\n";

    // x = y + y;
    // y += x;
    // std::cout << "2: " << x << " " << y << "\n";
    
    // y = y/2;
    // y = y*2.3;
    // y = y*2;

    // x = x+1;
    // std::cout << "3: " << x << " " << y << "\n";

    // y = y*0 + 1.3;
    // x = y*0 + 1.30000001;
    // std::cout << "4: " << x << " " << y << "\n";

    // std::cout << "are equal: " << (y == x) << "\n";
    // std::cout << "are equal: " << (y == (x+1)) << "\n";

    // y = y*0 + 2.1;
    // std::cout << "5: " << x << " " << y << "\n";

    // std::cout << (y<2.2) << "\n";
    // std::cout << (y<2.0) << "\n";

    // std::cout << (y>2.2) << "\n";
    // std::cout << (y>2.0) << "\n";


    // std::cout << y << "\n";
}