/*
* ΔLaunch
* Copyright (c) 2018-2019, Reisyukaku
* 
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
* 
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once
#include <string>
#include <functional>
#include <vector>
#include "UIElement.hpp"
#include "../../Core/types.h"

class Option: public UIElement
{
    public:
        Option(std::string optName, std::vector<std::string> opts, u32 defaultVal, std::function<Result()> callback) : UIElement() {
            Callback = callback;
            Opts = opts;
            optIndex = defaultVal;
            Name = optName;
            Properties = ELEM_Option;
        };
        
        Option(std::string optName, std::string opt, u32 defaultVal, std::function<Result()> callback) : UIElement() {
            Callback = callback;
            Opts.push_back(opt);
            optIndex = defaultVal;
            Name = optName;
            Properties = ELEM_Option;
        };
        
        ~Option() {
            Opts.clear();
        };
        
        void Draw(ElementType type, Rect pos) override {
            Graphics::DrawOption(pos, Name + ": ", !Opts.empty() ? Opts.at(optIndex) : "", type);
        }
        
        Result Run() override {
            Result rc = 0;
            if(Callback != nullptr) {
                rc = Callback();
            }
            return rc;
        }
        
        u32 GetOptIndex() { 
            return optIndex;
        }
        
        void IncrementOption(u32 cap = 0) override {
            optIndex = ((!Opts.empty() && optIndex >= (cap == 0 ? Opts.size() : cap) - 1) ? 0 : optIndex + 1);
        }
        
        void SetOption(std::string text, u32 ind = 0) override {
            Opts[ind] = text;
        }
        
        void SetOptions(std::vector<std::string> opts) override {
            Opts = opts;
        }
        
        void SetNameText(std::string name) override {
            Name = name;
        }
        
    private:
        u32 optIndex;
        std::vector<std::string> Opts;
};