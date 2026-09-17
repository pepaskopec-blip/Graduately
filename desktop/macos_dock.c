#include "maturita.h"

#ifdef __APPLE__
#include <objc/message.h>
#include <objc/runtime.h>

/* Set the macOS Dock tile icon. GTK's window icon-name is ignored on
 * Quartz, so we talk to AppKit directly via the Objective-C runtime. */
void macos_set_dock_icon(const char *path) {
    Class nsstring;
    Class nsimage;
    Class nsapp;
    id path_str;
    id image;
    id app;

    if (!path || !path[0])
        return;

    nsstring = objc_getClass("NSString");
    nsimage = objc_getClass("NSImage");
    nsapp = objc_getClass("NSApplication");
    if (!nsstring || !nsimage || !nsapp)
        return;

    path_str = ((id (*)(Class, SEL, const char *))objc_msgSend)(
        nsstring, sel_registerName("stringWithUTF8String:"), path);
    if (!path_str)
        return;

    /* NSImage has -initWithContentsOfFile:, not +imageWithContentsOfFile:. */
    image = ((id (*)(id, SEL, id))objc_msgSend)(
        ((id (*)(Class, SEL))objc_msgSend)(
            nsimage, sel_registerName("alloc")),
        sel_registerName("initWithContentsOfFile:"),
        path_str);
    if (!image)
        return;

    app = ((id (*)(Class, SEL))objc_msgSend)(
        nsapp, sel_registerName("sharedApplication"));
    if (!app) {
        ((void (*)(id, SEL))objc_msgSend)(image, sel_registerName("release"));
        return;
    }

    ((void (*)(id, SEL, id))objc_msgSend)(
        app, sel_registerName("setApplicationIconImage:"), image);
    ((void (*)(id, SEL))objc_msgSend)(image, sel_registerName("release"));
}
#else
void macos_set_dock_icon(const char *path) {
    (void)path;
}
#endif
