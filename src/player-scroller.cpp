#include "player-scroller.h"

PlayerScroller::PlayerScroller (const std::string& playerName) : DBusPlayerInfo(playerName)
{
    _viewerPos = 0;
    _viewerLength = 0;
}

PlayerScroller::PlayerScroller () : PlayerScroller("")
{
}

PlayerScroller::~PlayerScroller ()
{
}

void PlayerScroller::refreshMetadata ()
{
    // If the player is paused, reset the viewer
    if (getPlayerStatus() == DBusPlayerInfo::PLAYBACK_STATUS::PAUSED)
    {
        resetViewer();
        toggleMetadataSyncState(true);
        return;
    }

    // If metadata is already updated, do nothing
    if (getMetadataSyncState())
        return;

    // If updated metadata is of size 0, it means the player was closed
    std::cout << "songSz: " << getSongInfo().size() << std::endl;
    if (!getSongInfo().size())
    {
        _metadataBuffer = "";
        resetViewerPosition();
        toggleMetadataSyncState(true);
        return;
    }

    bool shouldResize = false;

    // Check if metadata is at least as big as the viewer
    if (getSongInfo().size() < getViewerLength())
        shouldResize = true;

    // Resize metadata if needed
    if (!shouldResize)
        _metadataBuffer = getSongInfo();
    else
        _metadataBuffer = getSongInfo() + std::string(getViewerLength() - getSongInfo().size(), SPACE_CHAR);

    // Toggle metadata update state
    toggleMetadataSyncState(true);

    // New data received, reset viewer position
    resetViewerPosition();
}

void PlayerScroller::initViewerLength (const size_t viewerLength)
{
    _viewerLength = viewerLength;

    // Check if metadata is not empty
    if (!getSongInfo().size())
        return;

    // Check if metadata is at least as big as the viewer
    if (getSongInfo().size() < viewerLength)
        resizeSongInfo(viewerLength);

    // Initialize metadata view
    if (getSongInfo().size() < viewerLength)
        _metadataBuffer = std::string(getSongInfo().c_str(), viewerLength);
    else
        _metadataBuffer = getSongInfo();
}

void PlayerScroller::metadataScroller ()
{
    // Only scroll if metadata update state is true
    if (!getMetadataSyncState())
        return;

    // Backup the first char of the viewer
    char firstChar = _metadataBuffer[0];

    // Shift the viewer to the left
    _metadataBuffer = _metadataBuffer.substr(1, _metadataBuffer.size() - 1) + firstChar;

    // Increment viewer position
    _viewerPos++;

    if (_viewerPos == _metadataBuffer.size())
        resetViewerPosition();
}

void PlayerScroller::resetViewer ()
{
    if (getViewerPosition() == 0 && getMetadataSyncState() == true)
        return;

    bool shouldResize = false;

    // Check if metadata is at least as big as the viewer
    if (getSongInfo().size() < getViewerLength())
        shouldResize = true;

    // Resize metadata if needed
    if (!shouldResize)
        _metadataBuffer = getSongInfo();
    else
        _metadataBuffer = getSongInfo() + std::string(getViewerLength() - getSongInfo().size(), SPACE_CHAR);

    // Reset viewer position
    resetViewerPosition();
}