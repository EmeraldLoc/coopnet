#pragma once

#include <thread>
#include <map>
#include <mutex>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "connection.hpp"
#include "lobby.hpp"

class Server {
    private:
        std::thread mThreadRecv;
        std::thread mThreadUpdate;
        int mSocket;
        std::map<uint64_t, Connection*> mClients;
        std::mutex mClientsMutex;
        std::map<uint64_t, Lobby*> mLobbies;
        uint64_t mNextClientId = 1;
        uint64_t mNextLobbyId = 1;

    public:
        bool Begin(uint32_t aPort);
        void Receive();
        void Update();

        Lobby* LobbyGet(uint64_t aLobbyId);
        void LobbyListGet(Connection& aConnection, std::string aGame);

        void OnClientDisconnect(Connection* aConnection);
        void OnLobbyJoin(Lobby* aLobby, Connection* aConnection);
        void OnLobbyLeave(Lobby* aLobby, Connection* aConnection);
        void OnLobbyDestroy(Lobby* aLobby);

        void LobbyCreate(Connection* aConnection, std::string& aGame, std::string& aVersion, std::string& aTitle, uint16_t aMaxConnections);
};

extern Server* gServer;
