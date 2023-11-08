#pragma once

#include <exception>
#include <string>

namespace opengles_workspace
{
class Exception: public std::exception
{
public:
    Exception(std::string message)
        : mMessage(std::move(message)) {}
    
    const char* what() const noexcept override { return mMessage.c_str(); }
private:
    std::string mMessage;
};
}