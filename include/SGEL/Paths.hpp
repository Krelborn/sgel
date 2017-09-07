//
//  Paths.hpp
//
//  SGEL - Simple Game Engine Library for SFML
//

#pragma once

#include <string>

namespace sgel
{
    namespace Paths
    {
        /**
         *  Return the path to the resource folder.
         *
         *  @return The path to the resource folder associate with the main bundle or an empty string is there is no bundle.
         */
        std::string GetResourcePath();
    }
}
