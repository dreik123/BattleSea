#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include "Core/CoreDefines.h"


#if EVENTS_STD_ANY_APPROACH
#include <any>
#else
#include <type_traits>

#include "Event.h"
template<typename T>
concept DerivedEvent = std::derived_from<T, Event>;
#endif // EVENTS_STD_ANY_APPROACH


// Very simple version of event bus, without any mutexes, streams, channels
// TODO make it thread safety if view is going to be in separate thread
class EventBus
{
public:
#if EVENTS_STD_ANY_APPROACH
    using EventListener = std::function<void(const std::any&)>;
#else
    using EventListener = std::function<void(const Event&)>;
#endif // EVENTS_STD_ANY_APPROACH

    EventBus() = default;
    ~EventBus() = default;

    // non-copyable type
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    EventBus(EventBus&& other) noexcept(true)
    {
        m_listeners = std::move(other.m_listeners);
    }
    EventBus& operator=(EventBus&& other) noexcept(true)
    {
        if (this == &other)
        {
            return *this;
        }

        m_listeners = std::move(other.m_listeners);
    }

    template <typename EventType>
    void subscribe(const EventListener& listener)
    {
        m_listeners[typeid(EventType).hash_code()].push_back(listener);
    }

    template <typename DerivedEvent>
    void publish(const DerivedEvent& event)
    {
        const auto& eventListeners = m_listeners[typeid(DerivedEvent).hash_code()];
        for (const auto& listener : eventListeners)
        {
            listener(event);
        }
    }

private:
    // map<hash_code, listeners>
    std::unordered_map<size_t, std::vector<EventListener>> m_listeners;
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
#if !EVENTS_STD_ANY_APPROACH
    : public Event
#endif
{
    int value = 0;
};


EventBus eventBus;


#if EVENTS_STD_ANY_APPROACH
eventBus.subscribe<TestEvent>([](const std::any& event)
    {
        auto evt = std::any_cast<TestEvent>(event);
        std::cout << "Value from event: " << evt.value << std::endl;
    });
#else
eventBus.subscribe<TestEvent>([](const Event& event)
    {
        // Can't use dynamic_cast since Event is not polymorphic type (no virtual method inside)
        const TestEvent& castEvent = static_cast<const TestEvent&>(event);
        std::cout << "Value from event: " << castEvent.value << std::endl;
    });
#endif
//////////////////////

TestEvent someEvent{ .value = 4 };
eventBus.publish(someEvent);

#endif