#ifndef INTERPRETER_MACHINE_H
#define INTERPRETER_MACHINE_H

#include <unordered_map>
#include <vector>
#include <enums.h>

class Value {
public:
    std::shared_ptr<void> &pval() {
        return val_;
    }

    Value &operator=(int val) {
        *std::static_pointer_cast<int>(val_) = val;
        return *this;
    }

    int &operator*() {
        return *std::static_pointer_cast<int>(val_);
    }

    TypeIdentifyer type() const {
        return type_;
    }

    Value() : type_(TypeIdentifyer::INT_T), val_(new int(0)) {}

    explicit Value(TypeIdentifyer type) :
            type_(type) {
        switch (type_) {
            case TypeIdentifyer::INT_T: {
                val_ = std::make_shared<int>(0);
                break;
            }
        }
    }

private:
    TypeIdentifyer type_;
    std::shared_ptr<void> val_;
};

class Machine {
    typedef int IndexT;
public:
    explicit Machine(std::istream &in = std::cin, std::ostream &out = std::cout) :
            in_(in), out_(out) {}

    void add(TypeIdentifyer type, const std::string &name) {
        if (vars_.find(name) != vars_.end()) {
            throw std::invalid_argument("Redefinition of variable " + name);
        }
        vars_[name] = Value(type);
    }

    Value &get(const std::string &name) {
        return vars_[name];
    }

    void push(const Value &val) {
        tmp_.push_back(val);
    }

    void push(TypeIdentifyer type = TypeIdentifyer::INT_T) {
        tmp_.emplace_back(type);
    }

    Value &top(IndexT k = 0) {
        if (tmp_.size() <= k) {
            throw std::overflow_error("Stack underflow");
        }
        return tmp_[tmp_.size() - k - 1];
    }

    void pop() {
        tmp_.pop_back();
    }

    Value &reg(IndexT num = 0) {
        return regs_[num];
    }

    void read_int() {
        int val;
        in_ >> val;
        push(TypeIdentifyer::INT_T);
        top() = val;
    }

    void write() {
        if (top().type() == TypeIdentifyer::INT_T) {
            int val = *top();
            out_ << val << "\n";
            pop();
        }
    }

private:
    std::unordered_map<std::string, Value> vars_;
    std::unordered_map<IndexT, Value> regs_;
    std::vector<Value> tmp_;

    std::istream &in_;
    std::ostream &out_;
};

#endif //INTERPRETER_MACHINE_H
