#include "class_testing.h"

int main(){

    CREATE_AND_ASSIGNED_TRACKER(a, 5);
    CREATE_AND_ASSIGNED_TRACKER(b, 5);

    a = 123;
    CREATE_TRACKER(c);
    c = a - 34 + b;
    VizDumper<CUR_T>::GetInstance()->CloseGraph();
    VizDumper<CUR_T>::GetInstance()->SaveToFile("dot.dot");
    return 0;

}
