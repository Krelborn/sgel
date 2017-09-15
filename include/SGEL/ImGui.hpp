//
//  ImGui.hpp
//
//  Copyright © 2017 Compiled Creations Ltd. All rights reserved.
//

#pragma once

#include "imgui.h"
#include "imgui-SFML.h"

namespace SGEL
{
    namespace ImGui
    {
        static auto vector_getter = [](void* vec, int idx, const char** out_text)
        {
            auto& vector = *static_cast<std::vector<std::string>*>(vec);
            if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
            *out_text = vector.at(idx).c_str();
            return true;
        };

        bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
        {
            if (values.empty()) { return false; }
            return ::ImGui::Combo(label, currIndex, vector_getter,
                         static_cast<void*>(&values), values.size());
        }

        bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values)
        {
            if (values.empty()) { return false; }
            return ::ImGui::ListBox(label, currIndex, vector_getter,
                           static_cast<void*>(&values), values.size());
        }
    }
}
