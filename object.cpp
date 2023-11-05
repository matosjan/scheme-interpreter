#include "object.h"

//////////////////////////////// Eval

std::shared_ptr<Object> Number::Eval() {
    return std::make_shared<Number>(GetValue());
}

std::shared_ptr<Object> Symbol::Eval() {
    if (functions_map.contains(name_)) {
        return functions_map[name_];
    } else {
        return std::make_shared<Symbol>(name_);
    }
}

std::shared_ptr<Object> Boolean::Eval() {
    return std::make_shared<Boolean>(GetBool());
}

std::shared_ptr<Object> Cell::Eval() {
    if (is_quoted) {
        return shared_from_this();
    }
    if (GetFirst() == nullptr && GetSecond() == nullptr) {
        throw RuntimeError("robin");
    }
    if (is_list_ && !Is<Symbol>(GetFirst())) {
        throw RuntimeError("no function in list");
    }
    if (Is<Symbol>(GetFirst())) {
        auto func = GetFirst()->Eval();
        std::vector<std::shared_ptr<Object>> args;
        if (Is<Quote>(func)) {
            args.push_back(GetSecond());
        } else if (Is<And>(func) || Is<Or>(func)) {
            TreeToVectorNoEval(GetSecond(), args);
        } else {
            if (GetSecond() != nullptr) {
                if (GetSecond()->Eval() == nullptr) {
                    args.push_back(nullptr);
                } else {
                    TreeToVector(GetSecond()->Eval(), args);
                }
            }
        }
        if (Is<Function>(func)) {
            return As<Function>(func)->Apply(args);
        }
    }
    if (GetSecond() == nullptr) {
        return GetFirst()->Eval();
    }
    if (GetFirst() == nullptr) {
        throw RuntimeError("batman");
    }
    return std::make_shared<Cell>(GetFirst()->Eval(), GetSecond()->Eval());
}

///////////////////////////////// Serialise
std::string Number::Serialise() {
    return std::to_string(val_);
    ;
}

std::string Boolean::Serialise() {
    if (bool_) {
        return "#t";
    } else {
        return "#f";
    }
}

std::string Symbol::Serialise() {
    return name_;
}

std::string Cell::Serialise() {
    std::string res;
    if (is_list_) {
        res = '(';
    }
    SerialiseInsideOfBrackets(std::make_shared<Cell>(*this), res);
    if (is_list_) {
        res.push_back(')');
    }
    return res;
}

void Cell::SerialiseInsideOfBrackets(std::shared_ptr<Object> node, std::string& str) {
    if (!Is<Cell>(node)) {
        str += node->Serialise();
    }

    // Left son
    if (Is<Cell>(As<Cell>(node)->GetFirst())) {
        if (!str.empty() && str.back() != '(') {
            str.push_back(' ');
        }
        if (As<Cell>(As<Cell>(node)->GetFirst())->is_list_) {
            str += As<Cell>(As<Cell>(node)->GetFirst())->Serialise();
        } else {
            std::string temp_str;
            SerialiseInsideOfBrackets(As<Cell>(node)->GetFirst(), temp_str);
            str += temp_str;
        }
    } else {
        if (!str.empty() && str.back() != '(') {
            str.push_back(' ');
        }
        if (As<Cell>(node)->GetFirst() != nullptr) {
            str += As<Cell>(node)->GetFirst()->Serialise();
        } else {
            str += "()";
        }
    }

    // Right son
    if (As<Cell>(node)->GetSecond() == nullptr) {
        return;
    }

    if (Is<Cell>(As<Cell>(node)->GetSecond())) {
        str.push_back(' ');
        if (!As<Cell>(node)->CellWithDot()) {
            if (Is<Cell>(As<Cell>(node)->GetSecond())) {
                if (As<Cell>(As<Cell>(node)->GetSecond())->CellWithDot()) {
                    std::string temp_str;
                    SerialiseInsideOfBrackets(As<Cell>(node)->GetSecond(), temp_str);
                    str += temp_str;
                } else {
                    str += As<Cell>(As<Cell>(node)->GetSecond())->Serialise();
                }
            } else {
                str += As<Cell>(node)->GetSecond()->Serialise();
            }
        } else {
            std::string temp_str;
            SerialiseInsideOfBrackets(As<Cell>(node)->GetSecond(), temp_str);
            str += temp_str;
        }
    } else {
        if (As<Cell>(node)->CellWithDot()) {
            str.push_back(' ');
            str.push_back('.');
            str.push_back(' ');
        } else {
            str.push_back(' ');
        }
        str += As<Cell>(node)->GetSecond()->Serialise();
    }
}

///////////////////////////////// Helpers
void TreeToVector(std::shared_ptr<Object> node, std::vector<std::shared_ptr<Object>>& res) {
    if (node != nullptr) {
        if (!Is<Cell>(node)) {
            res.push_back(node->Eval());
        } else {
            if (As<Cell>(node)->is_quoted) {
                res.push_back(node);
                return;
            }

            if (As<Cell>(node)->GetFirst() != nullptr) {
                if (Is<Cell>(As<Cell>(node)->GetFirst()) &&
                    As<Cell>(As<Cell>(node)->GetFirst())->is_quoted) {
                    res.push_back(As<Cell>(node)->GetFirst());
                } else {
                    TreeToVector(As<Cell>(node)->GetFirst()->Eval(), res);
                }
            }
            if (As<Cell>(node)->GetSecond() != nullptr) {
                if (Is<Cell>(As<Cell>(node)->GetSecond()) &&
                    As<Cell>(As<Cell>(node)->GetSecond())->is_quoted) {
                    res.push_back(As<Cell>(node)->GetSecond());
                } else {
                    TreeToVector(As<Cell>(node)->GetSecond()->Eval(), res);
                }
            }
        }
    }
}

void TreeToVectorNoEval(std::shared_ptr<Object> node, std::vector<std::shared_ptr<Object>>& res) {
    if (node != nullptr) {
        if (Is<Cell>(node) && !Is<Cell>(As<Cell>(node)->GetFirst()) &&
            !Is<Symbol>(As<Cell>(node)->GetFirst())) {
            res.push_back(As<Cell>(node)->GetFirst());
            if (As<Cell>(node)->GetSecond() != nullptr) {
                TreeToVectorNoEval(As<Cell>(node)->GetSecond(), res);
            }
        } else if (Is<Cell>(node) && !Is<Cell>(As<Cell>(node)->GetFirst()) &&
                   Is<Symbol>(As<Cell>(node)->GetFirst())) {
            res.push_back(node);
        } else {
            if (As<Cell>(node)->GetFirst() != nullptr) {
                TreeToVectorNoEval(As<Cell>(node)->GetFirst(), res);
            }
            if (As<Cell>(node)->GetSecond() != nullptr) {
                TreeToVectorNoEval(As<Cell>(node)->GetSecond(), res);
            }
        }
    }
}

bool Cell::CellWithDot() {
    if (!Is<Cell>(first_) && !Is<Cell>(second_)) {
        return true;
    }
    return false;
}

///////////////////////////////// Functions

std::shared_ptr<Object> IntegerPredicate::Apply(std::vector<std::shared_ptr<Object>> args) {
    return std::make_shared<Boolean>(Is<Number>(args.front()));
}

std::shared_ptr<Object> Sum::Apply(std::vector<std::shared_ptr<Object>> args) {
    int64_t res = 0;
    for (size_t i = 0; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            res += As<Number>(args[i])->GetValue();
        } else {
            throw RuntimeError("not number in args for +");
        }
    }
    return std::make_shared<Number>(res);
}

std::shared_ptr<Object> Subtraction::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        throw RuntimeError("No args for minus");
    }
    int64_t res = 0;
    for (size_t i = 0; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            if (i == 0) {
                res = As<Number>(args[i])->GetValue();
            } else {
                res -= As<Number>(args[i])->GetValue();
            }
        } else {
            throw RuntimeError("not number in args for -");
        }
    }
    return std::make_shared<Number>(res);
}

std::shared_ptr<Object> Product::Apply(std::vector<std::shared_ptr<Object>> args) {
    int64_t res = 1;
    for (size_t i = 0; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            res *= As<Number>(args[i])->GetValue();
        } else {
            throw RuntimeError("not number in args for *");
        }
    }
    return std::make_shared<Number>(res);
}

std::shared_ptr<Object> Division::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        throw RuntimeError("No args for minus");
    }
    int64_t res = 0;
    for (size_t i = 0; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            if (i == 0) {
                res = As<Number>(args[i])->GetValue();
            } else {
                res /= As<Number>(args[i])->GetValue();
            }
        } else {
            throw RuntimeError("not number in args for /");
        }
    }
    return std::make_shared<Number>(res);
}

std::shared_ptr<Object> Max::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        throw RuntimeError("No args for minus");
    }
    int64_t res = 0;
    for (size_t i = 0; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            res = std::max(res, As<Number>(args[i])->GetValue());
        } else {
            throw RuntimeError("not number in args for max");
        }
    }
    return std::make_shared<Number>(res);
}

std::shared_ptr<Object> Min::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        throw RuntimeError("No args for minus");
    }
    int64_t res = 10000000;  // mnogo жоско
    for (size_t i = 0; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            res = std::min(res, As<Number>(args[i])->GetValue());
        } else {
            throw RuntimeError("not number in args for min");
        }
    }
    return std::make_shared<Number>(res);
}

std::shared_ptr<Object> Abs::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty() || args.size() > 1) {
        throw RuntimeError("too many args or to few for abs");
    }
    if (!Is<Number>(args.front())) {
        throw RuntimeError("not number as arg to abs");
    }
    return std::make_shared<Number>(std::abs(As<Number>(args.front())->GetValue()));
}

std::shared_ptr<Object> Decreasing::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(true);
    }
    if (args.size() == 1 && Is<Number>(args.front())) {
        return std::make_shared<Boolean>(true);
    } else if (!Is<Number>(args.front())) {
        throw RuntimeError("not number in args for >");
    }
    for (size_t i = 1; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            if (As<Number>(args[i])->GetValue() >= As<Number>(args[i - 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError("not number in args for >");
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> DecreasingOrEqual::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(true);
    }
    if (args.size() == 1 && Is<Number>(args.front())) {
        return std::make_shared<Boolean>(true);
    } else if (!Is<Number>(args.front())) {
        throw RuntimeError("not number in args for >=");
    }
    for (size_t i = 1; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            if (As<Number>(args[i])->GetValue() > As<Number>(args[i - 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError("not number in args for >=");
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> Increasing::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(true);
    }
    if (args.size() == 1 && Is<Number>(args.front())) {
        return std::make_shared<Boolean>(true);
    } else if (!Is<Number>(args.front())) {
        throw RuntimeError("not number in args for <");
    }
    for (size_t i = 1; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            if (As<Number>(args[i])->GetValue() <= As<Number>(args[i - 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError("not number in args for <");
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> IncreasingOrEqual::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(true);
    }
    if (args.size() == 1 && Is<Number>(args.front())) {
        return std::make_shared<Boolean>(true);
    } else if (!Is<Number>(args.front())) {
        throw RuntimeError("not number in args for <=");
    }
    for (size_t i = 1; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            if (As<Number>(args[i])->GetValue() < As<Number>(args[i - 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError("not number in args for <=");
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> Equal::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(true);
    }
    if (args.size() == 1 && Is<Number>(args.front())) {
        return std::make_shared<Boolean>(true);
    } else if (!Is<Number>(args.front())) {
        throw RuntimeError("not number in args for ==");
    }
    for (size_t i = 1; i < args.size(); i++) {
        if (Is<Number>(args[i])) {
            if (As<Number>(args[i])->GetValue() != As<Number>(args[i - 1])->GetValue()) {
                return std::make_shared<Boolean>(false);
            }
        } else {
            throw RuntimeError("not number in args for ==");
        }
    }
    return std::make_shared<Boolean>(true);
}

std::shared_ptr<Object> BooleanPredicate::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty() || args.size() > 1) {
        throw RuntimeError("too many or too few args in boolean?");
    }
    if (Is<Boolean>(args.front())) {
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> Not::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty() || args.size() > 1) {
        throw RuntimeError("too many or too few args in boolean?");
    }
    if (Is<Boolean>(args.front())) {
        if (As<Boolean>(args.front())->GetBool()) {
            return std::make_shared<Boolean>(false);
        }
        return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
}

std::shared_ptr<Object> Quote::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (Is<Cell>(args.front())) {
        As<Cell>(args.front())->is_quoted = true;
    }
    return args.front();
}

std::shared_ptr<Object> PairPredicate::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.front() == nullptr) {
        return std::make_shared<Boolean>(false);
    }
    auto node = args.front();
    if (!Is<Cell>(node)) {
        return std::make_shared<Boolean>(false);
    }
    if (As<Cell>(node)->GetFirst() != nullptr && As<Cell>(node)->GetSecond() != nullptr) {
        return std::make_shared<Boolean>(true);
    }
}

std::shared_ptr<Object> NullPredicate::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.front() == nullptr) {
        return std::make_shared<Boolean>(true);
    } else {
        return std::make_shared<Boolean>(false);
    }
}

std::shared_ptr<Object> ListPredicate::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.front() == nullptr) {
        return std::make_shared<Boolean>(true);
    }
    std::shared_ptr<Object> temp = args.front();
    while (Is<Cell>(temp) && temp != nullptr) {
        temp = As<Cell>(temp)->GetSecond();
    }
    if (temp != nullptr) {
        return std::make_shared<Boolean>(false);
    } else {
        return std::make_shared<Boolean>(true);
    }
    return nullptr;
}
std::shared_ptr<Object> Cons::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.size() < 2) {
        throw RuntimeError("to few args for cons");
    }
    auto to_return = std::make_shared<Cell>(args[0], args[1]);
    to_return->IsList();
    return to_return;
}

std::shared_ptr<Object> Car::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty() || (args.size() == 1 && args.front() == nullptr)) {
        throw RuntimeError("no args for car");
    }
    return As<Cell>(args.front())->GetFirst();
}

std::shared_ptr<Object> Cdr::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.front() == nullptr) {
        throw RuntimeError("no args for car");
    }
    auto to_return = As<Cell>(args.front())->GetSecond();

    if (to_return != nullptr && !As<Cell>(args.front())->CellWithDot()) {
        As<Cell>(to_return)->IsList();
        return to_return;
    }
    return As<Cell>(args.front())->GetSecond();
}

std::shared_ptr<Object> List::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty() || args.front() == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Cell> to_return = nullptr;
    for (int i = args.size() - 1; i >= 0; i--) {
        to_return = std::make_shared<Cell>(args[i], to_return);
    }
    to_return->IsList();
    return to_return;
}

std::shared_ptr<Object> ListRef::Apply(std::vector<std::shared_ptr<Object>> args) {
    Cdr cdr;
    auto res = args.front();
    std::vector<std::shared_ptr<Object>> temp;
    temp.push_back(res);
    List list_maker;
    for (int i = 0; i < As<Number>(args.back())->GetValue(); i++) {
        if (res == nullptr) {
            throw RuntimeError("joker");
        }
        res = cdr.Apply(temp);
        temp.clear();
        temp.push_back(res);
    }
    Car car;
    res = car.Apply(temp);
    if (res != nullptr) {
        return res;
    } else {
        throw RuntimeError("matos");
    }
}

std::shared_ptr<Object> ListTail::Apply(std::vector<std::shared_ptr<Object>> args) {
    Cdr cdr;
    auto res = args.front();
    std::vector<std::shared_ptr<Object>> temp;
    temp.push_back(res);
    List list_maker;
    for (int i = 0; i < As<Number>(args.back())->GetValue(); i++) {
        if (res == nullptr) {
            throw RuntimeError("nike");
        }
        res = cdr.Apply(temp);
        temp.clear();
        temp.push_back(res);
    }
    return res;
}

std::shared_ptr<Object> And::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(true);
    }
    for (int i = 0; i < args.size(); i++) {
        if (Is<Boolean>(args[i]->Eval()) && !As<Boolean>(args[i]->Eval())->GetBool()) {
            return std::make_shared<Boolean>(false);
        }
    }
    return args.back()->Eval();
}
std::shared_ptr<Object> Or::Apply(std::vector<std::shared_ptr<Object>> args) {
    if (args.empty()) {
        return std::make_shared<Boolean>(false);
    }
    for (int i = 0; i < args.size(); i++) {
        if ((Is<Boolean>(args[i]->Eval()) && As<Boolean>(args[i]->Eval())->GetBool()) ||
            !Is<Boolean>(args[i]->Eval())) {
            return args[i]->Eval();
        }
    }
    return args.back()->Eval();
}
