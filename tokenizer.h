#pragma once

#include <variant>
#include <optional>
#include <istream>
const std::string kSymbolStartConst = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ<=>*/#";
const std::string kSymbolInsideConst =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ<=>*/#0123456789?!-";

struct SymbolToken {
    std::string name;
    bool operator==(const SymbolToken& other) const;
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const;
};

struct DotToken {
    bool operator==(const DotToken&) const;
};

struct BoolToken {
    bool bool_val;
    bool operator==(const BoolToken& other) const;
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    int value;
    bool operator==(const ConstantToken& other) const;
};

using Token =
    std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken, BoolToken>;

class Tokenizer {
public:
    Tokenizer(std::istream* in);

    bool IsEnd();

    void Next();

    Token GetToken();

    int GetNum(char curr_char);

private:
    std::istream* input_stream_;
    Token current_token_;
    bool is_end_ = false;
};
