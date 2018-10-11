/***************************************************************************
// Broadband Integrated Switching Platform(BISP) Basic Component
// Copyright(c) 2000-2008 Network Communications Company, 
//						  Haige Communications Group Incorporated Company.
// All rights reserved.
//
// This file is a part of the Broadband Integrated Switching Platform(BISP).
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
**************************************************************************/
/*模块连接定义，专门用来连接*.a文件中的模块到目标代码当中*/
#ifndef __BISPMODLINK_H_INCLUDED
#define __BISPMODLINK_H_INCLUDED

/* 模块连接声明 */

#define BISP_DECLARE_MODULE_LINK(Name) \
	int g_##Name##Link = 0;


#ifdef __cplusplus

/* 模块使用声明 */
#define BISP_USE_MODULE(Name) \
  extern "C" int g_##Name##Link; \
  void Bisp##Name##__FILE__##Link(){ g_##Name##Link = 0; }


#else

/* 模块使用声明 */
#define BISP_USE_MODULE(Name) \
  extern int g_##Name##Link; \
  void Bisp##Name##__FILE__##Link(){ g_##Name##Link = 0; }


#endif




#endif /* __BISPMODLINK_H_INCLUDED */


