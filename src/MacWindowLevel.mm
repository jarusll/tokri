#include "MacWindowLevel.h"

#import <Cocoa/Cocoa.h>

void MacWindowLevel::makeAlwaysOnTop(QWindow *w)
{
    if (!w)
        return;

    NSView *view = (__bridge NSView *)w->winId();
    if (!view)
        return;

    NSWindow *nswin = view.window;
    if (!nswin)
        return;

    NSLog(@"Tokri: makeAlwaysOnTop applied to %@", nswin);

    [nswin setLevel:NSStatusWindowLevel];
    [nswin setHidesOnDeactivate:NO];
}
