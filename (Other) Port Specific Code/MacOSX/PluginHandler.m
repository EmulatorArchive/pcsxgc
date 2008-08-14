//
//  PluginHandler.m
//  Pcsx
//
//  Created by Gil Pedersen on Sun Sep 21 2003.
//  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
//

#import "PluginHandler.h"

@implementation PluginHandler

+ (void)loadPlugins
{
    graphicsPlugins = [[PluginList alloc] initWithType: PS_LT_GPU];
}

@end
