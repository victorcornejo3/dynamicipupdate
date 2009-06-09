// Copyright (c) 2009 OpenDNS, LLC. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTTP_H__
#define HTTP_H__

#include "MemSegment.h"

class HttpResult {
public:
	/* 0 if no error */
	DWORD		  	error;
	MemSegment		data;

	HttpResult() {
		error = 0;
	}

	~HttpResult() {
		data.freeAll();
	}

	bool IsValid() {
		return 0 == error;
	}
};

HttpResult* HttpGet(const WCHAR *url);
HttpResult* HttpGet(const char *url);

HttpResult* HttpGet(const WCHAR *host, const WCHAR *url, bool https);
HttpResult* HttpGet(const char *host, const char *url, bool https);

HttpResult* HttpGetWithBasicAuth(const WCHAR *host, const WCHAR *url, const WCHAR *userName, const WCHAR *pwd, bool https);
HttpResult* HttpGetWithBasicAuth(const char *host, const char *url, const char *userName, const char *pwd, bool https);

HttpResult* HttpPost(const char *host, const char *url, const char *params, bool https);
HttpResult* HttpPost(const WCHAR *host, const WCHAR *url, const char *params, bool https);
HttpResult* HttpPostData(const char *host, const char *url, void *data, DWORD dataSize, bool https);
HttpResult* HttpPostData(const WCHAR *host, const WCHAR *url, void *data, DWORD dataSize, bool https);
bool HttpPostAsync(const char *host, const char *url, const char *params, bool https, HWND hwndToNotify, UINT msg);

#endif
