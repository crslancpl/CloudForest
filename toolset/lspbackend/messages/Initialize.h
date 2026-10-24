#pragma once

#include "Base.h"
#include <optional>
#include <string>

class Message {
public:
    virtual std::string& ToString();
    virtual void WriteToStream();
};

class InitializeParam : LspParams {
public:
    std::optional<int> ProcessId;

    const std::string& ToString();
};
