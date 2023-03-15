#include "tracker.h"

template<class T>
Tracker<T> func(Tracker<T>& a, Tracker<T>& b)
{
    VizDumper<int>::GetInstance()->CreateArea(__PRETTY_FUNCTION__);

    CREATE_AND_ASSIGNED_TRACKER(c, a + b - 15);

    VizDumper<int>::GetInstance()->CloseArea();

    return c;
}

int main()
{
    VizDumper<int>::GetInstance()->CreateArea(__PRETTY_FUNCTION__);

    CREATE_AND_ASSIGNED_TRACKER(a, 0);
    CREATE_AND_ASSIGNED_TRACKER(b, 1);

    CREATE_AND_ASSIGNED_TRACKER(d, func(a, b));

    //VizDumper<CUR_T>::GetInstance()->PasteText("wtf");
    VizDumper<int>::GetInstance()->CloseArea();
    
    VizDumper<CUR_T>::GetInstance()->CloseGraph();
    VizDumper<CUR_T>::GetInstance()->SaveToFile("dot.dot");

    return 0;
}

/*
real    0m4,625s
user    0m3,298s
sys     0m0,130s
*/

/*
real    0m1,668s
user    0m0,636s
sys     0m0,112s
*/