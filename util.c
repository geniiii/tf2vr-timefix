static void DebugMsg(const char* fmt, ...) {
    time_t     t  = time(NULL);
    struct tm* tm = localtime(&t);
    char       time_buf[64];
    strftime(time_buf, sizeof time_buf, "%c", tm);

    char    buf[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof buf, fmt, args);
    va_end(args);

    FILE* f = fopen("tf2vr_timefix.log", "a");
    if (f) {
        fprintf(f, "[%s] %s\n", time_buf, buf);
        fclose(f);
    }
}

static u8* FindUnique(HMODULE mod, String8 pattern) {
    u8*                   base = (u8*) mod;
    IMAGE_NT_HEADERS64*   nt   = (IMAGE_NT_HEADERS64*) (base + ((IMAGE_DOS_HEADER*) base)->e_lfanew);
    IMAGE_SECTION_HEADER* sec  = IMAGE_FIRST_SECTION(nt);

    u8* found = NULL;
    i32 count = 0;
    for (u32 i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
        if (!(sec->Characteristics & IMAGE_SCN_MEM_EXECUTE)) {
            continue;
        }
        u8* data = base + sec->VirtualAddress;
        u64 size = sec->Misc.VirtualSize;
        u64 pos  = 0;
        u64 offset;
        while (pos < size && simdsc_easy_find(data + pos, size - pos, pattern, &offset) == SIMDSC_RESULT_SUCCESS) {
            found = data + pos + offset;
            pos += offset + 1;
            ++count;
        }
    }

    if (count != 1) {
        DebugMsg("pattern matched %d times but expected once", count);
        return NULL;
    }
    return found;
}
