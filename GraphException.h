#ifndef CAMPUSNAVIGATION_GRAPHEXCEPTION_H
#define CAMPUSNAVIGATION_GRAPHEXCEPTION_H

#include <exception>
#include <string>

namespace Graph
{
    class GraphException : public std::exception
    {
        private:
            std::string message;
        public:
            explicit GraphException(const std::string& msg) : message(msg) {}
            const char* what() const noexcept override
            {
                return message.c_str();
            }
    };
}

#endif // CAMPUSNAVIGATION_GRAPHEXCEPTION_H