#ifndef COMMAND_INTERFACE_H
#define COMMAND_INTERFACE_H

#include <string>
#include <array>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <chrono>

#define COMMAND_BUFFER_SIZE 128
#define SY_COMMAND_INTERFACE "CommandInterface"
#define COMMAND_INTERFACE_COLOR "\033[1;31m"
#define COMMAND_INTERFACE_COLOR_RESET "\033[0m"

class CommandInterface
{
    public:
        enum class CMD_RESULT
        {
            SUCCESS,
            FAILURE
        };

    public:
        static CommandInterface& getInstance() {
            static CommandInterface instance;
            return instance;
        }
        inline void setDebugMode(const bool debugMode) {
            _debugMode = debugMode;
        }
        inline bool isDebugMode() const {
            return _debugMode;
        }
        CMD_RESULT executeCommand(const std::string &command, std::string &result);
        std::string getSystemClock();

    private:
        CommandInterface() : _debugMode(false) {}
        ~CommandInterface() = default;
        CommandInterface(const CommandInterface&) = delete;
        CommandInterface& operator=(const CommandInterface&) = delete;

    private:
        bool _debugMode;
};


#endif // "COMMAND_INTERFACE_H"