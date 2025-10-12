#pragma once

#include <curl/curl.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// دالة رد النداء للكتابة
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

// دالة رد النداء للتقدم
int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
