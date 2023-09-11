#pragma once

#include <iostream>

#include <mutex>
#include <functional>
#include <condition_variable>

#include "ThreadPool.h"
#include "nlohmann/json.hpp"

#define  MQ_THREAD_MAX     3
#define  MQ_THREAD_MIN     1

#define  SQ_THREAD_MAX     3
#define  SQ_THREAD_MIN     1

namespace CommonLib {

template<typename Message>
struct Subscriber {
    std::string topic;
    std::function<void(const Message&)> callback;
};

template<typename Message, typename Response>
struct Responder {
    std::string topic;
    std::function<void(const Message&, std::function<void(const Response&)>)> callback;
};

template<typename Message>
class MessageQueue {
public:
    MessageQueue() : threadPool(MQ_THREAD_MIN, MQ_THREAD_MAX) {}
    MessageQueue(const MessageQueue&) = delete;
    MessageQueue& operator=(const MessageQueue&) = delete;

    static MessageQueue<Message>& getInstance() {
        static MessageQueue<Message> instance;
        return instance;
    }

    // 订阅在初始化处完成
    void subscribe(const std::string& topic, const std::function<void(const Message&)>& callback) {
        std::lock_guard<std::mutex> lock(mutex);
        subscribers.push_back({ topic, callback });
    }

    void publish(const std::string& topic, const Message& message) {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& subscriber : subscribers) {
            if (subscriber.topic == topic) {
                threadPool.Add(subscriber.callback, message);
            }
        }
    }

private:

public:
    std::vector<Subscriber<Message>> subscribers;
    ThreadPool threadPool;
    std::mutex mutex;
};

template<typename Message, typename Response>
class ServiceQueue {
public:
    ServiceQueue() : threadPool(SQ_THREAD_MIN, SQ_THREAD_MAX) {}
    ServiceQueue(const ServiceQueue&) = delete;
    ServiceQueue& operator=(const ServiceQueue&) = delete;

    static ServiceQueue<Message, Response>& getInstance() {
        static ServiceQueue<Message, Response> instance;
        return instance;
    }
    
    void service_subscribe(const std::string& topic, const std::function<void(const Message&, std::function<void(const Response&)>)>& callback) {
        std::lock_guard<std::mutex> lock(mutex);
        responders.push_back({ topic, callback });
    }

    std::future<Response> service_publish(const std::string& topic, const Message& message) {
        auto prom = std::make_shared<std::promise<Response>>();
        auto future = prom->get_future();

        std::function<void(const Message&, std::function<void(const Response&)>)> toCall;

        {
            std::lock_guard<std::mutex> lock(mutex);
            for (const auto& responder : responders) {
                if (responder.topic == topic) {
                    toCall = responder.callback;
                    break;  // Assuming only one responder per topic for simplicity
                }
            }
        }

        if (toCall) {
            threadPool.Add([&, callback=toCall, prom](const Message& msg) {
                        callback(msg, [prom](const Response& resp) {
                            prom->set_value(resp);
                        });
                    }, message);
        }

        return future;
    }

private:
    std::vector<Responder<Message, Response>> responders;
    ThreadPool threadPool;
    std::mutex mutex;
};

}
