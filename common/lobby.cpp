#include <cstdint>
#include <string>
#include <thread>
#include <algorithm>

#include "lobby.hpp"
#include "logging.hpp"
#include "utils.hpp"

// callbacks
void (*gOnLobbyJoin)(Lobby* lobby, Connection* connection) = nullptr;
void (*gOnLobbyLeave)(Lobby* lobby, Connection* connection) = nullptr;
void (*gOnLobbyDestroy)(Lobby* lobby) = nullptr;

Lobby::Lobby(Connection* aOwner, uint64_t aId, std::string& aGame, std::string& aVersion, std::string& aTitle, uint16_t aMaxConnections) {
    mOwner = aOwner;
    mId = aId;
    mGame = aGame;
    mVersion = aVersion;
    mTitle = aTitle;
    mMaxConnections = aMaxConnections;
}

Lobby::~Lobby() {
    LOG_INFO("Destroying lobby %" PRIu64 "", mId);

    for (auto& it : mConnections) {
        this->Leave(it);
    }

    if (gOnLobbyDestroy) { gOnLobbyDestroy(this); }
}

enum MPacketErrorNumber Lobby::Join(Connection* aConnection) {
    // sanity check
    if (!aConnection) { return MERR_LOBBY_JOIN_FAILED; }
    if (aConnection->mLobby == this) { return MERR_LOBBY_JOIN_FAILED; }

    // leave older lobby
    if (aConnection->mLobby != nullptr) {
        aConnection->mLobby->Leave(aConnection);
    }

    // find out if we're already in this
    if (mConnections.size() >= mMaxConnections) {
        return MERR_LOBBY_JOIN_FULL;
    }

    auto it = std::find(mConnections.begin(), mConnections.end(), aConnection);
    if (it == mConnections.end()) {
        mConnections.push_back(aConnection);
    }
    aConnection->mLobby = this;
    aConnection->mPriority = mNextPriority++;

    if (gOnLobbyJoin) { gOnLobbyJoin(this, aConnection); }
    return MERR_NONE;
}

void Lobby::Leave(Connection* aConnection) {
    if (!aConnection) { return; }
    if (aConnection->mLobby != this) { return; }

    if (gOnLobbyLeave) { gOnLobbyLeave(this, aConnection); }

    mConnections.erase(std::remove(mConnections.begin(), mConnections.end(), aConnection), mConnections.end());

    aConnection->mLobby = nullptr;

    if (mOwner == aConnection) {
        delete this;
    }
}