#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "connection.hpp"

class Connection;

class Lobby {
    private:
    public:
        bool mActive;
        Connection* mOwner;
        uint64_t mId;
        std::vector<Connection*> mConnections;
        uint16_t mMaxConnections;

        std::string mGame;
        std::string mVersion;
        std::string mTitle;

        Lobby(Connection* aOwner, uint64_t aId, std::string& aGame, std::string& aVersion, std::string& aTitle, uint16_t aMaxConnections);
        ~Lobby();

        bool Join(Connection* aConnection);
        void Leave(Connection* aConnection);
};

// callbacks
extern void (*gOnLobbyJoin)(Lobby* lobby, Connection* connection);
extern void (*gOnLobbyLeave)(Lobby* lobby, Connection* connection);
extern void (*gOnLobbyDestroy)(Lobby* lobby);
