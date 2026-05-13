//
// Created by SyperOlao on 13.05.2026.
//

#ifndef PINGPONG_IGAMEHOST_H
#define PINGPONG_IGAMEHOST_H

#include <memory>

class IGame;

class IGameHost {
public:
    virtual ~IGameHost() = default;

    virtual void RequestSwitchGame(std::unique_ptr<IGame> nextGame) = 0;

    virtual void RequestQuitApplication(const char *quitSource = nullptr) = 0;
};

#endif
