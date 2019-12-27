#ifndef INTERPRETER_MACHINE_H
#define INTERPRETER_MACHINE_H

#include <unordered_map>
#include <vector>
#include <enums.h>
#include <sstream>

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

    void load_str(const std::string &str) {
        val_ = std::make_shared<std::string>(str);
    }

    std::string &get_str() {
        return *std::static_pointer_cast<std::string>(pval());
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
            case TypeIdentifyer::STRING_T: {
                val_ = std::make_shared<std::string>();
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
            in_(in), out_(out) {
        local_.emplace_back();
    }

    void add(TypeIdentifyer type, const std::string &name) {
        if (vars_.find(name) != vars_.end()) {
            throw std::invalid_argument("Redefinition of variable " + name);
        }
        vars_[name] = Value(type);
        local_.back().push_back(name);
    }

    void enter_local_level() {
        local_.emplace_back();
    }

    void leave_local_level() {
        for (const auto &name : local_.back()) {
            vars_.erase(name);
        }
        local_.pop_back();
    }

    Value &get(const std::string &name) {
        if (vars_.find(name) == vars_.end()) {
            throw std::invalid_argument("No variable " + name);
        }
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

    void read_line() {
        std::string s;
        while (!getline(buffer_, s) && !is_eof_()) {
            buff_check_();
        }
        push(TypeIdentifyer::STRING_T);
        top().load_str(s);
    }

    void read_word() {
        std::string s;
        while (!(buffer_ >> s) && !is_eof_()) {
            buff_check_();
        }
        push(TypeIdentifyer::STRING_T);
        top().load_str(s);
    }

    void read_int() {
        int val = 0;
        while (!(buffer_ >> val) && !is_eof_()) {
            buff_check_();
        }
        push(TypeIdentifyer::INT_T);
        top() = val;
    }

    void write() {
        switch (top().type()) {
            case TypeIdentifyer::INT_T: {
                int val = *top();
                out_ << val;
                pop();
                break;
            }
            case TypeIdentifyer::STRING_T: {
                out_ << top().get_str();
                pop();
                break;
            }
        }
    }

    void write(const std::string &s) {
        push(TypeIdentifyer::STRING_T);
        top().load_str(s);
        write();
    }

private:
    std::unordered_map<std::string, Value> vars_;
    std::unordered_map<IndexT, Value> regs_;

    std::vector<Value> tmp_;

    std::vector<std::vector<std::string>> local_;
    std::istream &in_;

    std::ostream &out_;

    std::stringstream buffer_;

    bool eof_ = false;

    bool is_eof_() const {
        return eof_;
    }

    void read_buffer_() {
        std::string s;
        std::getline(in_, s);
        buffer_ << s;
        if (in_.eof()) {
            eof_ = true;
        }
    }

    void buff_check_() {
        if (buffer_.eof()) {
            buffer_.clear();
            read_buffer_();
        } else {
            buffer_.clear();
            throw std::invalid_argument(CANT_READ);
        }
    }
};

#endif //INTERPRETER_MACHINE_H
