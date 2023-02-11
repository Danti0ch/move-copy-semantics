#include <assert.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

typedef int CUR_T;

/*
struct OPER_CALL_LOG{
    const char obj_name[10];
};

// TODO: rename

template<typename T>
struct arg_info{
    Tracker<T>* p;
    int hist_pos;
};

template<typename T>
struct oper_log{
    std::string oper_name;
    //?
    std::vector<arg_info<T>> args;
    T res;
};
*/
template<typename T>
class VizDumper;

template<typename T>
class Tracker{
public:

    Tracker(const char* name);
    Tracker(const char* name, T val);
    Tracker(const char* name, const Tracker& other);
    Tracker(const Tracker<T>&& other);
    Tracker(const Tracker<T>& other);

    ~Tracker();

    Tracker& operator=(const Tracker& other);
    Tracker& operator=(const Tracker&& other);

    void operator+=(const Tracker& other);
    void operator-=(const Tracker& other);
    void operator*=(const Tracker& other);
    void operator/=(const Tracker& other);

    Tracker operator+(Tracker& other);
    Tracker operator-(Tracker& other);
    Tracker operator*(Tracker& other);
    Tracker operator/(Tracker& other);

    void operator+=(const CUR_T& other);
    void operator-=(const CUR_T& other);
    void operator*=(const CUR_T& other);
    void operator/=(const CUR_T& other);

    void operator=(const CUR_T& other);
    Tracker operator+(const CUR_T& other);
    Tracker operator-(const CUR_T& other);
    Tracker operator*(const CUR_T& other);
    Tracker operator/(const CUR_T& other);

    static int last_n_node; //! init
    int         n_node_;
private:
    T           var_;
    int         usage_cap_;
    std::string name_;
    //std::vector<oper_log<T>> backtrace_;

private:

    bool check_cap() const;
    bool decrease_cap();
    void oper_log(const Tracker& var1, const Tracker& var2, std::string oper_name);
    void oper_log(const Tracker& var2, std::string oper_name);
    void oper_log(const Tracker& var1, const CUR_T& ct, std::string oper_name);
    void oper_log(const CUR_T& ct, std::string oper_name);
    void tmp_oper_log(const Tracker<T>& tmp_var, const Tracker<T>& var1, const Tracker<T>& var2, std::string oper_name);
    void tmp_oper_log(const Tracker<T>& tmp_var, const Tracker<T>& var, const CUR_T& ct, std::string oper_name);

    friend class VizDumper<T>;
};

#define CREATE_TRACKER(name)                     Tracker<CUR_T> name = Tracker<CUR_T>(#name)
#define CREATE_AND_ASSIGNED_TRACKER(name, val)   Tracker<CUR_T> name = Tracker<CUR_T>(#name, val)

template<typename T>
class VizDumper{
public:
    VizDumper():
        data_(),
        print_func_(NULL)
    {}

    VizDumper(void (*print_func)(const T&)):
        data_(),
        print_func_(print_func)
    { }  

    ~VizDumper(){
        delete instance_;
    }

    void InitGraph(){
        data_ += "digraph G{\n";
    }

    void CloseGraph(){
        data_ += "}\n";
    }

    void CreateEdge(int node1, int node2){
        data_ += std::to_string(node1) + " -> " + std::to_string(node2) + ";\n";
    }

    void CreateVar(Tracker<T>& var){
        // TODO: more info
        const void * addr = static_cast<const void*>(&var);
        std::stringstream ss;
        ss << addr;  
        std::string addr_str = ss.str(); 

        data_ += "node [shape=record style=filled fillcolor=\"white\" label=\"{" + var.name_ + " | " + std::to_string(var.var_) + " | " +  addr_str + "}\"] v" + std::to_string(var.n_node_) + ";\n";
    }

    void CreateOper(const int& var1, const int& var2, const int& res, const char* name){
        data_ += "node [shape=record label=\"" + std::string(name) + "\"] oper" + std::to_string(oper_n_node_) + ";\n";

        data_ += "v" + std::to_string(var1) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "v" + std::to_string(var2) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "oper" + std::to_string(oper_n_node_) + " -> v" + std::to_string(res) + ";\n";

        oper_n_node_++;
        cnst_node_++;
    }
    
    void CreateOperCt(const int& var, const CUR_T& ct, const int& res, const char* name){
        data_ += "node [shape=record label=\"" + std::string(name) + "\"] oper" + std::to_string(oper_n_node_) + ";\n";
        data_  += "node [shape=record label=\"" + std::to_string(ct) + "\"] c" + std::to_string(cnst_node_) + ";\n";

        data_ += "c" + std::to_string(cnst_node_) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "v" + std::to_string(var) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "oper" + std::to_string(oper_n_node_) + " -> v" + std::to_string(res) + ";\n";

        oper_n_node_++;
        cnst_node_++;
    }
    
    void CreateMove(const int& other, const int& to){
        data_  += "node [label=\"move\"] mv" + std::to_string(move_node_) + ";\n";

        data_ += "v" + std::to_string(other) + " -> mv" + std::to_string(move_node_) + ";\n";
        data_ += "mv" + std::to_string(move_node_) + " -> v" + std::to_string(to) + ";\n";
    
        move_node_++;
    }

    void CreateCopy(const int& other, const int& to){
        data_  += "node [label=\"copy\"] cp" + std::to_string(move_node_) + ";\n";

        data_ += "v" + std::to_string(other) + " -> cp" + std::to_string(move_node_) + ";\n";
        data_ += "cp" + std::to_string(move_node_) + " -> v" + std::to_string(to) + ";\n";
    
        move_node_++;
    }

    void CreateArea(const char* func_name){

        data_ += "subgraph sub" + std::to_string(n_sub_graph_) + "{\nlabel = \"" + std::string(func_name) + "\";\n"
                  "style=filled;\n"
                  "color=lightgreen;\n"
                  "fillcolor=\"darkgray:gold\";\n\n";
        n_sub_graph_++;
    }

    void CloseArea(){
        data_ += "}\n\n";
    }

    static VizDumper* GetInstance(){
        if(instance_ == NULL){
            instance_ = new VizDumper<T>();
            instance_->InitGraph();
            n_sub_graph_ = 0;
        }

        return instance_;
    }

    void SaveToFile(const char* filename){

        std::ofstream to; to.open(filename);
        to << data_.c_str();
    }

private:
    std::string data_;
    void (*print_func_)(const T&);

    static VizDumper* instance_;
    static int cnst_node_ ;
    static int n_sub_graph_;
    static int oper_n_node_;
    static int move_node_;
};

template<typename T>
VizDumper<T>* VizDumper<T>::instance_ = NULL;

template<typename T>
int VizDumper<T>::n_sub_graph_ = 0;

template<typename T>
int VizDumper<T>::move_node_ = 0;

template<typename T>
int VizDumper<T>::cnst_node_ = 0;

template<typename T>
int VizDumper<T>::oper_n_node_ = 0;

template<typename T>
int Tracker<T>::last_n_node = 0;

//________________________________________________________________________________________________________________________________

const int USAGE_CAP_MAX = 4;
int tmpVarNameCounter = 0;

const char* generateTmpVarName(){
    std::string name = "tmp";
    name += std::to_string(tmpVarNameCounter++);

    return name.c_str();
}

template <typename T>
Tracker<T>::Tracker(const char* name):
    usage_cap_(USAGE_CAP_MAX),
    var_(),
    name_(name),
    n_node_(last_n_node++)
{
    // VizDumper<int>::GetInstance()->CreateArea(__func__);
    printf("%s created\n", name);
    VizDumper<int>::GetInstance()->CreateVar(*this);
    // VizDumper<int>::GetInstance()->CloseArea();
}

template <typename T>
Tracker<T>::Tracker(const char* name, T val):
    usage_cap_(USAGE_CAP_MAX),
    var_(val),
    name_(name),
    n_node_(last_n_node++)
{
    // VizDumper<int>::GetInstance()->CreateArea(__func__);
    printf("%s created and assigned with %d\n", name, val);
    VizDumper<int>::GetInstance()->CreateVar(*this);
    // VizDumper<int>::GetInstance()->CloseArea();
}

template <typename T>
Tracker<T>::Tracker(const char* name, const Tracker<T>& other):
    Tracker(name)
{
    // VizDumper<int>::GetInstance()->CreateArea(__func__);
    var_ = other.var_; 
    oper_log(other, "=");
    VizDumper<int>::GetInstance()->CreateVar(*this);
    // VizDumper<int>::GetInstance()->CloseArea();
}

template <typename T>
Tracker<T>::Tracker(const Tracker<T>&& other):
    Tracker(other.name_.c_str(), other.var_)
{
    // VizDumper<int>::GetInstance()->CreateArea(__func__);
    oper_log(other, "=");
    VizDumper<int>::GetInstance()->CreateVar(*this);
    VizDumper<int>::GetInstance()->CreateMove(other.n_node_, n_node_);
    // VizDumper<int>::GetInstance()->CloseArea();
}

template <typename T>
Tracker<T>::Tracker(const Tracker<T>& other):
    Tracker(other.name_.c_str(), other.var_)
{
    // VizDumper<int>::GetInstance()->CreateArea(__func__);
    oper_log(other, "=");
    VizDumper<int>::GetInstance()->CreateVar(*this);
    VizDumper<int>::GetInstance()->CreateCopy(other.n_node_, n_node_);
    // VizDumper<int>::GetInstance()->CloseArea();
}

template <typename T>
Tracker<T>::~Tracker(){
    printf("%s was killed\n", name_.c_str());
}
#include <iostream>
#define BIN_OPER_DEF(symb__)                          \
template<typename T> \
void Tracker<T>::operator symb__(const Tracker<T>& other){  \
    if(!decrease_cap()) return;                       \
                                                      \
    var_ symb__ other.var_;                           \
    oper_log(other, #symb__);                         \
        \
    int cur_n_node = n_node_; \
    n_node_ = last_n_node++;  \
    VizDumper<T>::GetInstance()->CreateVar(*this);  \
    VizDumper<T>::GetInstance()->CreateOper(cur_n_node, other.n_node_, this->n_node_, #symb__); \
    last_n_node++;  \
}

#define BIN_RET_OPER_DEF(symb__)                          \
template<typename T> \
Tracker<T> Tracker<T>::operator symb__(Tracker<T>& other){   \
    if(!(check_cap() && other.check_cap())) return *this; \
    decrease_cap(); other.decrease_cap();                 \
                                                          \
    if(!decrease_cap()) return *this;                     \
                                                          \
    Tracker<T> tmp(generateTmpVarName());                    \
                                                          \
    tmp.var_ = this->var_ symb__ other.var_;                 \
    tmp_oper_log(tmp, *this, other, #symb__);              \
    \
    VizDumper<T>::GetInstance()->CreateOper(this->n_node_, other.n_node_, tmp.n_node_, #symb__); \
                                                          \
    return tmp;                                           \
}

#define BIN_CT_OPER_DEF(symb__)                          \
template<typename T> \
void Tracker<T>::operator symb__(const CUR_T& other){  \
    if(!decrease_cap()) return;                       \
                                                      \
    var_ symb__ other;                           \
    oper_log(other, #symb__);                         \
    \
    int cur_n_node = n_node_; \
    n_node_ = last_n_node++;  \
    VizDumper<T>::GetInstance()->CreateVar(*this);  \
    VizDumper<T>::GetInstance()->CreateOperCt(cur_n_node, other, this->n_node_, #symb__); \
}

#define BIN_RET_CT_OPER_DEF(symb__)                          \
template<typename T> \
Tracker<T> Tracker<T>::operator symb__(const CUR_T& other){   \
    if(!(check_cap())) return *this; \
    decrease_cap();                      \
                                                          \
    if(!decrease_cap()) return *this;                     \
                                                          \
    Tracker<T> tmp(generateTmpVarName());                    \
                                                          \
    tmp.var_ = this->var_ symb__ other;                 \
    tmp_oper_log(tmp, *this, other, #symb__);                      \
                                                          \
    VizDumper<T>::GetInstance()->CreateOperCt(this->n_node_, other, tmp.n_node_, #symb__); \
    return tmp;                                           \
}

template<typename T> \
Tracker<T>& Tracker<T>::operator=(const Tracker& other){
    if(!decrease_cap()) return *this;

    var_ = other.var_;
    oper_log(other, "copy");
    
    VizDumper<int>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    return *this;
}

template<typename T> \
Tracker<T>& Tracker<T>::operator=(const Tracker&& other){
    if(!decrease_cap()) return *this;

    var_ = other.var_;
    oper_log(other, "copy");
    
    VizDumper<int>::GetInstance()->CreateMove(other.n_node_, n_node_);

    return *this;
}

BIN_OPER_DEF(+=)
BIN_OPER_DEF(-=)
BIN_OPER_DEF(*=)
BIN_OPER_DEF(/=)

BIN_RET_OPER_DEF(+)
BIN_RET_OPER_DEF(-)
BIN_RET_OPER_DEF(*)
BIN_RET_OPER_DEF(/)

BIN_CT_OPER_DEF(=)
BIN_CT_OPER_DEF(+=)
BIN_CT_OPER_DEF(-=)
BIN_CT_OPER_DEF(*=)
BIN_CT_OPER_DEF(/=)

BIN_RET_CT_OPER_DEF(+)
BIN_RET_CT_OPER_DEF(-)
BIN_RET_CT_OPER_DEF(*)
BIN_RET_CT_OPER_DEF(/)

#undef BIN_CT_OPER_DEF
#undef BIN_RET_CT_OPER_DEF

#undef BIN_OPER_DEF
#undef BIN_RET_OPER_DEF

template<typename T>
bool Tracker<T>::check_cap() const{
    assert(usage_cap_ >= 0);

    if(usage_cap_ == 0){
        printf("Integer %s max capacity riched, unable to perform operation\n", name_.c_str());
        return false;
    }
    return true;
}

template<typename T>
bool Tracker<T>::decrease_cap(){
    if(!check_cap()) return false;

    usage_cap_--;
    return true;
}

template<typename T>
void Tracker<T>::oper_log(const Tracker<T>& var1, const Tracker<T>& var2, std::string oper_name){
    printf("%s %s %s\n", var1.name_.c_str(), oper_name.c_str(), var2.name_.c_str());
}

template<typename T>
void Tracker<T>::oper_log(const Tracker<T>& var2, std::string oper_name){
    printf("%s %s %s\n", this->name_.c_str(), oper_name.c_str(), var2.name_.c_str());
}

template<typename T>
void Tracker<T>::oper_log(const Tracker<T>& var1, const CUR_T& ct, std::string oper_name){
    printf("%s %s %s\n", var1.name_.c_str(), oper_name.c_str(), std::to_string(ct).c_str());
}

template<typename T>
void Tracker<T>::oper_log(const CUR_T& ct, std::string oper_name){
    printf("%s %s %s\n", this->name_.c_str(), oper_name.c_str(), std::to_string(ct).c_str());
}

template<typename T>
void Tracker<T>::tmp_oper_log(const Tracker<T>& tmp_var, const Tracker<T>& var1, const Tracker<T>& var2, std::string oper_name){
    printf("%s = %s %s %s\n", tmp_var.name_.c_str(), var1.name_.c_str(), oper_name.c_str(), var2.name_.c_str());
}

template<typename T>
void Tracker<T>::tmp_oper_log(const Tracker<T>& tmp_var, const Tracker<T>& var, const CUR_T& ct, std::string oper_name){
    printf("%s = %s %s %s\n", tmp_var.name_.c_str(), var.name_.c_str(), oper_name.c_str(), std::to_string(ct).c_str());
}
