#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <any>

using ListenerHandleId = size_t;

// Simple version of event bus, without streams, channels
class EventBus
{
    using GuardLocker = std::lock_guard<std::recursive_mutex>;
public:
    using EventListener = std::function<void(const std::any&)>;

    EventBus() = default;
    ~EventBus() = default;

    // non-copyable type
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    EventBus(EventBus&& other) noexcept(true)
    {
        GuardLocker lock(m_mutex);
        m_listeners = std::move(other.m_listeners);
    }
    EventBus& operator=(EventBus&& other) noexcept(true)
    {
        GuardLocker lock(m_mutex);
        if (this == &other)
        {
            return *this;
        }

        m_listeners = std::move(other.m_listeners);
    }

    template <typename EventType>
    ListenerHandleId subscribe(const EventListener& listener)
    {
        m_mutex.lock();
        const ListenerHandleId listenerHandleId = generateListenerId();
        m_listeners[typeid(EventType).hash_code()].emplace_back(listener, listenerHandleId);
        m_mutex.unlock();

        return listenerHandleId;
    }

    void unsubscribe(const ListenerHandleId& handleId)
    {
        GuardLocker lock(m_mutex);
        for (auto& [typeHash, eventListeners] : m_listeners)
        {
            auto it = std::find_if(eventListeners.begin(), eventListeners.end(),
                [&handleId](const auto& pair) { return pair.second == handleId; });

            if (it != eventListeners.end())
            {
                eventListeners.erase(it);

                // Don't see significant improvement in removing empty bucket per type, but just to keep code clean let's do it
                if (eventListeners.empty())
                {
                    m_listeners.erase(typeHash); // iterators got invalidated, but execution is interrupted with break
                }
                break;
            }
        }
    }

    template <typename EventType>
    void publish(const EventType& event)
    {
        GuardLocker lock(m_mutex);
        const auto& eventListeners = m_listeners[typeid(EventType).hash_code()];
        for (const auto& [listener, _] : eventListeners)
        {
            listener(event);
        }
    }

private:
    // Pair of listener and unique identifier
    using ListenerHandle = std::pair<EventListener, ListenerHandleId>;

    // Generates a unique identifier for each listener
    ListenerHandleId generateListenerId() const
    {
        static ListenerHandleId idCounter = 0;
        return idCounter++;
    }

private:
    // map<hash_code, listeners>
    std::unordered_map<size_t, std::vector<ListenerHandle>> m_listeners;
    std::recursive_mutex m_mutex; // publish can trigger another public call
};


#if 0 // tests
    EventBus bus;
    //EventBus bus2(bus); // error
    EventBus bus3(EventBus()); // ok
    EventBus bus4(std::move(bus)); // ok
    bus4 = std::move(bus4); // ok
#endif


#if 0 // examples
struct TestEvent final
{
    int value = 0;
};


EventBus eventBus;

eventBus.subscribe<TestEvent>([](const std::any& event)
    {
        auto evt = std::any_cast<TestEvent>(event);
        std::cout << "Value from event: " << evt.value << std::endl;
    });


TestEvent someEvent{ .value = 4 };
eventBus.publish(someEvent);

#endif