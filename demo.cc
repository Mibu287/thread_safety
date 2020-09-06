#include "mutex.h"
#include "shared_mutex.h"
#include "spin_lock.h"
#include <array>
#include <iostream>
#include <thread>
#include <utility>

class Foo
{
  public:
    explicit Foo() : Foo{0l} {}
    explicit Foo(long value) : lck_{}, value_{value} {}
    ~Foo() EXCLUDES(lck_) = default;

    long
    get() const
    {
        scope_guard guard(lck_);
        return value_;
    }

    void
    set(long value)
    {
        scope_guard guard(lck_);
        value_ = value;
    }

    void
    operator++()
    {
        scope_guard guard(lck_);
        ++value_;
    }

    void
    incr(long limit)
    {
        while (true)
        {
            scope_guard guard(lck_);
            if (value_ >= limit)
                return;
            ++value_;
        }
    }

  private:
    mutable SpinLock lck_;
    long value_ GUARDED_BY(lck_);

  private:
    DELETE_COPY_MOVE(Foo);
};

constexpr long MEANING_OF_LIFE = 42;

void
test_spin_lock()
{
    Foo foo{0};
    constexpr std::size_t N_THREADS = 4;
    std::array<std::thread, N_THREADS> thread_pool{};

    for (auto &t : thread_pool)
    {
        t = std::thread{[&foo](long limit) { foo.incr(limit); },
                        MEANING_OF_LIFE};
    }

    for (auto &t : thread_pool)
    {
        if (t.joinable())
            t.join();
    }

    if (foo.get() != MEANING_OF_LIFE)
        std::cerr << "foo.get() is not MEANING_OF_LIFE\n";
}

int
main()
{
    const std::size_t N_LOOPS = 100'000;
    for (std::size_t i = 0; i < N_LOOPS; ++i)
        test_spin_lock();
}
