#include "error.hpp"
#include <string>

/// @brief function to simplify error feedback
/// @param code the error code
void handle_error(int code)
{
    std::string message;
    switch (code)
    {
    case NOT_IMPLEMENTED:
        message = "Not implemented";
        break;
    case ERR_FORMAT:
        message = "Format error";
        break;
    case PREMATURE_END:
        message = "Premature termination";
        break;
    case NOT_NUMBER:
        message = "Not a number";
        break;
    default:
        message = "Unknown error";
        break;
    }
    fprintf(stderr, "%s\n", message.c_str());
    exit(code);
}
