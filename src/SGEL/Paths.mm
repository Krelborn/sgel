//
//  Paths.hpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#import "Paths.hpp"

#import <Foundation/Foundation.h>

using namespace sgel;

/**
 *  Return the path to the resource folder.
 *
 *  @return The path to the resource folder associate with the main bundle or an empty string is there is no bundle.
 */
std::string Paths::GetResourcePath()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    std::string rpath;
    NSBundle* bundle = [NSBundle mainBundle];

    if (bundle == nil) {
#ifdef DEBUG
        NSLog(@"bundle is nil... thus no resources path can be found.");
#endif
    } else {
        NSString* path = [bundle resourcePath];
        rpath = [path UTF8String] + std::string("/");
    }

    [pool drain];

    return rpath;
}
