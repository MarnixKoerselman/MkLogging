#pragma once

#include <mutex>
#include <list>

template <typename EventNotificationInterface>
class CEventSource
{
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<std::recursive_mutex>;

public:
    CEventSource() = default;

    CEventSource(const std::list<std::shared_ptr<EventNotificationInterface>>& pListeners)
        : m_pListeners(pListeners)
    {
    }

    virtual ~CEventSource() = default;

    void AddListener(const std::shared_ptr<EventNotificationInterface>& pListener)
    {
        Lock lock(m_AccessListeners);
        m_pListeners.push_back(pListener);
    }

    void RemoveListener(const std::shared_ptr<EventNotificationInterface>& pListener)
    {
        Lock lock(m_AccessListeners);
        auto pos = std::find(m_pListeners.begin(), m_pListeners.end(), pListener);
        if (pos != m_pListeners.end())
        {
            m_pListeners.erase(pos);
        }
    }

protected:
    std::list<std::shared_ptr<EventNotificationInterface>> m_pListeners;
    Mutex m_AccessListeners;
};