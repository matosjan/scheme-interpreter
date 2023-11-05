#include <tokenizer.h>
#include <error.h>
////////////////////////////////////////////////////

Tokenizer::Tokenizer(std::istream *in) {
    input_stream_ = in;
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

void Tokenizer::Next() {
    while (isspace(input_stream_->peek())) {
        input_stream_->get();
    }
    if (input_stream_->peek() == EOF) {
        is_end_ = true;
        return;
    }

    char curr_char;
    input_stream_->get(curr_char);

    // Quote
    if (curr_char == '\'') {
        current_token_ = QuoteToken();
        return;
    }

    // Brackets
    if (curr_char == '(') {
        current_token_ = BracketToken::OPEN;
        return;
    }
    if (curr_char == ')') {
        current_token_ = BracketToken::CLOSE;
        return;
    }

    // Dot
    if (curr_char == '.') {
        current_token_ = DotToken();
        return;
    }

    // Bool
    if (curr_char == '#') {
        if (input_stream_->peek() == 't') {
            input_stream_->get(curr_char);
            current_token_ = BoolToken{true};
            return;
        }
        if (input_stream_->peek() == 'f') {
            input_stream_->get(curr_char);
            current_token_ = BoolToken{false};
            return;
        }
    }

    // Symbol
    if (kSymbolStartConst.find(curr_char) != std::string::npos) {
        std::string str;
        str.push_back(curr_char);
        while (input_stream_->peek() != EOF && !isspace(input_stream_->peek()) &&
               input_stream_->peek() != ')') {
            input_stream_->get(curr_char);
            if (kSymbolInsideConst.find(curr_char) != std::string::npos) {
                str.push_back(curr_char);
            } else {
                throw SyntaxError("SyntaxError");
            }
        }
        current_token_ = SymbolToken{str};
        return;
    }

    // Num
    if (isdigit(curr_char)) {
        current_token_ = ConstantToken{GetNum(curr_char)};
        return;
    }

    //+
    if (curr_char == '+') {
        if (!isdigit(input_stream_->peek())) {
            current_token_ = SymbolToken{"+"};
            return;
        }
        input_stream_->get(curr_char);
        current_token_ = ConstantToken{GetNum(curr_char)};
        return;
    }

    //-
    if (curr_char == '-') {
        if (!isdigit(input_stream_->peek())) {
            current_token_ = SymbolToken{"-"};
            return;
        }
        input_stream_->get(curr_char);
        current_token_ = ConstantToken{-GetNum(curr_char)};
        return;
    }

    throw SyntaxError("SyntaxError");
}

Token Tokenizer::GetToken() {
    return current_token_;
}

int Tokenizer::GetNum(char curr_char) {
    std::string num;
    num.push_back(curr_char);
    while (input_stream_->peek() != EOF && !isspace(input_stream_->peek())) {
        if (isdigit(input_stream_->peek())) {
            input_stream_->get(curr_char);
            num.push_back(curr_char);
        } else {
            break;
        }
    }
    return std::stoi(num);
}

////////////////////////////////////////////////////

bool SymbolToken::operator==(const SymbolToken &other) const {
    return name == other.name;
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

bool ConstantToken::operator==(const ConstantToken &other) const {
    return value == other.value;
}

bool BoolToken::operator==(const BoolToken &other) const {
    return bool_val == other.bool_val;
}
