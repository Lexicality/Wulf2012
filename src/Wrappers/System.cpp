//
// WulfGame/Wrappers/System.cpp
// Copyright (C) 2012 Lexi Robinson
// This code is freely available under the MIT licence.
//
#include "System.h"

#if defined _WIN32
#include <windows.h>
#elif defined __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#else
#include <iostream>
#endif

void System::ErrorBox(const std::string& title, const std::string& text) {
#if defined _WIN32
	// I dunno about this, but I'm not a win32 programmer.
	const char *ttl = title.c_str();
	const char *txt = text.c_str();
	LPCTSTR lttl = (LPCTSTR) ttl;
	LPCTSTR ltxt = (LPCTSTR) txt;
	MessageBox(NULL, ltxt, lttl, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
#elif defined __APPLE__
    // http://macosx-programming.blogspot.co.uk/2011/09/message-box-using-corefoundation.html
    SInt32 nRes = 0;
    CFUserNotificationRef pDlg = NULL;
    const void* keys[] = { kCFUserNotificationAlertHeaderKey,
        kCFUserNotificationAlertMessageKey };
    const void* vals[] = {
        CFStringCreateWithCString(NULL, title.c_str(), kCFStringEncodingASCII),
        CFStringCreateWithCString(NULL, text.c_str(), kCFStringEncodingASCII)
    };
    CFDictionaryRef dict = CFDictionaryCreate(0, keys, vals,
                                              sizeof(keys)/sizeof(*keys), 
                                              &kCFTypeDictionaryKeyCallBacks, 
                                              &kCFTypeDictionaryValueCallBacks);
    pDlg = CFUserNotificationCreate(kCFAllocatorDefault, 0, 
                                    kCFUserNotificationPlainAlertLevel, 
                                    &nRes, dict);
#else
	std::cerr << "ERROR: " << title << ": " << text << std::endl;
#endif
}

