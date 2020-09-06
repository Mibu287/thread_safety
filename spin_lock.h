#include "thread_annotations.h"
#include <atomic>
#include <cassert>

class CAPABILITY("mutex") SpinLock
{
  public:
    SpinLock() : flag_{UNLOCKED} {}
    ~SpinLock() EXCLUDES(this) = default;

    void lock() ACQUIRE();
    void unlock() RELEASE();
    bool try_lock() TRY_ACQUIRE(true);

    static constexpr int UNLOCKED = 0;
    static constexpr int LOCKED = 1;

  private:
    std::atomic_int flag_;
    DELETE_COPY_MOVE(SpinLock);
};

void
SpinLock::lock()
{
    int unlocked = SpinLock::UNLOCKED;
    while (!flag_.compare_exchange_weak(unlocked, SpinLock::LOCKED))
    {
        unlocked = SpinLock::UNLOCKED;
    }
}

void
SpinLock::unlock()
{
    assert(flag_.load() == SpinLock::LOCKED);
    flag_.store(SpinLock::UNLOCKED);
}

bool
SpinLock::try_lock()
{
    int unlocked = SpinLock::UNLOCKED;
    return flag_.compare_exchange_strong(unlocked, SpinLock::LOCKED);
}
