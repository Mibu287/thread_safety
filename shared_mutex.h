#include "thread_annotations.h"
#include <shared_mutex>
#include <type_traits>

class CAPABILITY("mutex") shared_mutex
{
  public:
    shared_mutex() : mu_{} {}
    ~shared_mutex() EXCLUDES(this) = default;

    // clang-format off
    inline void lock() ACQUIRE() { mu_.lock(); }
    inline void unlock() RELEASE() { mu_.unlock(); }
    inline bool try_lock() TRY_ACQUIRE(true) { return mu_.try_lock(); }

    inline void lock_shared() ACQUIRE_SHARED() { mu_.lock_shared(); }
    inline void unlock_shared() RELEASE_SHARED() { mu_.unlock_shared(); }
    inline bool try_lock_shared() TRY_ACQUIRE_SHARED(true) { return mu_.try_lock_shared(); }
    // clang-format on

  private:
    std::shared_mutex mu_;
    DELETE_COPY_MOVE(shared_mutex);
};

template <typename Guard>
class SCOPED_CAPABILITY shared_scope_guard
{
  public:
    static_assert(std::is_same<Guard, typename std::decay<Guard>::type>::value);

    shared_scope_guard(Guard &guard) ACQUIRE_SHARED(guard)
        : shared_guard_ref_{guard}
    {
        shared_guard_ref_.lock_shared();
    }

    ~shared_scope_guard() RELEASE() { shared_guard_ref_.unlock_shared(); }

  private:
    Guard &shared_guard_ref_;
    DELETE_COPY_MOVE(shared_scope_guard);
};

#if __cplusplus >= 201703L
template <typename Guard>
shared_scope_guard(Guard &) -> shared_scope_guard<std::decay_t<Guard>>;
#endif //__cplusplus == 201703L
