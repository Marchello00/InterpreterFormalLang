#include <utility>

#include <utility>

#include <utility>

#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include <iostream>
#include <vector>

enum class NodeType {
    OPERATOR,
    COMMAND,
    COMMAND_LIST,
    CONSTANT,
    VARIABLE,
    TYPE,
};

enum class TypeIdentifyer {
    INT_T,
};

class Node {
public:
    NodeType type() const {
        return type_;
    }

    explicit Node(NodeType type) : type_(type) {}

    virtual void print(int depth, std::ostream &out) {};

private:
    const NodeType type_;
};

class ExpressionNode : public Node {
public:
    explicit ExpressionNode(NodeType type) :
            Node(type) {}
};

class CmdNode : public Node {
public:
    explicit CmdNode(Node *cmd) :
            Node(NodeType::COMMAND), cmd_(cmd) {}

    void setSimple() {
        simple_ = true;
    }

    void print(int depth, std::ostream &out) override {
        auto tab = std::string(depth, '\t');
        if (cmd_->type() == NodeType::COMMAND_LIST) {
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

    void addCmd(Node *cmd) {
        cmds_.push_back(cmd);
    }

    ~CmdListNode() {
        for (auto cmd : cmds_) {
            delete cmd;
        }
    }

    void print(int depth, std::ostream &out) override {
        for (auto cmd : cmds_) {
            cmd->print(depth, out);
        }
    }

private:
    std::vector<Node *> cmds_;
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

private:
    TypeIdentifyer type_id_;
};

class ValueNode : public ExpressionNode {
public:
    ValueNode(std::string value, TypeNode *type_n) :
            ExpressionNode(NodeType::CONSTANT),
            value_(std::move(value)), type_n_(type_n) {}

    ~ValueNode() {
        delete type_n_;
    }

    void print(int depth, std::ostream &out) override {
        out << value_;
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

private:
    int int_value_;
};

class VariableNode : public ExpressionNode {
public:
    explicit VariableNode(std::string name) :
            ExpressionNode(NodeType::VARIABLE), name_(std::move(name)) {}

    void print(int depth, std::ostream &out) override {
        out << name_;
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

    ~BinaryOperator() {
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

private:
    ExpressionNode *left_;
    ExpressionNode *right_;
};

class UnaryOperator : public OperatorNode {
public:
    explicit UnaryOperator(ExpressionNode *arg, const std::string &oper = "") :
            OperatorNode(oper), arg_(arg) {}

    ~UnaryOperator() {
        delete arg_;
    }

    void print(int depth, std::ostream &out) override {
        OperatorNode::print(depth, out);
        arg_->print(depth, out);
    }

private:
    ExpressionNode *arg_;
};

class NotOperator : public UnaryOperator {
public:
    explicit NotOperator(ExpressionNode *arg) :
            UnaryOperator(arg, "!") {}
};

class UnaryMinusOperator : public UnaryOperator {
public:
    explicit UnaryMinusOperator(ExpressionNode *arg) :
            UnaryOperator(arg, "-") {}
};

class PlusOperator : public BinaryOperator {
public:
    PlusOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "+") {}
};

class MinusOperator : public BinaryOperator {
public:
    MinusOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "-") {}
};

class MultOperator : public BinaryOperator {
public:
    MultOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "*") {}
};

class DivideOperator : public BinaryOperator {
public:
    DivideOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "/") {}
};

class AndOperator : public BinaryOperator {
public:
    AndOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "&&") {}
};

class OrOperator : public BinaryOperator {
public:
    OrOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "||") {}
};

class EqOperator : public BinaryOperator {
public:
    EqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "==") {}
};

class NotEqOperator : public BinaryOperator {
public:
    NotEqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "!=") {}
};

class LessOperator : public BinaryOperator {
public:
    LessOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "<") {}
};

class GrOperator : public BinaryOperator {
public:
    GrOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, ">") {}
};

class LessEqOperator : public BinaryOperator {
public:
    LessEqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, "<=") {}
};

class GrEqOperator : public BinaryOperator {
public:
    GrEqOperator(ExpressionNode *left, ExpressionNode *right) :
            BinaryOperator(left, right, ">=") {}
};

class AssignOperator : public OperatorNode {
public:
    AssignOperator(const std::string &var_name, ExpressionNode *expression) :
            AssignOperator(new VariableNode(var_name), expression) {}

    AssignOperator(VariableNode *variable, ExpressionNode *expression) :
            OperatorNode("="), variable_(variable), expression_(expression) {}

    void print(int depth, std::ostream &out) override {
        variable_->print(depth, out);
        out << " ";
        OperatorNode::print(depth, out);
        out << " ";
        expression_->print(depth, out);
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

    void print(int depth, std::ostream &out) override {
        var_type_->print(depth, out);
        out << " ";
        var_->print(depth, out);
        out << " ";
        OperatorNode::print(depth, out);
        out << " ";
        expression_->print(depth, out);
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

    ~IfOperatorNode() {
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
        out << tab << "else\n";
        false_branch_->print(depth, out);
    }

private:
    ExpressionNode *condition_;
    CmdNode *true_branch_;
    CmdNode *false_branch_;
};

#endif // SYNTAX_TREE_H
