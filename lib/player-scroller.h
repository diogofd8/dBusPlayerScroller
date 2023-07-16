#ifndef STRING_SCROLLER_H
#define STRING_SCROLLER_H

#include "dbus-player-info.h"

#define SY_PLAYER_SCROLLER "PlayerScroller"
#define PLAYER_SCROLLER_COLOR "\033[1;33m"
#define PLAYER_SCROLLER_COLOR_RESET "\033[0m"

class DBusPlayerInfo;

class PlayerScroller : public DBusPlayerInfo
{
    public:
        PlayerScroller();
        PlayerScroller(const std::string& playerName);
        ~PlayerScroller();
        void refreshMetadata();
        void initViewerLength(const size_t viewerLength);
        void metadataScroller();
        void resetViewer();
        inline int getViewerPosition () const {
            return _viewerPos;
        }
        inline size_t getViewerLength () const {
            return _viewerLength;
        }
        inline const std::string getViewerStr () const {
            if (!_metadataBuffer.size())
                return _metadataBuffer;
            return _metadataBuffer.substr(0, _viewerLength);
        }

    protected:
        inline void resetViewerPosition () {
            _viewerPos = 0;
        }
        inline const std::string& getMetadataBuffer () const {
            return _metadataBuffer;
        }
    private:
        std::string _metadataBuffer;
        size_t _viewerPos;
        size_t _viewerLength;
};

#endif // STRING_SCROLLER_H