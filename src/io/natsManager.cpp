#include "aopc-cli/io/natsManager.hpp"
#include <iostream>


NatsManager::NatsManager(const std::string& url) {
    status = natsConnection_ConnectTo(&connection, url.c_str());
    if (status != NATS_OK) {
        std::cerr << "Failed to connect to NATS server: " << natsStatus_GetText(status) << std::endl;
    }
}

NatsManager::~NatsManager() {
    if (subscription) {
        natsSubscription_Destroy(subscription);
    }
    if (connection) {
        natsConnection_Destroy(connection);
    }
}

void NatsManager::messageHandler(natsConnection* nc, natsSubscription* sub, natsMsg* msg, void* closure) {
    NatsManager* manager = static_cast<NatsManager*>(closure);
    std::string message(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));

    {
        std::lock_guard<std::mutex> lock(manager->queueMutex);
        manager->messageQueue.push(message);
        std::cout << "Received message: " << message << std::endl;
    }

    natsMsg_Destroy(msg);
}

bool NatsManager::subscribe(const std::string& subject) {
    status = natsConnection_Subscribe(&subscription, connection, subject.c_str(), messageHandler, this);
    return status == NATS_OK;
}

std::vector<std::string> NatsManager::getMessages() {
    std::vector<std::string> messages;

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        while (!messageQueue.empty()) {
            messages.push_back(messageQueue.front());
            messageQueue.pop();
        }
    }

    return messages;
}