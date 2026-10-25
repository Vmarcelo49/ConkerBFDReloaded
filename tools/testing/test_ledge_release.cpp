// Focused host-side regression tests; no ROM or save data is used.
#include <cassert>
#include <vector>
#include "../../host/src/ledge_grab.cpp"

namespace {
bool enabled = true;
uint16_t buttons = 0;
int animation_calls = 0;
}
bool conker::qol::ledge_grab() { return enabled; }
uint16_t conker::qol::get_player_buttons() { return buttons; }
double conker::testing::game_seconds() { return 0.0; }
extern "C" void func_1505E650(uint8_t* rdram, recomp_context* ctx) {
    assert((int32_t)ctx->r4 == player);
    assert(ctx->r5 == falling_animation);
    MEM_W(0, (gpr)(player + 0x84)) = falling_animation << 16;
    ++animation_calls;
}
int main() {
    std::vector<uint8_t> memory(16 * 1024 * 1024);
    uint8_t* rdram = memory.data();
    recomp_context ctx{};
    ctx.r29 = (gpr)(int32_t)0x807FF000;
    auto reset = [&](uint16_t anim, uint16_t input, bool option = true) {
        fall = {}; hold = {}; enabled = option; buttons = input; animation_calls = 0;
        MEM_W(0, (gpr)(player + 0x84)) = anim << 16;
        MEM_W(0, (gpr)(int32_t)0x800E0A90) = 100;
        MEM_B(0x100, (gpr)player) = 0x10;
        set_real(rdram, player + 0x18, 300.0f);
    };
    reset(hanging_animation, 0);
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 0);
    buttons = 0x4000;
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 1 && animation(rdram) == falling_animation);
    assert(MEM_BU(0x100, (gpr)player) == 0x11);
    assert(real(rdram, player + 0x20) == 0.0f);
    assert(real(rdram, player + 0x24) == walking_gravity);
    assert(real(rdram, player + 0x1CC) == 300.0f && fall.caught && !hold.on);
    ctx.r7 = (gpr)player; ctx.r8 = 0x0E000000;
    conker_ledge_grab_mask(rdram, &ctx);
    assert(ctx.r8 == 0x0E000000); // Released falls cannot reopen the grab search.
    MEM_B(0x100, (gpr)player) = 0x01;
    conker_ledge_grab_mask(rdram, &ctx);
    assert(!fall.caught); // Landing permits the next grab.
    reset(hanging_animation, 0x4000);
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 0); // An attack held before catching must not drop Conker.
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 0); // Continuing to hold B is not a fresh press.
    buttons = 0;
    conker_ledge_grab_watchdog(rdram, &ctx);
    buttons = 0x4000;
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 1); // Release and press again while hanging.
    reset(hanging_animation, 0x4000);
    conker_ledge_grab_watchdog(rdram, &ctx);
    MEM_W(0, (gpr)(player + 0x84)) = falling_animation << 16;
    buttons = 0;
    conker_ledge_grab_watchdog(rdram, &ctx);
    MEM_W(0, (gpr)(player + 0x84)) = hanging_animation << 16;
    buttons = 0x4000;
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 0); // A new hold must arm independently.
    for (auto anim : { climbing_animation, hopping_animation, falling_animation }) {
        reset(anim, 0x4000);
        conker_ledge_grab_watchdog(rdram, &ctx);
        assert(animation_calls == 0);
    }
    reset(hanging_animation, 0x4000, false);
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 0); // Original behavior with option disabled.
    reset(hanging_animation, 0);
    conker_ledge_grab_watchdog(rdram, &ctx);
    MEM_W(0, (gpr)(int32_t)0x800E0A90) = 148;
    set_real(rdram, player + 0x18, 250.0f);
    conker_ledge_grab_watchdog(rdram, &ctx);
    assert(animation_calls == 1 && fall.caught); // Existing safety net remains intact.
    std::puts("All ledge-release regression tests passed.");
}
