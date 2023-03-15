#include "nstd.h"
#include "tracker.h"
#include <stdio.h>
#include <vector>

template<bool is_optimized>
void my_swap(Tracker<std::vector<int>>& obj1, Tracker<std::vector<int>>& obj2);

template<>
void my_swap<false>(Tracker<std::vector<int>>& obj1, Tracker<std::vector<int>>& obj2) {
    SEMC_INIT(std::vector<int>)

    // x3 lvalue

    Tracker<std::vector<int>> tmp = obj1;
    obj1 = obj2;
    obj2 = tmp;

    SEMC_END(std::vector<int>)
}

template<>
void my_swap<true>(Tracker<std::vector<int>>& obj1, Tracker<std::vector<int>>& obj2) {

    // x3 rvalue 
    SEMC_INIT(std::vector<int>)
    
    Tracker<std::vector<int>> tmp = std::move(obj1);
    
    obj1 = std::move(obj2);
    obj2 = std::move(tmp);

    SEMC_END(std::vector<int>)
}

template<bool is_optimized>
void swap_show(){

    SEMC_INIT(std::vector<int>)

    Tracker<std::vector<int>> Rafik("Rafik");
    for(int i = 0; i < 100000; i++)
        Rafik.val().push_back(i);

    Tracker<std::vector<int>> Belyash("Belyash");
    for(int i = 0; i < 100000; i++)
        Belyash.val().push_back(i * 2);

    for(int i = 0; i < 1000 * 100; i++)
        my_swap<is_optimized>(Rafik, Belyash);

    SEMC_END(std::vector<int>)
}

int main(){

    #if (IS_OPTIMIZED == 0)
        swap_show<false>();
    #elif (IS_OPTIMIZED == 1)
        swap_show<true>();
    #endif 

    VizDumper<std::vector<int>>::GetInstance()->CloseGraph();
    VizDumper<std::vector<int>>::GetInstance()->SaveToFile("dot.dot");

    return 0;
}
