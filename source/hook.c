#define TARGET_PATTERN                                                \
    "40 55 53 56 57 41 56 41 57 48 8D 6C 24 F8 "                      \
    "48 81 EC 08 01 00 00 48 8B 05 ?? ?? ?? ?? 48 33 C4 48 89 45 F0 " \
    "49 8B F9 45 8B F0 48 8B DA 48 8B F1 4D 85 C9 7F 35"

typedef i32(__stdcall ConvertQpcType)(void* instance, const LARGE_INTEGER* qpc, i64* time);

typedef struct {
    Pad(0xB0);
    ConvertQpcType* convert_performance_counter_to_time;
    Pad(0x218 - 0xB8);
    void* instance;
} XrContext;

#define HAND_MOTION(name) void* name(XrContext* ctx, void* out, u32 hand, i64 time)
typedef HAND_MOTION(HandMotionType);

static HandMotionType* hand_motion;

static HAND_MOTION(HandMotionHook) {
    if (time <= 0) {
        LARGE_INTEGER qpc;
        QueryPerformanceCounter(&qpc);
        i64 fresh = 0;
        i32 rc    = ctx->convert_performance_counter_to_time(ctx->instance, &qpc, &fresh);
        if (rc >= 0 && fresh > 0) {
            DebugMsg("time 0 to %lld", fresh);
            time = fresh;
        } else {
            DebugMsg("convert failed (wtf?!): code %d", rc);
        }
    }
    return hand_motion(ctx, out, hand, time);
}

static void LoadHooks(void) {
    HMODULE mod = GetModuleHandleA("Titanfall2VR.dll");
    if (!mod) {
        DebugMsg("Titanfall2VR.dll not loaded");
        return;
    }

    u8* fn = FindUnique(mod, S8Lit(TARGET_PATTERN));
    if (!fn) {
        return;
    }

    MH_STATUS status;
    if ((status = MH_Initialize()) != MH_OK ||
        (status = MH_CreateHook(fn, (LPVOID) &HandMotionHook, (LPVOID*) &hand_motion)) != MH_OK ||
        (status = MH_EnableHook(fn)) != MH_OK) {
        DebugMsg("MinHook failed: %d", status);
        return;
    }
    DebugMsg("Hooked Titanfall2VR.dll+0x%llX", (unsigned long long) (fn - (u8*) mod));
}
