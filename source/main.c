#define PLUGIN_ID_VERSION        "PluginId001"
#define PLUGIN_CALLBACKS_VERSION "PluginCallbacks001"
#define PLUGIN_NAME              "TF2VRFIX "
#define PLUGIN_CONTEXT_CLIENT    0x2

typedef enum {
    ID_NAME = 0,
    ID_LOG_NAME,
    ID_DEPENDENCY_NAME,
} PluginString;

typedef enum {
    ID_CONTEXT = 0,
} PluginField;

typedef enum {
    IFACE_OK = 0,
    IFACE_FAILED,
} InterfaceStatus;

typedef struct IPluginId {
    struct IPluginId_vftable {
        const char* (*GetString)(struct IPluginId* self, PluginString prop);
        i64 (*GetField)(struct IPluginId* self, PluginField prop);
    }* vftable;
} IPluginId;

typedef struct IPluginCallbacks {
    struct IPluginCallbacks_vftable {
        void (*Init)(struct IPluginCallbacks* self, HMODULE module, void* data, char reloaded);
        void (*Finalize)(struct IPluginCallbacks* self);
        u8 (*Unload)(struct IPluginCallbacks* self);
        void (*OnSqvmCreated)(struct IPluginCallbacks* self, void* sqvm);
        void (*OnSqvmDestroyed)(struct IPluginCallbacks* self, void* sqvm);
        void (*OnLibraryLoaded)(struct IPluginCallbacks* self, HMODULE module, const char* name);
        void (*RunFrame)(struct IPluginCallbacks* self);
    }* vftable;
} IPluginCallbacks;

static const char* GetString(IPluginId* self, PluginString prop) {
    switch (prop) {
        case ID_NAME:
        case ID_LOG_NAME:
        case ID_DEPENDENCY_NAME: return PLUGIN_NAME;
        default: return NULL;
    }
}

static i64 GetField(IPluginId* self, PluginField prop) {
    return prop == ID_CONTEXT ? PLUGIN_CONTEXT_CLIENT : 0;
}

static void Init(IPluginCallbacks* self, HMODULE module, void* data, char reloaded) {}
static void Finalize(IPluginCallbacks* self) { LoadHooks(); }
static void OnSqvmCreated(IPluginCallbacks* self, void* sqvm) {}
static void OnSqvmDestroyed(IPluginCallbacks* self, void* sqvm) {}
static void OnLibraryLoaded(IPluginCallbacks* self, HMODULE module, const char* name) {}
static void RunFrame(IPluginCallbacks* self) {}

static u8 Unload(IPluginCallbacks* self) { return 0; }

static IPluginId plugin_id = {
    .vftable = &(struct IPluginId_vftable) {.GetString = GetString, .GetField = GetField}
};

static IPluginCallbacks plugin_callbacks = {
    .vftable = &(struct IPluginCallbacks_vftable) {
                                                   .Init            = Init,
                                                   .Finalize        = Finalize,
                                                   .Unload          = Unload,
                                                   .OnSqvmCreated   = OnSqvmCreated,
                                                   .OnSqvmDestroyed = OnSqvmDestroyed,
                                                   .OnLibraryLoaded = OnLibraryLoaded,
                                                   .RunFrame        = RunFrame}
};

__declspec(dllexport) void* CreateInterface(const char* name, InterfaceStatus* status) {
    void* result = NULL;
    if (name && !strcmp(name, PLUGIN_ID_VERSION)) {
        result = &plugin_id;
    } else if (name && !strcmp(name, PLUGIN_CALLBACKS_VERSION)) {
        result = &plugin_callbacks;
    }
    if (status) {
        *status = result ? IFACE_OK : IFACE_FAILED;
    }
    return result;
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved) {
    return 1;
}
