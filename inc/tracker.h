#include <assert.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

typedef int CUR_T;

const CUR_T POISON_CUR_T = (int)0xDEADBEEF;

template<typename T>
class VizDumper;

enum class VAR_TYPE{
    TEMP,
    NONTEMP
};

template<typename T>
class Tracker{
public:

    Tracker(const char* name);
    Tracker(const char* name, T val);
    Tracker(T val);

#ifdef ALLOW_COPY_NONCONST_SEMANTICS
    Tracker(const char* name, Tracker<T>& other);
    Tracker(Tracker<T>& other);
    Tracker& operator=(Tracker& other);
#endif 

#ifdef ALLOW_COPY_SEMANTICS
    Tracker(const char* name, const Tracker& other);
    Tracker(const Tracker<T>& other);
    Tracker& operator=(const Tracker& other);
#endif

#ifdef ALLOW_MOVE_SEMANTICS
    Tracker(const char* name, Tracker&& other);
    Tracker(Tracker<T>&& other);
    Tracker& operator=(Tracker&& other);
#endif

#ifdef ALLOW_MOVE_CONST_SEMANTICS
    Tracker(const char* name, const Tracker&& other);
    Tracker(const Tracker<T>&& other);
    Tracker& operator=(const Tracker&& other);
#endif

    ~Tracker();

    void operator+=(const Tracker& other);
    void operator-=(const Tracker& other);
    void operator*=(const Tracker& other);
    void operator/=(const Tracker& other);

    Tracker operator+(const Tracker& other) const ;
    Tracker operator-(const Tracker& other) const;
    Tracker operator*(const Tracker& other) const;
    Tracker operator/(const Tracker& other) const;

    T& val() { return var_; }

    static int last_n_node;
    int         n_node_;
private:
    T           var_;
    int         usage_cap_;
    std::string name_;
    VAR_TYPE    var_type_;
    //std::vector<oper_log<T>> backtrace_;

private:

    bool check_cap() const;
    bool decrease_cap();

    friend class VizDumper<T>;
};

#define CREATE_TRACKER(name)                     Tracker<CUR_T> name = Tracker<CUR_T>(#name)
#define CREATE_AND_ASSIGNED_TRACKER(name, val)   Tracker<CUR_T> name = Tracker<CUR_T>(#name, val)

//________________________________________________________________________________-

static int n_called_func = 0;

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

        data_ += "subgraph caption1 {\n"
                "node [shape=record];\n"        
                "label=\"...\"\n"
                "cps [label=\"N_COPY=" + std::to_string(n_copies_) + "\"]\n"
                "tmps [label=\"N_TMP=" + std::to_string(n_tmp_vars_) + "\"]\n"
                "}\n";

        data_ += "}\n";
    }

    void CreateEdge(int node1, int node2){
        data_ += std::to_string(node1) + " -> " + std::to_string(node2) + ";\n";
    }

    void CreateVar(Tracker<T>& var){
        const void * addr = static_cast<const void*>(&var);
        std::stringstream ss;
        ss << addr;  
        std::string addr_str = ss.str(); 

        
        data_ += "node [shape=record style=filled fillcolor=\"#fefdce\" label=\"{" + var.name_ + " | " +  addr_str + "}\"] v" + std::to_string(var.n_node_) + ";\n";
    }

    void CreateTmpVar(Tracker<T>& var){
        const void * addr = static_cast<const void*>(&var);
        std::stringstream ss;
        ss << addr;  
        std::string addr_str = ss.str(); 

        data_ += "node [shape=record style=filled fillcolor=\"#f8b90f\" label=\"{" + var.name_ + " | " +  addr_str + "}\"] v" + std::to_string(var.n_node_) + ";\n";
        n_tmp_vars_++;
    }

    /*
    void CreateVar(Tracker<int>& var){
        const void * addr = static_cast<const void*>(&var);
        std::stringstream ss;
        ss << addr;  
        std::string addr_str = ss.str(); 

        
        data_ += "node [shape=record style=filled fillcolor=\"#fefdce\" label=\"{" + var.name_ + " | " + std::to_string(var.var_) + " | " +  addr_str + "}\"] v" + std::to_string(var.n_node_) + ";\n";
    }

    void CreateTmpVar(Tracker<int>& var){
        const void * addr = static_cast<const void*>(&var);
        std::stringstream ss;
        ss << addr;  
        std::string addr_str = ss.str(); 

        data_ += "node [shape=record style=filled fillcolor=\"#f8b90f\" label=\"{" + var.name_ + " | " + std::to_string(var.var_) + " | " +  addr_str + "}\"] v" + std::to_string(var.n_node_) + ";\n";
    }
    */

    void CreateOper(int var1, int var2, int res, const char* name){
        data_ += "node [shape=record style=filled fillcolor=\"#ffc1ca\" label=\"" + std::string(name) + "\"] oper" + std::to_string(oper_n_node_) + ";\n";

        data_ += "v" + std::to_string(var1) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "v" + std::to_string(var2) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "oper" + std::to_string(oper_n_node_) + " -> v" + std::to_string(res) + ";\n";

        oper_n_node_++;
        cnst_node_++;
    }
    
    void CreateOperCt(int var, CUR_T ct, int res, const char* name){
        data_ += "node [shape=record style=filled fillcolor=pink label=\"" + std::string(name) + "\"] oper" + std::to_string(oper_n_node_) + ";\n";
        data_  += "node [shape=record label=\"" + std::to_string(ct) + "\"] c" + std::to_string(cnst_node_) + ";\n";

        data_ += "c" + std::to_string(cnst_node_) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "v" + std::to_string(var) + " -> oper" + std::to_string(oper_n_node_) + ";\n";
        data_ += "oper" + std::to_string(oper_n_node_) + " -> v" + std::to_string(res) + ";\n";

        oper_n_node_++;
        cnst_node_++;
    }
    
    void CreateMove(int other, int to){
        data_  += "node [label=\"move\" style=filled fillcolor=\"green\"] mv" + std::to_string(move_node_) + ";\n";

        data_ += "v" + std::to_string(other) + " -> mv" + std::to_string(move_node_) + ";\n";
        data_ += "mv" + std::to_string(move_node_) + " -> v" + std::to_string(to) + ";\n";
    
        move_node_++;
    }

    void CreateCopy(int other, int to){
        data_  += "node [label=\"copy\"  style=filled fillcolor=\"#DC143C\"] cp" + std::to_string(n_copies_) + ";\n";

        data_ += "v" + std::to_string(other) + " -> cp" + std::to_string(n_copies_) + ";\n";
        data_ += "cp" + std::to_string(n_copies_) + " -> v" + std::to_string(to) + ";\n";
    
        n_copies_++;
    }

    void CreateCopyCt(T other, int to){
        data_  += "node [label=\"copy\"  style=filled fillcolor=\"#DC143C\"] cp" + std::to_string(n_copies_) + ";\n";
        data_  += "node [shape=record label=\"" + std::to_string(other) + "\" style=filled fillcolor=\"white\"] c" + std::to_string(cnst_node_) + ";\n";

        data_ += "c" + std::to_string(cnst_node_) + " -> cp" + std::to_string(n_copies_) + ";\n";
        data_ += "cp" + std::to_string(n_copies_) + " -> v" + std::to_string(to) + ";\n";
    
        cnst_node_++;
        n_copies_++;
    }

    void CreateArea(const char* func_name){
        
        data_ += "subgraph cluster_" + std::to_string(n_sub_graph_) + "{\nlabel = \"[ " + std::to_string(n_called_func++) + " ] - " + std::string(func_name) + "\";\n"
                  "style=filled;\n"
                  "fillcolor=\"#00000010\";\n";
        n_sub_graph_++;
    }

    void PasteText(const std::string& text){
        data_  += "node [label=\"" + text + "\"  pos=\"100,0!\" style=filled fillcolor=\"green\"] txt\n";
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
    static int n_copies_;
    static int n_tmp_vars_;
};

template<typename T>
VizDumper<T>* VizDumper<T>::instance_ = NULL;

template<typename T>
int VizDumper<T>::n_sub_graph_ = 0;

template<typename T>
int VizDumper<T>::n_copies_ = 0;

template<typename T>
int VizDumper<T>::n_tmp_vars_ = 0;

template<typename T>
int VizDumper<T>::move_node_ = 0;

template<typename T>
int VizDumper<T>::cnst_node_ = 0;

template<typename T>
int VizDumper<T>::oper_n_node_ = 0;

template<typename T>
int Tracker<T>::last_n_node = 0;

const int USAGE_CAP_MAX = 4;
static int tmpVarNameCounter = 0;

//___________________________________________________________________________________________________________________________
//                                          COPY-MOVE SEMANTICS
//___________________________________________________________________________________________________________________________

// TODO: fix logs

#define SEMC_INIT(T)     VizDumper<T>::GetInstance()->CreateArea(__PRETTY_FUNCTION__);
#define SEMC_END(T)      VizDumper<T>::GetInstance()->CloseArea();

std::string generateTmpVarName(){
    std::string name = "tmp";
    name += std::to_string(tmpVarNameCounter++);

    return name;
}

template <typename T>
Tracker<T>::Tracker(const char* name):
    usage_cap_(USAGE_CAP_MAX),
    var_(),
    name_(name),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::NONTEMP)
{
    SEMC_INIT(T)
    VizDumper<T>::GetInstance()->CreateVar(*this);
    SEMC_END(T)
}

template <typename T>
Tracker<T>::Tracker(const char* name, T val):
    usage_cap_(USAGE_CAP_MAX),
    var_(val),
    name_(name),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::NONTEMP)
{
    SEMC_INIT(T)

    VizDumper<T>::GetInstance()->CreateVar(*this);

    SEMC_END(T)
}

template <typename T>
Tracker<T>::Tracker(T val):
    usage_cap_(USAGE_CAP_MAX),
    var_(val),
    name_(generateTmpVarName()),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::TEMP)
{
    SEMC_INIT(T)

    VizDumper<T>::GetInstance()->CreateTmpVar(*this);

    SEMC_END(T)
}

#ifdef ALLOW_COPY_SEMANTICS
template <typename T>
Tracker<T>::Tracker(const char* name, const Tracker<T>& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(other.var_),
    name_(name),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::NONTEMP)
{
    SEMC_INIT(T)
    VizDumper<T>::GetInstance()->CreateVar(*this);
    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);
    SEMC_END(T)
}

template <typename T>
Tracker<T>::Tracker(const Tracker<T>& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(other.var_),
    name_(generateTmpVarName()),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::TEMP)
{
    SEMC_INIT(T)
    VizDumper<T>::GetInstance()->CreateTmpVar(*this);
    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);
    SEMC_END(T)
}

template<typename T>
Tracker<T>& Tracker<T>::operator=(const Tracker& other){
    if(!decrease_cap()) return *this;
    SEMC_INIT(T)

    var_ = other.var_;
    n_node_ = last_n_node++;

    if(var_type_ == VAR_TYPE::TEMP) 
        VizDumper<T>::GetInstance()->CreateTmpVar(*this);
    else
        VizDumper<T>::GetInstance()->CreateVar(*this);

    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    SEMC_END(T)
    return *this;
}

#endif // ALLOW_COPY_SEMANTICS

#ifdef ALLOW_COPY_NONCONST_SEMANTICS

template <typename T>
Tracker<T>::Tracker(const char* name, Tracker<T>& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(other.var_),
    name_(name),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::NONTEMP)
{
    SEMC_INIT(T)

    VizDumper<T>::GetInstance()->CreateVar(*this);
    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    SEMC_END(T)
}

template <typename T>
Tracker<T>::Tracker(Tracker<T>& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(other.var_),
    name_(generateTmpVarName()),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::TEMP)
{
    SEMC_INIT(T)

    VizDumper<T>::GetInstance()->CreateVar(*this);
    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    SEMC_END(T)
}

template<typename T>
Tracker<T>& Tracker<T>::operator=(Tracker& other){
    if(!decrease_cap()) return *this;
    SEMC_INIT(T)

    var_ = other.var_;
    
    n_node_ = last_n_node++;

    if(var_type_ == VAR_TYPE::TEMP) 
        VizDumper<T>::GetInstance()->CreateTmpVar(*this);
    else
        VizDumper<T>::GetInstance()->CreateVar(*this);

    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    SEMC_END(T)
    return *this;
}

#endif

#ifdef ALLOW_MOVE_SEMANTICS

template <typename T>
Tracker<T>::Tracker(const char* name, Tracker<T>&& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(std::move(other.var_)),
    name_(name),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::NONTEMP)
{
    SEMC_INIT(T)
    other.var_ = POISON_CUR_T;

    VizDumper<T>::GetInstance()->CreateVar(*this);
    VizDumper<T>::GetInstance()->CreateMove(other.n_node_, n_node_);

    SEMC_END(T)
}

template <typename T>
Tracker<T>::Tracker(Tracker<T>&& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(std::move(other.var_)),
    name_(generateTmpVarName()),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::TEMP)
{
    SEMC_INIT(T)
    // other.var_ = POISON_CUR_T;

    VizDumper<T>::GetInstance()->CreateTmpVar(*this);
    VizDumper<T>::GetInstance()->CreateMove(other.n_node_, n_node_);

    SEMC_END(T)
}

template<typename T> \
Tracker<T>& Tracker<T>::operator=(Tracker<T>&& other){
    if(!decrease_cap()) return *this;
    SEMC_INIT(T)

    var_ = std::move(other.var_);

    n_node_ = last_n_node++;

    if(var_type_ == VAR_TYPE::TEMP) 
        VizDumper<T>::GetInstance()->CreateTmpVar(*this);
    else
        VizDumper<T>::GetInstance()->CreateVar(*this);

    VizDumper<T>::GetInstance()->CreateMove(other.n_node_, n_node_);

    SEMC_END(T)
    return *this;
}

#endif // ALLOW_MOVE_SEMANTICS

// TODO: проверить после прочтения статьи
#ifdef ALLOW_MOVE_CONST_SEMANTICS

template <typename T>
Tracker<T>::Tracker(const char* name, const Tracker&& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(other.var_),
    name_(name),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::NONTEMP)
{
    SEMC_INIT(T)
    //! other.var_ != POISON_CUR_T;

    VizDumper<T>::GetInstance()->CreateVar(*this);
    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    SEMC_END(T)
}

template <typename T>
Tracker<T>::Tracker(const Tracker&& other):
    usage_cap_(USAGE_CAP_MAX),
    var_(other.var_),
    name_(generateTmpVarName()),
    n_node_(last_n_node++),
    var_type_(VAR_TYPE::TEMP)
{
    SEMC_INIT(T)
    //! other.var_ != POISON_CUR_T;

    VizDumper<T>::GetInstance()->CreateTmpVar(*this);
    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    SEMC_END(T)
}


template<typename T>
Tracker<T>& Tracker<T>::operator=(const Tracker&& other){
    if(!decrease_cap()) return *this;
    SEMC_INIT(T)

    var_ = other.var_;

    n_node_ = last_n_node++;

    if(var_type_ == VAR_TYPE::TEMP) 
        VizDumper<T>::GetInstance()->CreateTmpVar(*this);
    else
        VizDumper<T>::GetInstance()->CreateVar(*this);

    VizDumper<T>::GetInstance()->CreateCopy(other.n_node_, n_node_);

    SEMC_END(T)
    return *this;
}
#endif 

template <typename T>
Tracker<T>::~Tracker(){
    // printf("%s w as killed\n", name_.c_str());
}

#define BIN_OPER_DEF(symb__)                          \
template<typename T> \
void Tracker<T>::operator symb__(const Tracker<T>& other){  \
    if(!decrease_cap()) return;                       \
                                                      \
    var_ symb__ other.var_;                           \
        \
    int cur_n_node = n_node_; \
    n_node_ = last_n_node++;  \
    last_n_node++;  \
}

// TODO: /*other.decrease_cap(); decrease_cap();*/
//     if(!decrease_cap()) return *this;                     
//    tmp_oper_log(tmp, *this, other, #symb__);            

#define BIN_RET_OPER_DEF(symb__)                          \
template<typename T> \
Tracker<T> Tracker<T>::operator symb__(const Tracker<T>& other) const {   \
    if(!(check_cap() && other.check_cap())) return *this; \
    SEMC_INIT(T) \
                                                          \
    Tracker<T> tmp = Tracker<T>(this->var_ symb__ other.var_);                    \
                                                          \
    \
    VizDumper<T>::GetInstance()->CreateOper(this->n_node_, other.n_node_, tmp.n_node_, #symb__); \
    SEMC_END(T) \
    return tmp;                                           \
}

BIN_OPER_DEF(+=)
BIN_OPER_DEF(-=)
BIN_OPER_DEF(*=)
BIN_OPER_DEF(/=)

BIN_RET_OPER_DEF(+)
BIN_RET_OPER_DEF(-)
BIN_RET_OPER_DEF(*)
BIN_RET_OPER_DEF(/)

#undef BIN_OPER_DEF
#undef BIN_RET_OPER_DEF

template<typename T>
bool Tracker<T>::check_cap() const{

    return true;
    assert(usage_cap_ >= 0);

    if(usage_cap_ == 0){
        printf("Integer %s max capacity riched, unable to perform operation\n", name_.c_str());
        return false;
    }
    
    return true;
}

template<typename T>
bool Tracker<T>::decrease_cap(){

    return true;
    
    if(!check_cap()) return false;

    usage_cap_--;
    return true;
}
