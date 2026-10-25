// Exercise the actual runtime timer worker with synthetic RDRAM.
#include <cassert>
#include <string>
#include <vector>
#include "../../deps/N64ModernRuntime/ultramodern/src/timer.cpp"
void ultramodern::set_native_thread_name(const std::string&) {}
void ultramodern::set_native_thread_priority(ultramodern::ThreadPriority) {}
void ultramodern::enqueue_external_message_src(PTR(OSMesgQueue), OSMesg, bool, ultramodern::EventMessageSource) {}
int main(int argc, char**) {
    std::vector<uint8_t> memory(16 * 1024 * 1024);
    uint8_t* rdram = memory.data();
    if (argc > 1) {
        // A timer 24 hours away must not delay shutdown.
        osSetTimer(rdram, (int32_t)0x80001000, uint64_t(counter_per_ms) * 86400000, 0, 0, 0);
    }
    ultramodern::init_timers(rdram);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    const auto start = std::chrono::steady_clock::now();
    ultramodern::join_timer_thread();
    assert(std::chrono::steady_clock::now() - start < std::chrono::seconds(1));
    assert(!timer_context.thread.joinable());
    ultramodern::join_timer_thread(); // Safe if shutdown is requested again.
}
