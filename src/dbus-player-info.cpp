#include "dbus-player-info.h"

DBusPlayerInfo::DBusPlayerInfo () : DBusPlayerInfo("")
{
}

DBusPlayerInfo::DBusPlayerInfo (const std::string& playerName)
{
    _isMetadataSynced = false;
    _playerStatus = PLAYBACK_STATUS::NOT_FOUND;
    _pCommandInterface = &CommandInterface::getInstance();
    setPlayerName(playerName);
}

DBusPlayerInfo::~DBusPlayerInfo ()
{
}

void DBusPlayerInfo::clearDBusPlayerData ()
{
    setPlayerStatus(PLAYBACK_STATUS::NOT_FOUND);
    setPlaybackMetadata("");
    setSongInfo("");
    toggleMetadataSyncState(false);
}

DBusPlayerInfo::PLAYBACK_STATUS DBusPlayerInfo::updatePlayerStatus ()
{
    // Sanity check for CommandInterface singleton
    if (_pCommandInterface == nullptr)
    {
        char errorMsg[COMMAND_BUFFER_SIZE];
        snprintf(errorMsg, COMMAND_BUFFER_SIZE, "%s[%s] [%s] [CommandInterface singleton is null!]%s\n", DBUS_PLAYER_INFO_COLOR, SY_DBUS_PLAYER_INFO, __func__, DBUS_PLAYER_INFO_COLOR_RESET);
        throw std::runtime_error(errorMsg);
        clearDBusPlayerData();
        return PLAYBACK_STATUS::NOT_FOUND;
    }

    // Sanity check for _playerName
    if (_playerName.empty())
    {
        char errorMsg[COMMAND_BUFFER_SIZE];
        snprintf(errorMsg, COMMAND_BUFFER_SIZE, "%s[%s] [%s] [%s] [Class playerName is empty!]%s\n", DBUS_PLAYER_INFO_COLOR, _pCommandInterface->getSystemClock().c_str(), SY_DBUS_PLAYER_INFO, __func__, DBUS_PLAYER_INFO_COLOR_RESET);
        throw std::runtime_error(errorMsg);
        clearDBusPlayerData();
        return PLAYBACK_STATUS::NOT_FOUND;
    }

    // Build dbus command
    const std::string dbusCommand = "dbus-send --print-reply --session \
                                    --dest=org.mpris.MediaPlayer2." + _playerName + " \
                                    /org/mpris/MediaPlayer2 \
                                    org.freedesktop.DBus.Properties.Get \
                                    string:org.mpris.MediaPlayer2.Player \
                                    string:PlaybackStatus \
                                    2>/dev/null";

    // Get player status
    std::string playerStatusResponse;
    _pCommandInterface->executeCommand(dbusCommand, playerStatusResponse);

    // Check if player is playing
    if (playerStatusResponse.find("Playing") != std::string::npos)
        setPlayerStatus(PLAYBACK_STATUS::PLAYING);
    else if (playerStatusResponse.find("Paused") != std::string::npos)
        setPlayerStatus(PLAYBACK_STATUS::PAUSED);
    else
        clearDBusPlayerData();

    return getPlayerStatus();
}

void DBusPlayerInfo::updateSongInfo ()
{
    // Attempt to find song title
    size_t songTitlePos;
    if ((songTitlePos = _playbackMetadata.find("xesam:title")) == std::string::npos)
    {
        char errorMsg[COMMAND_BUFFER_SIZE];
        snprintf(errorMsg, COMMAND_BUFFER_SIZE, "%s[%s] [%s] [ERROR: Song title not found!]%s\n", DBUS_PLAYER_INFO_COLOR, SY_DBUS_PLAYER_INFO, __func__, DBUS_PLAYER_INFO_COLOR_RESET);
        throw std::runtime_error(errorMsg);
        return setSongInfo("");
    }

    // Parse metadata -- song title
    size_t songTitleStartQuote = _playbackMetadata.find('"', songTitlePos + 12); // 12 = length of "xesam:title" +1
    size_t songTitleEndQuote = _playbackMetadata.find('"', songTitleStartQuote + 1);
    std::string songTitle = _playbackMetadata.substr(songTitleStartQuote + 1, songTitleEndQuote - songTitleStartQuote - 1);

    // Attempt to find song artists
    size_t songArtistsPos;
    if ((songArtistsPos = _playbackMetadata.find("xesam:artist")) == std::string::npos)
    {
        char errorMsg[COMMAND_BUFFER_SIZE];
        snprintf(errorMsg, COMMAND_BUFFER_SIZE, "%s[%s] [%s] [ERROR: Song artist(s) not found!]%s\n", DBUS_PLAYER_INFO_COLOR, SY_DBUS_PLAYER_INFO, __func__, DBUS_PLAYER_INFO_COLOR_RESET);
        throw std::runtime_error(errorMsg);
        return setSongInfo("");
    }

    // Parse metadata -- song artists
    songArtistsPos += 13;  // 13 = length of "xesam:artist" +1
    size_t songArtistsEndArray = _playbackMetadata.find("]", songArtistsPos);
    std::string songArtists;
    while (songArtistsPos < songArtistsEndArray)
    {
        size_t songArtistStartQuote = _playbackMetadata.find('"', songArtistsPos);
        size_t songArtistEndQuote = _playbackMetadata.find('"', songArtistStartQuote + 1);
        songArtists += _playbackMetadata.substr(songArtistStartQuote + 1, songArtistEndQuote - songArtistStartQuote - 1);

        if ((songArtistEndQuote + 17) < songArtistsEndArray)
            songArtists += ", ";
            
        songArtistsPos = songArtistEndQuote;
        break;
    }

    // Format the song metadata
    std::cout << "hello" << std::endl;
    _songInfo = getSongPrefix() + songArtists + getSongSeparator() + songTitle + getSongSuffix();
    std::cout << _songInfo << std::endl;
    return;
}

bool DBusPlayerInfo::updatePlaybackMetadata ()
{
    // Sanity check for CommandInterface singleton
    if (_pCommandInterface == nullptr)
    {
        char errorMsg[COMMAND_BUFFER_SIZE];
        snprintf(errorMsg, COMMAND_BUFFER_SIZE, "%s[%s] [%s] [CommandInterface singleton is null!]%s\n", DBUS_PLAYER_INFO_COLOR, SY_DBUS_PLAYER_INFO, __func__, DBUS_PLAYER_INFO_COLOR_RESET);
        throw std::runtime_error(errorMsg);
        clearDBusPlayerData();
        return false;
    }

    // Sanity check for _playerName
    if (_playerName.empty())
    {
        char errorMsg[COMMAND_BUFFER_SIZE];
        snprintf(errorMsg, COMMAND_BUFFER_SIZE, "%s[%s] [%s] [%s] [playerName is empty!]%s\n", DBUS_PLAYER_INFO_COLOR, _pCommandInterface->getSystemClock().c_str(), SY_DBUS_PLAYER_INFO, __func__, DBUS_PLAYER_INFO_COLOR_RESET);
        throw std::runtime_error(errorMsg);
        clearDBusPlayerData();
        return false;
    }

    // Build dbus command
    const std::string dbusCommand = "dbus-send --print-reply --session \
                                    --dest=org.mpris.MediaPlayer2." + _playerName + " \
                                    /org/mpris/MediaPlayer2 \
                                    org.freedesktop.DBus.Properties.Get \
                                    string:org.mpris.MediaPlayer2.Player \
                                    string:Metadata \
                                    2>/dev/null";

    std::string playerMetadataResponse;
    _pCommandInterface->executeCommand(dbusCommand, playerMetadataResponse);

    // Sanity check for playerMetadataResponse
    if (playerMetadataResponse.empty())
    {
        _playbackMetadata = playerMetadataResponse;
        setSongInfo("");
        toggleMetadataSyncState(true);
        return true;
    }

    // Check if _playbackMetadata is empty
    if (_playbackMetadata.empty())
    {
        _playbackMetadata = playerMetadataResponse;
        toggleMetadataSyncState(false);
        updateSongInfo();
        return true;
    }

    // Do not update if the retrieved metadata is the same as the previous instance - the header is always different
    if (_playbackMetadata.substr(103, _playbackMetadata.size()) == playerMetadataResponse.substr(103, playerMetadataResponse.size()))
        return false;

    // Update metadata
    _playbackMetadata = playerMetadataResponse;

    // Toggle metadata sync state and update song info
    toggleMetadataSyncState(false);
    updateSongInfo();

    return true;
}

