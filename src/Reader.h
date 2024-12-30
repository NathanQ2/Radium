#pragma once

#include <functional>
#include <optional>
#include <vector>

template<typename T>
class Reader {
public:
    explicit Reader(const std::vector<T>& buff) : m_buff(buff), m_i(0)
    {
    }
    
    std::optional<T> peek(const int ahead = 0)
    {
        if (m_i + ahead < 0 || m_i + ahead >= m_buff.size())
            return std::nullopt;
        
        return m_buff[m_i + ahead];
    }

    bool peekAnd(std::function<bool(const T&)> condition, const int ahead = 0)
    {
        if (auto val = peek(ahead))
        {
            return condition(val.value());
        }

        return false;
    }

    std::optional<T> at(const int i)
    {
        if (i < 0 || i >= m_buff.size())
            return std::nullopt;
        
        return m_buff[i];
    }
    
    T consume()
    {
        return m_buff[m_i++];
    }

    int consumeUntil(std::function<bool(const T&)> condition)
    {
        int amt = 0;
        while (peekAnd(condition))
        {
            amt++;
            consume();
        }

        return amt;
    }

    const std::vector<T>& get()
    {
        return m_buff;
    }

    [[nodiscard]] int getPos() const 
    {
        return m_i;
    }
private:
    std::vector<T> m_buff;
    size_t m_i;
};
