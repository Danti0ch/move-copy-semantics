#include "tracker.h"
/*
int main(){

    CREATE_AND_ASSIGNED_TRACKER(a, 0);
    CREATE_AND_ASSIGNED_TRACKER(b, 1);

    CREATE_TRACKER(c);
    c = a + b;
    VizDumper<CUR_T>::GetInstance()->CloseGraph();
    VizDumper<CUR_T>::GetInstance()->SaveToFile("dot.dot");
    return 0;

}*/

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

    CREATE_AND_ASSIGNED_TRACKER(d, a + b);

    //VizDumper<CUR_T>::GetInstance()->PasteText("wtf");
    VizDumper<int>::GetInstance()->CloseArea();
    
    VizDumper<CUR_T>::GetInstance()->CloseGraph();
    VizDumper<CUR_T>::GetInstance()->SaveToFile("dot.dot");

    return 0;
}

/*


template<class T>
Tracker<T> func(Tracker<T> a, Tracker<T> b)
{
    VizDumper<int>::GetInstance()->CreateArea(__PRETTY_FUNCTION__);

    Tracker<T> c = a + b;

    VizDumper<int>::GetInstance()->CloseArea();

    return c;
}

int main()
{

    CREATE_AND_ASSIGNED_TRACKER(a, 0);
    CREATE_AND_ASSIGNED_TRACKER(b, 1);
    CREATE_AND_ASSIGNED_TRACKER(c, 2);
    CREATE_AND_ASSIGNED_TRACKER(res, -1);

    res = c;
    res = func(a, b * c);

    VizDumper<CUR_T>::GetInstance()->PasteText("wtf");
    VizDumper<CUR_T>::GetInstance()->CloseGraph();
    VizDumper<CUR_T>::GetInstance()->SaveToFile("dot.dot");

    return 0;
}

*/