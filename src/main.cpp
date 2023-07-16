/*
    * ===| DBus Player Scroller |===
    * Author: Diogo Duarte (diogofd8)
    * Github: @diogofd8
*/

#include <iostream>
#include <string>
#include <thread>


#include "command-interface.h"
#include "dbus-player-info.h"
#include "player-scroller.h"

#define SY_MAIN "Main"
#define MAIN_COLOR "\033[1;37m"
#define MAIN_COLOR_RESET "\033[0m"

// Global variables
CommandInterface* pCmdI = nullptr;

class ProgramOptions
{
    public:
        ProgramOptions () :
            playerName("spotify"),
            metataPrefix(""),
            metadataSuffix(" | "),
            metadataSeparator(" - "),
            viewLength(30),
            scrollSpeed(250),
            debugMode(false),
            syntax("Syntax: dBusStringScroller [Options]\nOptions:\n\t-p | --player\t\t[player name]\n\t-s | --speed\t\t[scroll speed]\n\t-l | --length\t\t[view length]\n\t-x | --suffix\t\t[metadata suffix]\n\t-f | --prefix\t\t[metadata prefix]\n\t-r | --separator\t[metadata separator]\n\t-d | --debug")
        {}
        ~ProgramOptions () {}

    public:
        std::string playerName;
        std::string metataPrefix;
        std::string metadataSuffix;
        std::string metadataSeparator;
        int viewLength;
        size_t scrollSpeed;
        bool debugMode;
        std::string syntax;
};

ProgramOptions execArgsParser (int argc, char* argv[])
{
    ProgramOptions options;

    if (argc == 1)
    {
        return options;
    }

    bool bParseError = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--speed" || arg == "-s")
        {
            if (argc > ++i && argv[i][0] != '-')
                options.scrollSpeed = std::stoul(argv[i]);
            else
                bParseError = true;
        }
        else if (arg == "--length" || arg == "-l")
        {
            if (argc > ++i && argv[i][0] != '-')
                options.viewLength = std::stoi(argv[i]);
            else
                bParseError = true;
        }
        else if (arg == "--player" || arg == "-p")
        {
            if (argc > ++i  && argv[i][0] != '-')
                options.playerName = argv[i];
            else
                bParseError = true;
        }
        else if (arg == "--suffix" || arg == "-x")
        {
            if (argc > ++i  && argv[i][0] != '-')
                options.metadataSuffix = argv[i];
            else
                bParseError = true;
        }
        else if (arg == "--prefix" || arg == "-f")
        {
            if (argc > ++i  && argv[i][0] != '-')
                options.metataPrefix = argv[i];
            else
                bParseError = true;
        }
        else if (arg == "--separator" || arg == "-r")
        {
            if (argc > ++i  && argv[i][0] != '-')
                options.metadataSeparator = argv[i];
            else
                bParseError = true;
        }
        else if (arg == "--debug" || arg == "-d")
            options.debugMode = true;
        else
            bParseError = true;

        if (bParseError)
        {
            std::cout << MAIN_COLOR << "Invalid syntax: couldn't parse value for [" << arg << "]" << std::endl;
            std::cout << options.syntax << MAIN_COLOR_RESET << std::endl;
            exit(1);
        }
    }

    // Print selected options
    if (options.debugMode)
        printf("%s[%s] [%s] [%s] [Options: --player %s --speed %ld --length %d --prefix '%s' --suffix '%s' --separator '%s' --debug %d]%s\n", MAIN_COLOR, pCmdI->getSystemClock().c_str(), SY_MAIN, __func__, options.playerName.c_str(), options.scrollSpeed, options.viewLength, options.metataPrefix.c_str(), options.metadataSuffix.c_str(), options.metadataSeparator.c_str(), static_cast<int>(options.debugMode), MAIN_COLOR_RESET);

    return options;
}


void playbackMetadataCallback (int periodicyMs, PlayerScroller& mediaPlayer)
{
    bool isPlayerFound = false;

    while (true)
    {
        // Check player status
        switch (mediaPlayer.updatePlayerStatus())
        {
            case DBusPlayerInfo::PLAYBACK_STATUS::PLAYING:
            case DBusPlayerInfo::PLAYBACK_STATUS::PAUSED:
                isPlayerFound = true;
                mediaPlayer.updatePlaybackMetadata();
                break;
            case DBusPlayerInfo::PLAYBACK_STATUS::NOT_FOUND:
                if (isPlayerFound)
                {
                    mediaPlayer.clearDBusPlayerData();
                    isPlayerFound = false;
                }
                break;
        }

        // Sleep for the specified period
        std::this_thread::sleep_for(std::chrono::milliseconds(periodicyMs));
    }
}

void stringScrollerCallback (int periodicyMs, PlayerScroller& mediaPlayer, size_t viewLength)
{
    // Initialize the view length
    mediaPlayer.initViewerLength(viewLength);

    bool isPlayerFound = false;

    while (true)
    {
        switch (mediaPlayer.getPlayerStatus())
        {
            case DBusPlayerInfo::PLAYBACK_STATUS::PLAYING:
                mediaPlayer.metadataScroller();
                [[fallthrough]];
            case DBusPlayerInfo::PLAYBACK_STATUS::PAUSED:
                mediaPlayer.refreshMetadata();
                isPlayerFound = true;
                break;
            case DBusPlayerInfo::PLAYBACK_STATUS::NOT_FOUND:
                if (isPlayerFound)
                {
                    mediaPlayer.refreshMetadata();
                    isPlayerFound = false;
                }
                break;
        }

        if (pCmdI->isDebugMode())
            std::cout << "[" << pCmdI->getSystemClock() << "] |" << mediaPlayer.getViewerStr() << "|" << std::endl;
        else
            std::cout << mediaPlayer.getViewerStr() << std::endl;

        // Sleep for the specified period
        std::this_thread::sleep_for(std::chrono::milliseconds(periodicyMs));
    }

}

int main (int argc, char* argv[])
{
    // Initialize the CommandInterface
    pCmdI = &CommandInterface::getInstance();

    // Parse the execution arguments
    ProgramOptions options = execArgsParser(argc, argv);

    // Handle debug flag
    if (options.debugMode)
        pCmdI->setDebugMode(true);

    // Initialize the StringScroller
    PlayerScroller mediaPlayer(options.playerName);

    // Set the metadata prefix, suffix and separator
    if (!options.metataPrefix.empty())
        mediaPlayer.setSongPrefix(options.metataPrefix);
    if (!options.metadataSuffix.empty())
        mediaPlayer.setSongSuffix(options.metadataSuffix);
    if (!options.metadataSeparator.empty())
        mediaPlayer.setSongSeparator(options.metadataSeparator);

    // Metadata thread
    std::thread tPlaybackMetadata([&]()
    {
        playbackMetadataCallback(1000, mediaPlayer);
    });

    // Scroller thread
    std::thread tStringScroller([&]()
    {
        stringScrollerCallback(options.scrollSpeed, mediaPlayer, options.viewLength);
    });

    // Join the threads
    if (tPlaybackMetadata.joinable())
        tPlaybackMetadata.join();
    if (tStringScroller.joinable())
        tStringScroller.join();

    return 0;
}