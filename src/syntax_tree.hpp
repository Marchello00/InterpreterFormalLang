#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include <iostream>
#include <vector>

enum class NodeType {
    OPERATOR,
    OPERATOR_LIST,
    INTEGER_VARIABLE,
    INTEGER_CONSTANT,
};

class Node {
public:
    NodeType type() const {
        return type_;
    }

    Node(NodeType type): type_(type) {}
private:
    const NodeType type_;
};

class ExpressionNode: public Node {
public:
    ExpressionNode(NodeType type):
        Node(type) {}
};

class NodesListNode: public Node {
public:
    NodesListNode(): Node(NodeType::OPERATOR_LIST) {}

    void addNode(Node *node) {
        nodes_.push_back(node);
    }

    ~NodesListNode() {
        for (auto node : nodes_) {
            delete node;
        }
    }
private:
    std::vector<Node *> nodes_;
};

class IntValueNode: public ExpressionNode {
public:
    int value() const {
        return value_;
    }

    IntValueNode(int value = 0):
        ExpressionNode(NodeType::INTEGER_CONSTANT), value_(value) { }
private:
    int value_;
};

class IntVariableNode: public ExpressionNode {
public:
    IntVariableNode(const char *name):
        ExpressionNode(NodeType::INTEGER_VARIABLE), name_(name) {}
private:
    const char *name_;
};

class OperatorNode: public ExpressionNode {
public:
    OperatorNode(const char *oper = NULL):
        ExpressionNode(NodeType::OPERATOR), oper_(oper) {}
private:
    const char *oper_;
};

class BinaryOperator: public OperatorNode {
public:
    BinaryOperator(ExpressionNode *left, ExpressionNode *right,
                    const char *oper = NULL):
        OperatorNode(oper), left_(left), right_(right) {}

    ~BinaryOperator() {
        delete left_;
        delete right_;
    }
private:
    ExpressionNode *left_;
    ExpressionNode *right_;
};

class UnaryOperator: public OperatorNode {
public:
    UnaryOperator(ExpressionNode *arg, const char *oper = NULL):
        OperatorNode(oper), arg_(arg) {}

    ~UnaryOperator() {
        delete arg_;
    }
private:
    ExpressionNode *arg_;
};

class NotOperator: public UnaryOperator {
    NotOperator(ExpressionNode *arg):
        UnaryOperator(arg, "!") {}
};

class UnaryMinusOperator: public UnaryOperator {
    UnaryMinusOperator(ExpressionNode *arg):
        UnaryOperator(arg,  "-") {}
};

class PlusOperator: public BinaryOperator {
public:
    PlusOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "+") {}
};

class MinusOperator: public BinaryOperator {
public:
    MinusOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "-") {}
};

class MultOperator: public BinaryOperator {
public:
    MultOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "*") {}
};

class DivideOperator: public BinaryOperator {
public:
    DivideOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "/") {}
};

class AndOperator: public BinaryOperator {
public:
    AndOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "&&") {}
};

class OrOperator: public BinaryOperator {
public:
    OrOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "||") {}
};

class EqOperator: public BinaryOperator {
public:
    EqOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "==") {}
};

class LessOperator: public BinaryOperator {
public:
    LessOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "<") {}
};

class GrOperator: public BinaryOperator {
public:
    GrOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, ">") {}
};

class LessEqOperator: public BinaryOperator {
public:
    LessEqOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "<=") {}
};

class GrEqOperator: public BinaryOperator {
public:
    GrEqOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, ">=") {}
};

class AssignOperator: public BinaryOperator {
public:
    AssignOperator(ExpressionNode *left, ExpressionNode *right):
        BinaryOperator(left, right, "=") {}
};

class IfOperatorNode: public OperatorNode {
public:
    IfOperatorNode(ExpressionNode *condition,
        NodesListNode *true_branch,
        NodesListNode *false_branch = NULL):
        condition_(condition),
        true_branch_(true_branch),
        false_branch_(false_branch) { }

    ~IfOperatorNode() {
        delete condition_;
        delete true_branch_;
        delete false_branch_;
    }
private:
    ExpressionNode *condition_;
    NodesListNode *true_branch_;
    NodesListNode *false_branch_;
};

#endif SYNTAX_TREE_H
