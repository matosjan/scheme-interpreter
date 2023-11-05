#include <parser.h>

std::shared_ptr<Object> Read(Tokenizer* tokenizer, bool first_read) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("SyntaxError");
    }

    Token curr_token = tokenizer->GetToken();
    tokenizer->Next();
    // (
    if (curr_token == Token{BracketToken::OPEN}) {
        auto subtree = ReadList(tokenizer, true);
        if (tokenizer->GetToken() == Token{BracketToken::CLOSE} && !tokenizer->IsEnd()) {
            tokenizer->Next();
        } else {
            throw SyntaxError("SyntaxError");
        }
        if (first_read) {
            if (!tokenizer->IsEnd()) {
                throw SyntaxError("SyntaxError");
            }
        }
        return subtree;
    }
    // )
    if (curr_token == Token{BracketToken::CLOSE}) {
        throw SyntaxError("SyntaxError");
    }
    // Const Token
    if (std::holds_alternative<ConstantToken>(curr_token)) {
        return std::make_shared<Number>(std::get<ConstantToken>(curr_token).value);
    }
    // Symbol Token
    if (std::holds_alternative<SymbolToken>(curr_token)) {
        return std::make_shared<Symbol>(std::get<SymbolToken>(curr_token).name);
    }
    // BoolToken
    if (std::holds_alternative<BoolToken>(curr_token)) {
        return std::make_shared<Boolean>(std::get<BoolToken>(curr_token).bool_val);
    }
    // DotToken -> syntax error
    if (std::holds_alternative<DotToken>(curr_token)) {
        throw SyntaxError("SyntaxError");
    }
    // QuoteToken
    if (std::holds_alternative<QuoteToken>(curr_token)) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError("Syntax Error");
        } else {
            return std::make_shared<Cell>(std::make_shared<Symbol>("quote"),
                                          Read(tokenizer, false));
        }
    }
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer, bool from_open) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("SyntaxError");
    }

    if (tokenizer->GetToken() == Token{BracketToken::CLOSE}) {
        return nullptr;
    }

    Cell to_return(nullptr, nullptr);
    if (tokenizer->GetToken() != Token{BracketToken::OPEN} && from_open) {
        to_return.IsList();
    }
    std::shared_ptr<Object> first_elem = Read(tokenizer, false);
    std::shared_ptr<Object> second_elem = nullptr;

    if (first_elem == nullptr && second_elem == nullptr) {
        to_return.IsList();
    }

    // встретил точку
    if (std::holds_alternative<DotToken>(tokenizer->GetToken())) {
        tokenizer->Next();
        second_elem = Read(tokenizer, false);
    } else {
        second_elem = ReadList(tokenizer, false);
    }

    to_return.SetFirst(first_elem);
    to_return.SetSecond(second_elem);
    return std::make_shared<Cell>(to_return);
}
