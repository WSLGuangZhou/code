///////////////////////////////////////////////////////////////////////////////////////////
// Broadband Integrated Switching Platform(BISP) Basic Component
// Copyright(c) 2000-2008 Network Communications Company, 
//						  Haige Communications Group Incorporated Company.
// All rights reserved.
//
// This file is a part of the Broadband Integrated Switching Platform(BISP).
// The code and information is provided "as-is" without
// warranty of any kind, either expressed or implied.
///////////////////////////////////////////////////////////////////////////////////////////
//	文 件 名 : FastMalloc.h
//	文件功能 : 针对小块内存的频繁分配和释放的优化内存分配算法
//	作    者 : 谭 北 华
//	创建时间 : 2005年5月27日
//	项目名称 : 
//	备    注 :
//	历史记录 : 2007.11.30 增加驱动内存分配函数
///////////////////////////////////////////////////////////////

#ifndef __FASEMALLOC_H_INCLUDED
#define __FASEMALLOC_H_INCLUDED

//默认的驱动Buffer大小
#define DEFAULT_DRVMEM_ALLOC_SIZE 1600L

//采用最优内存大小分配方法，可以更加有效的增加内存利用率
#define FASTMALLOC_OPTIMIZ_SIZE

//默认一次分配16K
#define DEFAULT_SIZEOF_ALLOCBLOCK (1024*16)

#define CALC_BLOCK_SIZE(n) (  ((n)>512)? (n):( (n)>256? 512:( (n)>128? 256:( (n)>64? 128:64 ) ) )  )

//FastMalloc 句柄
typedef struct __tagFastMalloc{void* pPrivate;}* HFASTMALLOC;

//FixedMalloc 句柄
typedef struct __tagFixedMalloc{void* pPrivate;}* HFIXEDMALLOC;

//DrvMalloc 句柄
typedef struct __tagDrvMalloc{void* pPrivate;}* HDRVMALLOC;

#ifdef __cplusplus
extern "C"{
#endif //__cplusplus

//创建快速内存分配结构
HFASTMALLOC FastMallocCreate(int nSizeOfAllocBlock);

//销毁快速内存分配结构
BOOL FastMallocDestroy(HFASTMALLOC hFastMalloc);

//分配内存
void* FastMalloc(HFASTMALLOC hFastMalloc,size_t size);

//释放内存
void FastFree(HFASTMALLOC hFastMalloc,void* pData);

//内存使用情况统计打印
int FastMallocPrint(HFASTMALLOC hFastMalloc,char* pBuffer,int nBufferSize);
/////////////////////////////////////////////////////////////////
//固定内存块分配，比FastMalloc更快

//创建固定内存分配结构
HFIXEDMALLOC FixedMallocCreate(int nFixedSize,int nSizeOfAllocBlock);

//销毁固定内存分配结构
BOOL FixedMallocDestroy(HFIXEDMALLOC hFixedMalloc);

//分配固定内存
void* FixedMalloc(HFIXEDMALLOC hFixedMalloc);

//释放固定内存
void FixedFree(HFIXEDMALLOC hFixedMalloc,void* pData);

//内存使用情况统计打印
int FixedMallocPrint(HFIXEDMALLOC hFixedMalloc,char* pBuffer,int nBufferSize);

///////////////////////////////////////////////////////////////
//驱动层内存分配，适用于中断缓冲

//创建驱动内存分配结构
HDRVMALLOC	DrvMemMallocCreate(int nAllocSize,void* pDrvMem,int nMemSize);

//销毁驱动内存分配结构
BOOL DrvMemMallocDestroy(HDRVMALLOC hDrvMalloc);

//分配驱动内存
void* DrvMemMalloc(HDRVMALLOC hDrvMalloc);

//释放驱动内存
void DrvMemFree(HDRVMALLOC hDrvMalloc,void* pData);

//内存使用情况统计打印
int DrvMemMallocPrint(HDRVMALLOC hDrvMalloc,char* pBuffer,int nBufferSize);


#ifdef __cplusplus
};
#endif //__cplusplus

//C++定义一个快速内存分配类
#ifdef __cplusplus

//快速内存分配类
class CFastMalloc
{
public:
	CFastMalloc(int nSizeOfAllocBlock = DEFAULT_SIZEOF_ALLOCBLOCK)
	{
		m_hMalloc = FastMallocCreate(nSizeOfAllocBlock);
	}
	~CFastMalloc()
	{
		FastMallocDestroy(m_hMalloc);
		m_hMalloc = NULL;
	}
	//分配函数
	void* _Malloc(size_t size)
	{
		return FastMalloc(m_hMalloc,size);
	}
	//释放函数
	void _Free(void* pData)
	{
		FastFree(m_hMalloc,pData);
	}
private:
	HFASTMALLOC m_hMalloc;
};

/*lint -save -e1712 default constructor */

//固定内存分配类
class CFixedMalloc
{
public:
	CFixedMalloc(int nFixedSize,int nSizeOfAllocBlock = DEFAULT_SIZEOF_ALLOCBLOCK)
	{
		m_hMalloc = FixedMallocCreate(nFixedSize,nSizeOfAllocBlock);
	}
	~CFixedMalloc()
	{
		FixedMallocDestroy(m_hMalloc);
		m_hMalloc = NULL;
	}
	//分配函数
	void* _Malloc()
	{
		return FixedMalloc(m_hMalloc);
	}
	//释放函数
	void _Free(void* pData)
	{
		FixedFree(m_hMalloc,pData);
	}
private:
	HFIXEDMALLOC m_hMalloc;	
};
/*lint -restore */

// DECLARE_FAST_MALLOC -- used in class definition
#define DECLARE_FAST_MALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		return s_malloc._Malloc(size); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { s_malloc._Free(p); } \
protected: \
	static CFastMalloc s_malloc;

// IMPLEMENT_FAST_MALLOC -- used in class implementation file
#define IMPLEMENT_FAST_MALLOC(class_name) \
CFastMalloc class_name::s_malloc(DEFAULT_SIZEOF_ALLOCBLOCK);


// DECLARE_FIXED_MALLOC -- used in class definition
#define DECLARE_FIXED_MALLOC(class_name) \
public: \
	void* operator new(size_t size) \
	{ \
		return s_malloc._Malloc(); \
	} \
	void* operator new(size_t, void* p) \
		{ return p; } \
	void operator delete(void* p) { s_malloc._Free(p); } \
protected: \
	static CFixedMalloc s_malloc;


// IMPLEMENT_FIXED_MALLOC -- used in class implementation file
#define IMPLEMENT_FIXED_MALLOC(class_name) \
CFixedMalloc class_name::s_malloc(sizeof(class_name),DEFAULT_SIZEOF_ALLOCBLOCK);


#endif //__cplusplus



#endif // __FASEMALLOC_H_INCLUDED
