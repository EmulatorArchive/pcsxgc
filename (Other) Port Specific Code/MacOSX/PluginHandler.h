//
//  PluginHandler.h
//  Pcsx
//
//  Created by Gil Pedersen on Sun Sep 21 2003.
//  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface PluginHandler : NSObject {

    @private
    PluginList graphicsPlugins;
}

+ (void)loadPlugins

@end
