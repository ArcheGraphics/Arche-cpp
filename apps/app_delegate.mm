//  Copyright (c) 2023 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#import "app_delegate.h"

@interface AAPLAppDelegate ()
@end

@implementation AAPLAppDelegate {
}

/// Callback when the app launches.
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
}

/// Callback when the app needs to close.
- (void)applicationWillTerminate:(NSNotification *)aNotification {
}

/// Says that the application closes if all the windows are closed.
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    return YES;
}

@end
