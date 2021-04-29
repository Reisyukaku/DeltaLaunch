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

#include <curl/curl.h>
#include <string>
#include "../UI/Forms.hpp"
#include "../Core/types.h"

enum RequestMethod {
    HTTP_GET,
    HTTP_POST
};

class Network
{
    public:
        static Network* getInstance() {
            if (instance == nullptr)
                instance = new Network();
            return instance;
        };
        
        std::string Request(std::string url, RequestMethod method, std::string postdata = "");
        void Download(std::string url, std::string file);
        u32 GetWifiStrength();
        
    private:
        static Network* instance;
        static struct curl_slist *hosts;
        UI::MessageBox *msgBox;
        Network();
        
        Network(const Network&);
        Network& operator=(const Network&);
        
};