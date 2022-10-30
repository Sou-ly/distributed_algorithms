#include <atomic>
#include <queue>
#include <mutex>
#include <optional>

namespace da
{
template<typename T>
class blocking_queue
{
    public:
        blocking_queue();
        ~blocking_queue();
        void push(const T& elem);
        std::optional<T&> pop();
        bool is_empty();
    private:
        std::mutex mutex;
        std::queue<T> queue;
};
}