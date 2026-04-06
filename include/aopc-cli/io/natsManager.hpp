#pragma once

#include <nats.h>
#include <string>
#include <vector>
#include <mutex>
#include <queue>


class NatsManager {
    public:
        NatsManager(const std::string& url);
        ~NatsManager();

        bool subscribe(const std::string& subject);

        std::vector<std::string> getMessages();

    private:
        natsConnection* connection = nullptr;
        natsSubscription* subscription = nullptr;
        natsStatus status;

        std::queue<std::string> messageQueue;
        std::mutex queueMutex;

        static void messageHandler(natsConnection* nc, natsSubscription* sub, natsMsg* msg, void* closure);
};