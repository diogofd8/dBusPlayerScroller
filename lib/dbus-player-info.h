#ifndef DBUS_PLAYER_INFO
#define DBUS_PLAYER_INFO

#include <iostream>
#include <string>
#include <array>

#include "command-interface.h"

#define SY_DBUS_PLAYER_INFO "DBusPlayerInfo"
#define DBUS_PLAYER_INFO_COLOR "\033[1;34m"
#define DBUS_PLAYER_INFO_COLOR_RESET "\033[0m"
#define COMMAND_BUFFER_SIZE 128
#define SPACE_CHAR ' '

class DBusPlayerInfo
{
    public:
        enum class PLAYBACK_STATUS 
            {
                PLAYING,
                PAUSED,
                NOT_FOUND
            };

    public:
        DBusPlayerInfo();
        DBusPlayerInfo(const std::string& playerName);
        ~DBusPlayerInfo();
        PLAYBACK_STATUS updatePlayerStatus();
        bool updatePlaybackMetadata();
        void clearDBusPlayerData();
        inline const std::string& getSongInfo () const {
            return _songInfo;
        }
        inline const std::string& getPlayerName () const {
            return _playerName;
        }
        inline const std::string& getSongPrefix () const {
            return _songPrefix;
        }
        inline const std::string& getSongSuffix () const {
            return _songSuffix;
        }
        inline const std::string& getSongSeparator () const {
            return _songSeparator;
        }
        inline PLAYBACK_STATUS getPlayerStatus () const {
            return _playerStatus;
        }
        inline const std::string debugPlayerStatus () const {
            switch (_playerStatus)
            {
                case PLAYBACK_STATUS::PLAYING:      return "PLAYING";
                case PLAYBACK_STATUS::PAUSED:       return "PAUSED";
                case PLAYBACK_STATUS::NOT_FOUND:    return "NOT_FOUND";
            }
            return "ERROR: UNKNOWN";
        }
        inline bool getMetadataSyncState () const {
            return _isMetadataSynced;
        }
        inline void setSongPrefix (const std::string& songPrefix) {
            _songPrefix = std::move(songPrefix);
        }
        inline void setSongSuffix (const std::string& songSuffix) {
            _songSuffix = std::move(songSuffix);
        }
        inline void setSongSeparator (const std::string& songSeparator) {
            _songSeparator = std::move(songSeparator);
        }

    protected:
        inline void setPlaybackMetadata (const std::string& playbackMetadata) {
            _playbackMetadata = std::move(playbackMetadata);
        }
        inline void setSongInfo (const std::string& songInfo) {
            _songInfo = std::move(songInfo);
        }
        inline void setPlayerName (const std::string& playerName) {
            _playerName = std::move(playerName);
        }
        inline void setPlayerStatus (const PLAYBACK_STATUS currentStatus) {
            _playerStatus = std::move(currentStatus);
        }
        inline void toggleMetadataSyncState (const bool currentState) {
            _isMetadataSynced = currentState;
        }
        inline void resizeSongInfo (const size_t newSize) {
            _songInfo.resize(newSize, SPACE_CHAR);
        }
        void updateSongInfo();

    private:
        std::string _playbackMetadata;
        std::string _songInfo;
        std::string _playerName;
        std::string _songPrefix;
        std::string _songSuffix;
        std::string _songSeparator;
        bool _isMetadataSynced;
        PLAYBACK_STATUS _playerStatus;
        CommandInterface* _pCommandInterface;

};

#endif // DBUS_PLAYER_INFO