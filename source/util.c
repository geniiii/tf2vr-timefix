static HANDLE log_file = INVALID_HANDLE_VALUE;

static u32 S8Equals(String8 a, String8 b) {
    return a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
}

static void LogOpen(HMODULE module) {
    char    path[MAX_PATH];
    String8 s = {
        .s    = (u8*) path,
        .size = GetModuleFileNameA(module, path, sizeof path),
    };
    if (s.size < 4) {
        return;
    }
    // NOTE(geni): Replace .dll extension with .log
    memcpy(s.s + s.size - 3, "log", 3);
    log_file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

static void DebugMsg(const char* fmt, ...) {
    if (log_file == INVALID_HANDLE_VALUE) {
        return;
    }

    SYSTEMTIME t;
    GetLocalTime(&t);

    char buf[512];
    i32  size = snprintf(buf, sizeof buf, "[%04d-%02d-%02d %02d:%02d:%02d] ", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

    va_list args;
    va_start(args, fmt);
    size += vsnprintf(buf + size, sizeof buf - size - 1, fmt, args);
    va_end(args);
    if (size > (i32) sizeof buf - 2) {
        size = sizeof buf - 2;
    }
    buf[size++] = '\n';

    String8 line = {
        .s    = (u8*) buf,
        .size = (u64) size,
    };
    DWORD written;
    WriteFile(log_file, line.data, (DWORD) line.size, &written, NULL);
}

static u8* FindUnique(HMODULE mod, String8 pattern) {
    // NOTE(geni): simdsc buffer sizes must be multiples of 32
    u8  compiled[64] = {0};
    u8  mask[64]     = {0};
    u64 pattern_size;
    if (simdsc_compile_signature(pattern, compiled, sizeof compiled, mask, sizeof mask, &pattern_size) != SIMDSC_RESULT_SUCCESS) {
        DebugMsg("failed to compile pattern");
        return NULL;
    }

    u8*                   base = (u8*) mod;
    IMAGE_NT_HEADERS64*   nt   = (IMAGE_NT_HEADERS64*) (base + ((IMAGE_DOS_HEADER*) base)->e_lfanew);
    IMAGE_SECTION_HEADER* sec  = IMAGE_FIRST_SECTION(nt);

    u8* found = NULL;
    i32 count = 0;
    for (u32 i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
        if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)) {
            continue;
        }
        u8* cur  = base + sec->VirtualAddress;
        u64 left = sec->Misc.VirtualSize;
        u64 offset;
        while (simdsc_auto_pattern_match(cur, left, mask, sizeof mask, compiled, sizeof compiled, pattern_size, &offset) == SIMDSC_RESULT_SUCCESS) {
            found = cur + offset;
            cur += offset + 1;
            left -= offset + 1;
            ++count;
        }
    }

    if (count != 1) {
        DebugMsg("pattern matched %d times but expected once", count);
        return NULL;
    }
    return found;
}
