#include "scheme.h"

std::string Interpreter::Run(const std::string &input) {
    std::stringstream input_stream(input);
    Tokenizer tokenizer(&input_stream);
    auto input_ast = Read(&tokenizer);
    if (input_ast == nullptr) {
        throw RuntimeError("empty list");
    }
    auto res_res = input_ast->Eval();
    if (res_res == nullptr) {
        return "()";
    }
    return res_res->Serialise();
}
