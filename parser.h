#pragma once

#include <memory>

#include "object.h"
#include <tokenizer.h>
#include "error.h"

std::shared_ptr<Object> Read(Tokenizer* tokenizer, bool first_read = true);

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer, bool from_open = false);
