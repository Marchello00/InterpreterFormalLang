#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include <iostream>
#include <vector>
#include <machine.h>
#include <enums.h>

class Node {
public:
    NodeType nodeType() const {
        return type_;
    }

    explicit Node(NodeType type) : type_(type) {}

    virtual ~Node() = default;

    virtual void print(int depth, std::ostream &out) = 0;

    virtual void evaluate(Machine &machine) {};

private:
    const NodeType type_;
};

class ExpressionNode : public Node {
public:
    explicit ExpressionNode(NodeType type = NodeType::EMPTY) :
            Node(type) {}

    void print(int depth, std::ostream &out) override {}

    void evaluate(Machine &machine) override {
        machine.push(TypeIdentifyer::INT_T);
        machine.top() = true;
    }
};

class CmdNode : public Node {
public:
    explicit CmdNode(Node *cmd) :
            Node(NodeType::COMMAND), cmd_(cmd) {}

    ~CmdNode() override {
        delete cmd_;
    }

    void setSimple() {
        simple_ = true;
    }

    void print(int depth, std::ostream &out) override {
        auto tab = std::string(depth, '\t');
        if (cmd_->nodeType() == NodeType::EMPTY) {
            return;
        }
        if (cmd_->nodeType() == NodeType::COMMAND_LIST) {
            out << tab << "{\n";
            cmd_->print(depth + 1, out);
            out << tab << "}\n";
        } else if (simple_) {
            out << tab;
            cmd_->print(0, out);
            out << ";\n";
        } else {
            cmd_->print(depth, out);
        }
    }

    void evaluate(Machine &machine) override {
        if (cmd_) {
            if (!simple_) {
                machine.enter_local_level();
            }
            cmd_->evaluate(machine);
            if (!simple_) {
                machine.leave_local_level();
            }
        }
    }

private:
    Node *cmd_;
    bool simple_ = false;
};

class CmdListNode : public Node {
public:
    CmdListNode() : Node(NodeType::COMMAND_LIST) {}

    explicit CmdListNode(CmdNode *cmd) : CmdListNode() {
        addCmd(cmd);
    }

    void addCmd(CmdNode *cmd) {
        cmds_.push_back(cmd);
    }

    ~CmdListNode() override {
        for (auto cmd : cmds_) {
            delete cmd;
        }
    }

    void print(int depth, std::ostream &out) override {
        for (auto cmd : cmds_) {
            cmd->print(depth, out);
        }
    }

    void evaluate(Machine &machine) override {
        for (auto cmd : cmds_) {
            cmd->evaluate(machine);
        }
    }

private:
    std::vector<CmdNode *> cmds_;
};

class TypeNode : public Node {
public:
    explicit TypeNode(TypeIdentifyer type_id) :
            Node(NodeType::TYPE), type_id_(type_id) {}

    void print(int depth, std::ostream &out) override {
        switch (type_id_) {
            case (TypeIdentifyer::INT_T): {
                out << "int";
                break;
            }
        }
    }

    TypeIdentifyer type() {
        return type_id_;
    }

private:
    TypeIdentifyer type_id_;
};

class ValueNode : public ExpressionNode {
public:
    ValueNode(std::string value, TypeNode *type_n) :
            ExpressionNode(NodeType::CONSTANT),
            value_(std::move(value)), type_n_(type_n) {}

    ~ValueNode() override {
        delete type_n_;
    }

    void print(int depth, std::ostream &out) override {
        out << value_;
    }

    void evaluate(Machine &machine) override {
        machine.push(type_n_->type());
        machine.top().pval() = std::make_shared<std::string>(value_);
    }

private:
    std::string value_;
    TypeNode *type_n_;
};

class IntValueNode : public ValueNode {
public:
    int value() const {
        return int_value_;
    }

    explicit IntValueNode(const std::string &value) :
            ValueNode(value, new TypeNode(TypeIdentifyer::INT_T)) {
        int_value_ = strtol(value.c_str(), nullptr, 10);
    }

    void evaluate(Machine &machine) override {
        machine.push(TypeIdentifyer::INT_T);
        machine.top() = int_value_;
    }

private:
    int int_value_;
};

class VariableNode : public ExpressionNode {
public:
    explicit VariableNode(std::string name) :
            ExpressionNode(NodeType::VARIABLE), name_(std::move(name)) {}

    std::string name() {
        return name_;
    }

    void print(int depth, std::ostream &out) override {
        out << name_;
    }

    void evaluate(Machine &machine) override {
        machine.push(machine.get(name_));
    }

private:
    std::string name_;
};

class OperatorNode : public ExpressionNode {
public:
    explicit OperatorNode(std::string oper = "") :
            ExpressionNode(NodeType::OPERATOR), oper_(std::move(oper)) {}

    void print(int depth, std::ostream &out) override {
        out << oper_;
    }

private:
    std::string oper_;
};

class BinaryOperator : public OperatorNode {
public:
    BinaryOperator(ExpressionNode *left, ExpressionNode *right,
                   const std::string &oper = "") :
            OperatorNode(oper), left_(left), right_(right) {}

    ~BinaryOperator() override {
        delete left_;
        delete right_;
    }

    void print(int depth, std::ostream &out) override {
        left_->print(depth, out);
        out << " ";
        OperatorNode::print(depth, out);
        out << " ";
        right_->print(depth, out);
    }

    void evaluate(Machine &machine) override {
        left_->evaluate(machine);
        right_->evaluate(machine);
    }

private:
    ExpressionNode *left_;
    ExpressionNode *right_;
};

class UnaryOperator : public OperatorNode {
public:
    explicit UnaryOperator(ExpressionNode *arg, const std::string &oper = "") :
            OperatorNode(oper), arg_(arg) {}

    ~UnaryOperator() override {
        delete arg_;
    }

    void print(int depth, std::ostream &out) override {
        OperatorNode::print(depth, out);
        arg_->print(depth, out);
    }

    void evaluate(Machine &machine) override {
        arg_->evaluate(machine);
    }

private:
    ExpressionNode *arg_;
};

class NotOperator : public UnaryOperator {
public:
    explicit NotOperator(ExpressionNode *arg) :
            UnaryOperator(arg, "!") {}

    void evaluate(Machine &machine) override {
        UnaryOperator::evaluate(machine);
        auto &val = machine.top();
        if (val.type() != TypeIdentifyer::INT_T) {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
        int val_v = *val;
        machine.pop();
        machine.push(TypeIdentifyer::INT_T);
        *machine.top() = !val_v;
    }
};

class UnaryMinusOperator : public UnaryOperator {
public:
    explicit UnaryMinusOperator(ExpressionNode *arg) :
            UnaryOperator(arg, "-") {}

    void evaluate(Machine &machine) override {
        UnaryOperator::evaluate(machine);
        auto &val = machine.top();
        if (val.type() != TypeIdentifyer::INT_T) {
            throw std::invalid_argument(NOT_INT);
        }
        int val_v = *val;
        machine.pop();
        machine.push(TypeIdentifyer::INT_T);
        *machine.top() = -val_v;
    }
};

class PlusOperator : public BinaryOperator {
public:
    PlusOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "+") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v + sval_v;
        } else {
            throw std::invalid_argument(NOT_INT);
        }
    }
};

class MinusOperator : public BinaryOperator {
public:
    MinusOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "-") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v - sval_v;
        } else {
            throw std::invalid_argument(NOT_INT);
        }
    }
};

class MultOperator : public BinaryOperator {
public:
    MultOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "*") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v * sval_v;
        } else {
            throw std::invalid_argument(NOT_INT);
        }
    }
};

class DivideOperator : public BinaryOperator {
public:
    DivideOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "/") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            if (*sval == 0) {
                throw std::runtime_error("Division by zero.");
            }
            *machine.top() = fval_v / sval_v;
        } else {
            throw std::invalid_argument(NOT_INT);
        }
    }
};

class AndOperator : public BinaryOperator {
public:
    AndOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "&&") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v && sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class OrOperator : public BinaryOperator {
public:
    OrOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "||") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v || sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class EqOperator : public BinaryOperator {
public:
    EqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "==") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v == sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class NotEqOperator : public BinaryOperator {
public:
    NotEqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "!=") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v != sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class LessOperator : public BinaryOperator {
public:
    LessOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "<") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v < sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class GrOperator : public BinaryOperator {
public:
    GrOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, ">") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v > sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class LessEqOperator : public BinaryOperator {
public:
    LessEqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "<=") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v <= sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class GrEqOperator : public BinaryOperator {
public:
    GrEqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, ">=") {}

    void evaluate(Machine &machine) override {
        BinaryOperator::evaluate(machine);
        auto &fval = machine.top(1);
        auto &sval = machine.top(0);
        if (fval.type() == TypeIdentifyer::INT_T &&
            sval.type() == TypeIdentifyer::INT_T) {
            int fval_v = *fval;
            int sval_v = *sval;
            machine.pop();
            machine.pop();
            machine.push(TypeIdentifyer::INT_T);
            *machine.top() = fval_v >= sval_v;
        } else {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
    }
};

class AssignOperator : public OperatorNode {
public:
    AssignOperator(const std::string &var_name, ExpressionNode *expression) :
            AssignOperator(new VariableNode(var_name), expression) {}

    AssignOperator(VariableNode *variable, ExpressionNode *expression) :
            OperatorNode("="), variable_(variable), expression_(expression) {}

    ~AssignOperator() override {
        delete variable_;
        delete expression_;
    }

    void print(int depth, std::ostream &out) override {
        variable_->print(depth, out);
        out << " ";
        OperatorNode::print(depth, out);
        out << " ";
        expression_->print(depth, out);
    }

    void evaluate(Machine &machine) override {
        expression_->evaluate(machine);
        auto &expr = machine.top();
        auto &var = machine.get(variable_->name());
        if (var.type() != expr.type()) {
            throw std::invalid_argument(VAR_NEQ_EXPR);
        }
        var = expr;
        machine.pop();
    }

private:
    VariableNode *variable_;
    ExpressionNode *expression_;
};

class CreateOperator : public OperatorNode {
public:
    CreateOperator(TypeNode *var_type, const std::string &var_name,
                   ExpressionNode *expression = nullptr) :
            CreateOperator(var_type, new VariableNode(var_name),
                           expression) {}

    CreateOperator(TypeIdentifyer type, const std::string &var_name,
                   ExpressionNode *expression = nullptr) :
            CreateOperator(new TypeNode(type), new VariableNode(var_name),
                           expression) {}

    CreateOperator(TypeNode *var_type, VariableNode *var,
                   ExpressionNode *expression = nullptr) :
            OperatorNode("="),
            var_type_(var_type), var_(var), expression_(expression) {}

    ~CreateOperator() override {
        delete var_type_;
        delete var_;
        delete expression_;
    }

    void print(int depth, std::ostream &out) override {
        var_type_->print(depth, out);
        out << " ";
        var_->print(depth, out);
        out << " ";
        OperatorNode::print(depth, out);
        out << " ";
        expression_->print(depth, out);
    }

    void evaluate(Machine &machine) override {
        machine.add(var_type_->type(), var_->name());
        if (expression_) {
            expression_->evaluate(machine);
            auto &expr = machine.top();
            auto &var = machine.get(var_->name());
            if (var.type() != expr.type()) {
                throw std::invalid_argument(VAR_NEQ_EXPR);
            }
            var = expr;
            machine.pop();
        }
    }

private:
    TypeNode *var_type_;
    VariableNode *var_;
    ExpressionNode *expression_;
};

class IfOperatorNode : public OperatorNode {
public:
    IfOperatorNode(ExpressionNode *condition,
                   CmdNode *true_branch,
                   CmdNode *false_branch = nullptr) :
            condition_(condition),
            true_branch_(true_branch),
            false_branch_(false_branch) {}

    ~IfOperatorNode() override {
        delete condition_;
        delete true_branch_;
        delete false_branch_;
    }

    void print(int depth, std::ostream &out) override {
        auto tab = std::string(depth, '\t');
        out << tab << "if (";
        condition_->print(depth, out);
        out << ")\n";
        true_branch_->print(depth, out);
        if (false_branch_) {
            out << tab << "else\n";
            false_branch_->print(depth, out);
        }
    }

    void evaluate(Machine &machine) override {
        condition_->evaluate(machine);
        auto &cond = machine.top();
        if (cond.type() != TypeIdentifyer::INT_T) {
            throw std::invalid_argument(NOT_BOOL_INT);
        }
        int cond_v = *cond;
        machine.pop();
        if (cond_v) {
            true_branch_->evaluate(machine);
        } else {
            if (false_branch_) {
                false_branch_->evaluate(machine);
            }
        }
    }

private:
    ExpressionNode *condition_;
    CmdNode *true_branch_;
    CmdNode *false_branch_;
};

class WhileOperatorNode : public OperatorNode {
public:
    WhileOperatorNode(ExpressionNode *condition, CmdNode *cmd) :
            condition_(condition), cmd_(cmd) {}

    ~WhileOperatorNode() override {
        delete condition_;
        delete cmd_;
    }

    void print(int depth, std::ostream &out) override {
        auto tab = std::string(depth, '\t');
        out << tab << "while (";
        condition_->print(depth, out);
        out << ")\n";
        cmd_->print(depth, out);
    }

    void evaluate(Machine &machine) override {
        while (true) {
            condition_->evaluate(machine);
            auto &cond = machine.top();
            if (cond.type() != TypeIdentifyer::INT_T) {
                throw std::invalid_argument(NOT_BOOL_INT);
            }
            int cond_v = *cond;
            machine.pop();
            if (!cond_v) {
                break;
            }
            cmd_->evaluate(machine);
        }
    }

private:
    ExpressionNode *condition_;
    CmdNode *cmd_;
};

class ForOperatorNode : public OperatorNode {
public:
    ForOperatorNode(ExpressionNode *init, ExpressionNode *condition, ExpressionNode *after, CmdNode *cmd) :
            init_(init), condition_(condition), after_(after), cmd_(cmd) {}

    ~ForOperatorNode() override {
        delete init_;
        delete condition_;
        delete after_;
        delete cmd_;
    }

    void print(int depth, std::ostream &out) override {
        auto tab = std::string(depth, '\t');
        out << tab << "for (";
        init_->print(depth, out);
        out << ";";
        condition_->print(depth, out);
        out << ";";
        after_->print(depth, out);
        out << ")\n";
        cmd_->print(depth, out);
    }

    void evaluate(Machine &machine) override {
        init_->evaluate(machine);
        while (true) {
            condition_->evaluate(machine);
            auto &cond = machine.top();
            if (cond.type() != TypeIdentifyer::INT_T) {
                throw std::invalid_argument(NOT_BOOL_INT);
            }
            int cond_v = *cond;
            machine.pop();
            if (!cond_v) {
                break;
            }
            cmd_->evaluate(machine);
            after_->evaluate(machine);
        }
    }

private:
    ExpressionNode *init_;
    ExpressionNode *condition_;
    ExpressionNode *after_;
    CmdNode *cmd_;
};


class ReadIntNode : public OperatorNode {
public:
    void print(int depth, std::ostream &out) override {
        out << "read_int()";
    }

    void evaluate(Machine &machine) override {
        machine.read_int();
    }

private:
};

class WriteNode : public OperatorNode {
public:
    explicit WriteNode(ExpressionNode *dst) : dst_(dst) {}

    ~WriteNode() override {
        delete dst_;
    }

    void print(int depth, std::ostream &out) override {
        out << "write(";
        dst_->print(depth, out);
        out << ")";
    }

    void evaluate(Machine &machine) override {
        dst_->evaluate(machine);
        machine.write();
    }

private:
    ExpressionNode *dst_;
};

class ExitNode : public OperatorNode {
public:
    void print(int depth, std::ostream &out) override {
        out << "exit()";
    }

    void evaluate(Machine &machine) override {
        exit(0);
    }
};

#endif // SYNTAX_TREE_H
