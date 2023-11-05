#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "error.h"
#include <algorithm>
#include <cmath>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
    virtual std::string Serialise() {
        return "doesn't have";
    };
    virtual std::shared_ptr<Object> Eval() {
        return nullptr;
    }
};

class Function : public Object {
public:
    virtual std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) = 0;
};

class Number : public Object {
public:
    Number(int val) : val_(val){};

    int64_t GetValue() const {
        return val_;
    }

    void SetVal(int v) {
        val_ = v;
    }

    std::shared_ptr<Object> Eval() override;

    std::string Serialise() override;

private:
    int64_t val_ = 0;
};

class Symbol : public Object {
public:
    Symbol(const std::string& str) : name_(str){};

    const std::string& GetName() const {
        return name_;
    }

    void SetName(const std::string& str) {
        name_ = str;
    }

    std::shared_ptr<Object> Eval() override;

    std::string Serialise() override;

private:
    std::string name_;
};

class Boolean : public Object {
public:
    Boolean(bool boolean) : bool_(boolean){};

    bool GetBool() {
        return bool_;
    };

    std::shared_ptr<Object> Eval() override;

    std::string Serialise() override;

private:
    bool bool_;
};

class Cell : public Object {
public:
    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(first), second_(second){};

    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    void SetFirst(std::shared_ptr<Object> obj) {
        first_ = obj;
    }

    void SetSecond(std::shared_ptr<Object> obj) {
        second_ = obj;
    }

    std::shared_ptr<Object> Eval() override;

    std::string Serialise() override;

    void SerialiseInsideOfBrackets(std::shared_ptr<Object> node, std::string& str);

    bool CellWithDot();

    void IsList() {
        is_list_ = true;
    }
    bool is_quoted = false;

private:
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
    bool is_list_ = false;
};

/////////////////////////////////////////////////////////////////////////////// Functions

class IntegerPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Sum : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Subtraction : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Product : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Division : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Max : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Min : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Abs : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Decreasing : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class DecreasingOrEqual : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Increasing : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class IncreasingOrEqual : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Equal : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class BooleanPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Not : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Quote : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class PairPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class NullPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class ListPredicate : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Cons : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Car : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Cdr : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class List : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class ListRef : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class ListTail : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class And : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

class Or : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> args) override;
};

static std::unordered_map<std::string, std::shared_ptr<Function>> functions_map = {
    {"number?", std::make_shared<IntegerPredicate>()},
    {"+", std::make_shared<Sum>()},
    {"-", std::make_shared<Subtraction>()},
    {"*", std::make_shared<Product>()},
    {"/", std::make_shared<Division>()},
    {"max", std::make_shared<Max>()},
    {"min", std::make_shared<Min>()},
    {"abs", std::make_shared<Abs>()},
    {">", std::make_shared<Decreasing>()},
    {">=", std::make_shared<DecreasingOrEqual>()},
    {"<", std::make_shared<Increasing>()},
    {"<=", std::make_shared<IncreasingOrEqual>()},
    {"=", std::make_shared<Equal>()},
    {"boolean?", std::make_shared<BooleanPredicate>()},
    {"not", std::make_shared<Not>()},
    {"quote", std::make_shared<Quote>()},
    {"pair?", std::make_shared<PairPredicate>()},
    {"null?", std::make_shared<NullPredicate>()},
    {"list?", std::make_shared<ListPredicate>()},
    {"cons", std::make_shared<Cons>()},
    {"car", std::make_shared<Car>()},
    {"cdr", std::make_shared<Cdr>()},
    {"list", std::make_shared<List>()},
    {"list-ref", std::make_shared<ListRef>()},
    {"list-tail", std::make_shared<ListTail>()},
    {"and", std::make_shared<And>()},
    {"or", std::make_shared<Or>()}};

/////////////////////////////////////////////////////////////////////////////// Helpers

void TreeToVector(std::shared_ptr<Object> node, std::vector<std::shared_ptr<Object>>& res);
void TreeToVectorNoEval(std::shared_ptr<Object> node, std::vector<std::shared_ptr<Object>>& res);

// Runtime type checking and convertion.
// This can be helpful: https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    if (As<T>(obj)) {
        return true;
    }
    return false;
}
