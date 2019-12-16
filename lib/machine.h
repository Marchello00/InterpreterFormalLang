#ifndef INTERPRETER_MACHINE_H
#define INTERPRETER_MACHINE_H

#include <unordered_map>
#include <vector>
#include <enums.h>

class Value {
public:
    void *pval() {
        return val_;
    }

    Value() : type_(TypeIdentifyer::INT_T), val_(nullptr) {}

    ~Value() {
        switch (type_) {
            case TypeIdentifyer::INT_T: {
                delete (int *)(val_);
                break;
            }
        }
    }

    explicit Value(TypeIdentifyer type) :
            type_(type) {
        switch (type_) {
            case TypeIdentifyer::INT_T: {
                val_ = new int(0);
                break;
            }
        }
    }

private:
    TypeIdentifyer type_;
    void *val_;
};

class Machine {
    typedef int IndexT;
public:
    void addVar(TypeIdentifyer type, const std::string &name) {
        if (vars_.find(name) != vars_.end()) {
            throw std::invalid_argument("Redefinition of variable " + name);
        }
        vars_[name] = Value(type);
    }

    void push(TypeIdentifyer type) {
        tmp_.emplace_back(type);
    }

    Value &top(int k = 0) {
        if (tmp_.size() <= k) {
            throw std::overflow_error("Stack underflow");
        }
        return tmp_[tmp_.size() -  k - 1];
    }

    void pop() {
        tmp_.pop_back();
    }

private:
    std::unordered_map<std::string, Value> vars_;
    std::vector<Value> tmp_;
};

#endif //INTERPRETER_MACHINE_H
