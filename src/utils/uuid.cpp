#include "utils/uuid.h"

std::string generate_uuid()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 15);

    const char *uuid_chars = "0123456789abcdef";

    std::stringstream ss;
    for (int i = 0; i < 32; ++i)
    {
        int nibble = distrib(gen);
        char c = uuid_chars[nibble];
        if (i == 8 || i == 12 || i == 16 || i == 20)
        {
            ss << "-";
        }
        ss << c;
    }
    return ss.str();
}