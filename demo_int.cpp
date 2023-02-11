#include <assert.h>
#include <string>

typedef int T;
const int USAGE_CAP_MAX = 4;

int tmpVarNameCounter = 0;

#define CREATE_INT(name)        Integer name = Integer(#name)
#define CREATE_AND_ASSIGNED_INT(name, val)   Integer name = Integer(#name, val)

const char* generateTmpVarName(){
    std::string name = "tmp";
    name += std::to_string(tmpVarNameCounter++);

    return name.c_str();
}

class Integer{
public:
    Integer(const char* name):
        usage_cap_(USAGE_CAP_MAX),
        val_(),
        name_(name)
    {
        printf("%s created\n", name);
    }

    Integer(const char* name, T val):
        usage_cap_(USAGE_CAP_MAX),
        val_(val),
        name_(name)
    {
        printf("%s created and assigned with %d\n", name, val);
    }

    Integer(const char* name, const Integer& other):
        Integer(name)
    { 
        val_ = other.val_; 
        oper_log(other, "=");
    }

    ~Integer(){
        //printf("%s fired\n", name_.c_str());
    }

    void operator=(const Integer& other){
        if(!decrease_cap()) return;

        val_ = other.val_;
        oper_log(other, "=");
    }

    void operator+=(const Integer& other){
        if(!decrease_cap()) return;

        this->val_ += other.val_;
        oper_log(other, "+=");
    }

    void operator-=(const Integer& other){
        if(!decrease_cap()) return;

        this->val_ -= other.val_;
        oper_log(other, "-=");
    }
    
    void operator*=(const Integer& other){
        if(!decrease_cap()) return;

        this->val_ *= other.val_;
        oper_log(other, "*=");
    }
    
    void operator/=(const Integer& other){
        if(!decrease_cap()) return;

        this->val_ /= other.val_;
        oper_log(other, "/=");
    }

    Integer operator+(Integer& other){

        // TODO: copypaste
        if(!(check_cap() && other.check_cap())) return *this;
        decrease_cap(); other.decrease_cap();

        Integer tmp(generateTmpVarName());

        tmp = *this;
        tmp += other;

        // oper_log(other, "+");
        return tmp;
    }

    Integer operator-(Integer& other){
        if(!(check_cap() && other.check_cap())) return *this;
        decrease_cap(); other.decrease_cap();

        if(!decrease_cap()) return *this;

        Integer tmp(generateTmpVarName());

        tmp = *this;
        tmp -= other;

        // oper_log(other, "-");
        return tmp;
    }    

    Integer operator*(Integer& other){
        if(!(check_cap() && other.check_cap())) return *this;
        decrease_cap(); other.decrease_cap();

        if(!decrease_cap()) return *this;

        Integer tmp(generateTmpVarName());

        tmp = *this;
        tmp *= other;

        // oper_log(other, "*");
        return tmp;
    }    

    Integer operator/(Integer& other){
        if(!(check_cap() && other.check_cap())) return *this;
        decrease_cap(); other.decrease_cap();

        if(!decrease_cap()) return *this;

        Integer tmp(generateTmpVarName());

        tmp = *this;
        tmp /= other;

        // oper_log(other, "/");
        return tmp;
    }

private:
    T val_;
    int usage_cap_;
    std::string name_;

private:

    bool check_cap() const{
        assert(usage_cap_ >= 0);

        if(usage_cap_ == 0){
            printf("Integer %s max capacity riched, unable to perform operation\n", name_.c_str());
            return false;
        }
        return true;
    }

    bool decrease_cap(){
        if(!check_cap()) return false;

        usage_cap_--;
        return true;
    }

    void oper_log(const Integer& other_var, std::string oper_name){
        oper_log(*this, other_var, oper_name);
    }

    void oper_log(const Integer& var1, const Integer& var2, std::string oper_name){
        printf("%s %s %s\n", var1.name_.c_str(), oper_name.c_str(), var2.name_.c_str());
    }

};

int main(){

    CREATE_AND_ASSIGNED_INT(a, 5);
    CREATE_INT(b);
    CREATE_AND_ASSIGNED_INT(c, 12);

    b = a + c;
    CREATE_AND_ASSIGNED_INT(d, (a - c) / b);

    b = a + c;
    b = a + c;

    return 0;

}
