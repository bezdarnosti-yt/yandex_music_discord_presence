#pragma once

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <regex>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

std::string urlEncode(const std::string& str);

std::string getAlbumCoverUrl(std::string artist, std::string title);