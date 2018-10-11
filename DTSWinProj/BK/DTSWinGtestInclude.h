#ifndef DTS_WIN_GTEST_H_INCLUDED
#define DTS_WIN_GTEST_H_INCLUDED

//WIN32
#ifdef WIN32

#ifndef DTS_TEST
	#define DTS_TEST
#endif

// 	#include <WinSock2.h>
// 	#include <WS2TCPIP.H>
// 	#include <windows.h>
// 	#include <stdio.h>
// 	#include <stdlib.h>
// 	#include <time.h>
// 	#include <assert.h>
	
	#include "gtest/gtest.h"
	#include "DTSTestUnit.h"
	#pragma comment(lib,"../lib/gtest/gtestd.lib")

//	#pragma comment(lib,"WS2_32.lib")
	#pragma warning(disable:4100)//disable warning "unreferenced formal parameter"
	#pragma comment(lib,"../lib/gtest/gtestd.lib")
//	#pragma comment(lib,"../lib/DTSCommon.lib")

#endif //WIN32

#endif //DTS_WIN_GTEST_H_INCLUDED
