#include "thread_annotations.h"
#include <mutex>
#include <type_traits>

class CAPABILITY("mutex") mutex
{
  public:
    mutex() : mu_{} {}
    ~mutex() EXCLUDES(this) = default;

    // clang-format off
    inline void lock() ACQUIRE() {mu_.lock();}
    inline void unlock() RELEASE() {mu_.unlock();}
    inline bool try_lock() TRY_ACQUIRE(true) {return mu_.try_lock();}
    // clang-format on

  private:
    std::mutex mu_;
    DELETE_COPY_MOVE(mutex);
};

template <typename Guard>
class SCOPED_CAPABILITY scope_guard
{
  public:
    static_assert(std::is_same<Guard, typename std::decay<Guard>::type>::value);

    scope_guard(Guard &guard) ACQUIRE(guard) : guard_ref_{guard}
    {
        guard_ref_.lock();
    }

    ~scope_guard() RELEASE() { guard_ref_.unlock(); }

  private:
    Guard &guard_ref_;
    DELETE_COPY_MOVE(scope_guard);
};

#if __cplusplus >= 201703L
template <typename Guard>
scope_guard(Guard &) -> scope_guard<std::decay_t<Guard>>;
#endif // __cplusplus == 201703L
