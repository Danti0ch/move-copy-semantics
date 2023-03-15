#include <iostream>
#include "tracker.h"
#include "nstd.h"
#include <vector>

template<class T>
void my_swap_slow(T&& obj1, T&& obj2) {

    SEMC_INIT(std::vector<int>);

    typename nstd::remove_reference<T>::type tmp = nstd::forward<T>(obj1);
    obj1  = nstd::forward<T>(obj2);
    obj2  = nstd::forward<T>(tmp);

    SEMC_END(std::vector<int>);
}

template<class T>
void my_swap_fast(T&& obj1, T&& obj2) {

    SEMC_INIT(std::vector<int>);

    typename nstd::remove_reference<T>::type tmp = nstd::move<T>(obj1);
    obj1  = nstd::move<T>(obj2);
    obj2  = nstd::move<T>(tmp);

    SEMC_END(std::vector<int>);
}

int main() { 
    
    SEMC_INIT(std::vector<int>);

    Tracker<std::vector<int>> a("a");
    Tracker<std::vector<int>> b("b");

    for(int i = 0; i < 100000; i++)
        a.val().push_back(i);

    for(int i = 0; i < 100000; i++)
        b.val().push_back(i);

    my_swap_fast(a, b);

    SEMC_END(std::vector<int>);

    VizDumper<std::vector<int>>::GetInstance()->CloseGraph();
    VizDumper<std::vector<int>>::GetInstance()->SaveToFile("dot.dot");

    return 0;
}
