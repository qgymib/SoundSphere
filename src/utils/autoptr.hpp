#ifndef SOUND_SPHERE_UTILS_AUTOPTR_HPP
#define SOUND_SPHERE_UTILS_AUTOPTR_HPP

#include <functional>
#include <memory>

namespace soundsphere
{

template <typename T> class auto_ptr
{
public:
    typedef std::function<void(T *)> DeleteFn;

public:
    /**
     * @brief Create a automatic pointer with initialize to nullptr.
     * @param[in] fn    Delete function.
     */
    auto_ptr(DeleteFn fn)
    {
        m_deleter = fn;
        m_obj = nullptr;
    }

    auto_ptr(const auto_ptr &orig) = delete;

    virtual ~auto_ptr()
    {
        reset();
    }

    void reset()
    {
        if (m_obj != nullptr)
        {
            m_deleter(m_obj);
            m_obj = nullptr;
        }
    }

public:
    T **operator&()
    {
        return &m_obj;
    }

    T *operator->()
    {
        return m_obj;
    }

private:
    DeleteFn m_deleter;
    T *m_obj;
};

class defer
{
public:
    typedef std::function<void()> DeferFn;

public:
    /**
     * @brief Create a defer object which execute function when destruction.
     * @param[in] fn    Function to execute.
     */
    defer(DeferFn fn)
    {
        m_fn = fn;
    }

    defer(const defer &orig) = delete;

    virtual ~defer()
    {
        m_fn();
    }

private:
    DeferFn m_fn;
};

} // namespace soundsphere

#endif
