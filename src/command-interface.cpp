#include "command-interface.h"

CommandInterface::CMD_RESULT CommandInterface::executeCommand (const std::string &command, std::string &result)
{
    std::array<char, COMMAND_BUFFER_SIZE> buffer;
    int cmdExitStatus;

    // Open pipe to execute the command
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe)
    {
        char errorMsg[COMMAND_BUFFER_SIZE];
        snprintf(errorMsg, COMMAND_BUFFER_SIZE, "%s[%s] [%s] [%s] [popen() failed!]%s\n", COMMAND_INTERFACE_COLOR, getSystemClock().c_str(), SY_COMMAND_INTERFACE, __func__, COMMAND_INTERFACE_COLOR_RESET);
        pclose(pipe);
        throw std::runtime_error(errorMsg);
    }

    // Reading command output
    while (fgets(buffer.data(), COMMAND_BUFFER_SIZE, pipe) != nullptr)
        result += buffer.data();

    // Closing pipe, handling exit status
    cmdExitStatus = pclose(pipe);
    if (WEXITSTATUS(cmdExitStatus) != 0)
    {
        if (isDebugMode())
            printf("%s[%s] [%s] [%s] [DBus service error!]%s\n", COMMAND_INTERFACE_COLOR, getSystemClock().c_str(), SY_COMMAND_INTERFACE, __func__, COMMAND_INTERFACE_COLOR_RESET);
        return CMD_RESULT::FAILURE;
    }

    return CMD_RESULT::SUCCESS;
}

std::string CommandInterface::getSystemClock ()
{
    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);

    // Format it to %H:%M:%S
    std::stringstream ss;
    ss << std::put_time(std::localtime(&currentTime), "%H:%M:%S");
    return ss.str();
}