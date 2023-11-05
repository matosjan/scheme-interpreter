#pragma once

#include <string>
#include "tokenizer.h"
#include "parser.h"
#include <sstream>

class Interpreter {
public:
    std::string Run(const std::string& input);
};
