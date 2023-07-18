#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <any>



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
    void subscribe(const EventListener& listener)
    {
        GuardLocker lock(m_mutex);
        m_listeners[typeid(EventType).hash_code()].push_back(listener);
    }

    template <typename DerivedEvent>
    void publish(const DerivedEvent& event)
    {
        GuardLocker lock(m_mutex);
        const auto& eventListeners = m_listeners[typeid(DerivedEvent).hash_code()];
        for (const auto& listener : eventListeners)
        {
            listener(event);
        }
    }

private:
    // map<hash_code, listeners>
    std::unordered_map<size_t, std::vector<EventListener>> m_listeners;
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