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
// Fast 快速链表定义
#ifndef __FASTLIST_H_INCLUDED
#define __FASTLIST_H_INCLUDED


//Member function 'Symbol' could be made const
/*lint -save -e1762 */

//ASSERT的定义
#ifdef BISPASSERT
#define FASTASSERT(f) BISPASSERT(f)
#else 
#define FASTASSERT(f) ((void)0)
#endif //BISPASSERT


//内联及寄存器定义
#ifdef __cplusplus
#define FAST_INLINE inline
#else
#define FAST_INLINE static inline
#endif
#define FAST_REGISTER register

//NOTE: destroy a list same as init a empty list
#define FastList_Destroy(pList) FastList_Init(pList)


//单个节点
typedef struct tagFastListNode
{
    struct tagFastListNode*	m_pNextNode;		//下一个节点
    struct tagFastListNode*	m_pPrevNode;		//前一个节点
}FASTLISTNODE,*LPFASTLISTNODE;

//单个链表
typedef struct tagFastList	
{
	int				m_nListCount;			//链表个数
	LPFASTLISTNODE	m_pHeadNode;			//头节点
	LPFASTLISTNODE	m_pTailNode;			//尾节点
}FASTLIST,*LPFASTLIST;

//初始化节点
FAST_INLINE
void FastList_InitNode(FAST_REGISTER LPFASTLISTNODE pNode)
{
	FASTASSERT(NULL != pNode);
//	if(NULL == pNode)
//	{
//		return;
//	}
	pNode->m_pNextNode = pNode->m_pPrevNode = NULL;
}

//初始化链表
FAST_INLINE
void FastList_Init(FAST_REGISTER LPFASTLIST pList)
{
	FASTASSERT(NULL != pList);

	pList->m_nListCount = 0;
	pList->m_pHeadNode = pList->m_pTailNode = NULL;
}

//取得链表大小
FAST_INLINE
int FastList_Size(FAST_REGISTER LPFASTLIST pList)
{
	FASTASSERT(NULL != pList);
	return pList->m_nListCount;
}

//从链表尾部插入一个节点
FAST_INLINE
void FastList_PushBack(FAST_REGISTER LPFASTLIST pList,
								LPFASTLISTNODE pNode)
{
	FASTASSERT(NULL != pList);
	FASTASSERT(NULL != pNode);
//	if(NULL == pNode)
//	{
//		return;
//	}
	pNode->m_pNextNode = NULL;
	pNode->m_pPrevNode = pList->m_pTailNode;
	if(NULL == pList->m_pTailNode)	//First Insert
	{
		pList->m_pHeadNode = pNode;
	}
	else	//list not empty
	{
		pList->m_pTailNode->m_pNextNode = pNode;
	}
	pList->m_pTailNode = pNode;
	pList->m_nListCount ++;
}

//从链表头部插入一个节点
FAST_INLINE
void FastList_PushFront(FAST_REGISTER LPFASTLIST pList,
						LPFASTLISTNODE pNode)
{
	FASTASSERT(NULL != pList);
	FASTASSERT(NULL != pNode);
//	if(NULL == pNode)
//	{
//		return;
//	}
	
	pNode->m_pPrevNode = NULL;
	pNode->m_pNextNode = pList->m_pHeadNode;
	if(NULL == pList->m_pHeadNode)	//First Insert
	{
		pList->m_pTailNode = pNode;
	}
	else
	{
		pList->m_pHeadNode->m_pPrevNode = pNode;
	}
	pList->m_pHeadNode = pNode;
	pList->m_nListCount ++;
}

//从链表尾部弹出一个节点
FAST_INLINE
LPFASTLISTNODE FastList_PopBack(FAST_REGISTER LPFASTLIST pList)
{
	FASTASSERT(NULL != pList);

	//得到弹出的Node
	LPFASTLISTNODE pNode = pList->m_pTailNode;
	if(NULL != pNode)	//弹出不为空
	{
		pList->m_pTailNode = pNode->m_pPrevNode;

		if(NULL != pList->m_pTailNode)
		{
			pList->m_pTailNode->m_pNextNode = NULL;
		}
		else
		{
			pList->m_pHeadNode = NULL;
		}
		pList->m_nListCount --;
	}
	return pNode;
}

//从链表头弹出一个节点
FAST_INLINE
LPFASTLISTNODE FastList_PopFront(FAST_REGISTER LPFASTLIST pList)
{
	FASTASSERT(NULL != pList);

	//得到弹出的Node
	LPFASTLISTNODE pNode = pList->m_pHeadNode;
	if(NULL != pNode)	//弹出不为空
	{
		pList->m_pHeadNode = pNode->m_pNextNode;

		if(NULL != pList->m_pHeadNode)
		{
			pList->m_pHeadNode->m_pPrevNode = NULL;
		}
		else
		{
			pList->m_pTailNode = NULL;
		}
		pList->m_nListCount --;
	}
	return pNode;
}

//删除一个链表
FAST_INLINE
void FastList_DeleteNode(FAST_REGISTER LPFASTLIST pList,
						 LPFASTLISTNODE pNode)
{
	FASTASSERT(NULL != pList);
	FASTASSERT(NULL != pNode);
//	if(NULL == pNode)
//	{
//		return;
//	}
	if(NULL != pNode->m_pNextNode)
	{
		pNode->m_pNextNode->m_pPrevNode = pNode->m_pPrevNode;
	}
	if(NULL != pNode->m_pPrevNode)
	{
		pNode->m_pPrevNode->m_pNextNode = pNode->m_pNextNode;
	}
	if(pList->m_pTailNode == pNode)	//delete tail?
	{
		pList->m_pTailNode = pNode->m_pPrevNode;
	}
	if(pList->m_pHeadNode == pNode) //delete head?
	{
		pList->m_pHeadNode = pNode->m_pNextNode;
	}
	pList->m_nListCount --;
}

//在一个节点前插入另外一个节点
FAST_INLINE
void FastList_InsertNodeBefore(FAST_REGISTER LPFASTLIST pList,
							   LPFASTLISTNODE pNode,
							   LPFASTLISTNODE pNodeBefore)
{
	FASTASSERT(NULL != pList);
	FASTASSERT(NULL != pNode);
//	if(NULL == pNode)
//	{
//		return;
//	}
	if(NULL == pNodeBefore)
	{
		FastList_PushFront(pList,pNode);
		return ;
	}
	if(NULL != pNodeBefore->m_pPrevNode)
	{
		pNodeBefore->m_pPrevNode->m_pNextNode = pNode;
	}
	pNode->m_pPrevNode = pNodeBefore->m_pPrevNode;
	pNodeBefore->m_pPrevNode = pNode;
	pNode->m_pNextNode = pNodeBefore;
	//是否插入到链表头?
	if(pList->m_pHeadNode == pNodeBefore)
	{
		pList->m_pHeadNode = pNode;
	}
	pList->m_nListCount ++;
}

//在一个节点后插入另外一个节点
FAST_INLINE
void FastList_InsertNodeAfter(FAST_REGISTER LPFASTLIST pList,
							   LPFASTLISTNODE pNode,
							   LPFASTLISTNODE pNodeAfter)
{
	FASTASSERT(NULL != pList);
	FASTASSERT(NULL != pNode);
//	if(NULL == pNode)
//	{
//		return;
//	}
	if(NULL == pNodeAfter)
	{
		FastList_PushBack(pList,pNode);
		return ;
	}
	if(NULL != pNodeAfter->m_pNextNode)
	{
		pNodeAfter->m_pNextNode->m_pPrevNode = pNode;
	}
	pNode->m_pNextNode = pNodeAfter->m_pNextNode;
	pNodeAfter->m_pNextNode = pNode;
	pNode->m_pPrevNode = pNodeAfter;
	//是否插入到链表尾?
	if(pList->m_pTailNode == pNodeAfter)
	{
		pList->m_pTailNode = pNode;
	}
	pList->m_nListCount ++;
}

//取得第N个节点指针
FAST_INLINE
LPFASTLISTNODE FastList_GetNth(FAST_REGISTER LPFASTLIST pList,int nNodeNum)
{
	FASTASSERT(NULL != pList);
	LPFASTLISTNODE pNode;
	if(nNodeNum >= 0)
	{
		pNode = pList->m_pHeadNode;
		while( nNodeNum-- > 0 && NULL != pNode)
		{
			pNode = pNode->m_pNextNode;
		}
	}
	else
	{
		pNode = pList->m_pTailNode;

		while( nNodeNum++ < -1 && NULL != pNode)
		{
			pNode = pNode->m_pPrevNode;
		}	
	}
	return pNode;
}

//交换两个相邻的节点
FAST_INLINE
void FastList_SwapNode(FAST_REGISTER LPFASTLIST pList,LPFASTLISTNODE pNode,LPFASTLISTNODE pNextNode)
{
	FASTASSERT(NULL != pList && NULL != pNode && NULL != pNextNode);

	LPFASTLISTNODE pPrevNodeV;
	LPFASTLISTNODE pPrevNode;
	LPFASTLISTNODE pNextNodeY;

	//交换节点
	pPrevNodeV = pNode->m_pPrevNode;
	pPrevNode = pNode;
	pNextNodeY = pNextNode->m_pNextNode;

	//之前：P=pNodePointer
	//       V         P          N        Y
	//       *=>p   v<=*=>n    p<=*=>y  n<=*
	//之后：
	//       V         N          P        Y    
	//       *=>n   v<=*=>p    n<=*=>y  p<=*
	
	//V(*=>p) = V(*=>n)
	if(NULL != pPrevNodeV)
	{
		pPrevNodeV->m_pNextNode = pNextNode;
	}
	//N(p<=*) = N(v<=*)
	pNextNode->m_pPrevNode = pPrevNodeV;
	//N(*=>y) = N(*=>p)
	pNextNode->m_pNextNode = pPrevNode;
	//P(v<=*) = P(n<=*)
	pPrevNode->m_pPrevNode = pNextNode;
	//P(*=>n) = P(*=>y)
	pPrevNode->m_pNextNode = pNextNodeY;
	//Y(n<=*) = Y(p<=*)
	if(NULL != pNextNodeY)
	{
		pNextNodeY->m_pPrevNode = pPrevNode;
	}
	//是否移动了头指针?
	if(pPrevNode == pList->m_pHeadNode)
	{
		pList->m_pHeadNode = pNextNode;
	}
	//是否移动了尾指针?
	if(pNextNode == pList->m_pTailNode)
	{
		pList->m_pTailNode = pPrevNode;
	}				
}

#ifdef __cplusplus	//in C++ uses template class

//FAST链表节点
template<class T>
class CFastNodeT
{
public:
	FAST_INLINE CFastNodeT():m_pNextNode(NULL),m_pPrevNode(NULL)
	{}
	FAST_INLINE void InitNode()
	{
		m_pNextNode = m_pPrevNode = NULL;
	}
	T*				m_pNextNode;		//下一个节点
	T*				m_pPrevNode;		//前一个节点
};

//FAST单个链表:
template<class TNODE>
class CFastListT
{
public:
	FAST_INLINE CFastListT():m_nListCount(0),m_pHeadNode(NULL),m_pTailNode(NULL)
	{}
	FAST_INLINE int GetCount() const
	{ 
		return m_nListCount;	
	}
	FAST_INLINE TNODE* GetHead()
	{
		return m_pHeadNode;
	}
	FAST_INLINE TNODE* GetTail()
	{
		return m_pTailNode;
	}
	FAST_INLINE void PushBack(TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		pNode->m_pNextNode = NULL;
		pNode->m_pPrevNode = m_pTailNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_pTailNode)
		{
			m_pHeadNode = pNode;
		}
		else	//list not empty
		{
			m_pTailNode->m_pNextNode = pNode;
		}
		m_pTailNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE void PushFront(TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		pNode->m_pPrevNode = NULL;
		pNode->m_pNextNode = m_pHeadNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_pHeadNode)
		{
			m_pTailNode = pNode;
		}
		else
		{
			m_pHeadNode->m_pPrevNode = pNode;
		}
		m_pHeadNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE TNODE* PopBack()
	{
		//得到弹出的Node
		TNODE* pNode = m_pTailNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_pTailNode = (TNODE*)pNode->m_pPrevNode;

			if(NULL != m_pTailNode)
			{
				m_pTailNode->m_pNextNode = NULL;
			}
			else
			{
				m_pHeadNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE TNODE* PopFront()
	{
		//得到弹出的Node
		TNODE* pNode = m_pHeadNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_pHeadNode = (TNODE*)pNode->m_pNextNode;

			if(NULL != m_pHeadNode)
			{
				m_pHeadNode->m_pPrevNode = NULL;
			}
			else
			{
				m_pTailNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE void DeleteNode(TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		if(NULL != pNode->m_pNextNode)
		{
			pNode->m_pNextNode->m_pPrevNode = pNode->m_pPrevNode;
		}
		if(NULL != pNode->m_pPrevNode)
		{
			pNode->m_pPrevNode->m_pNextNode = pNode->m_pNextNode;
		}
		if(m_pTailNode == pNode)	//delete tail?
		{
			m_pTailNode = (TNODE*)pNode->m_pPrevNode;
		}
		if(m_pHeadNode == pNode) //delete head?
		{
			m_pHeadNode = (TNODE*)pNode->m_pNextNode;
		}
		m_nListCount --;
	}
	FAST_INLINE void InsertNodeBefore(TNODE* pNode,TNODE* pNodeBefore)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		if(NULL == pNodeBefore)
		{
			PushFront(pNode);
			return ;
		}
		if(NULL != pNodeBefore->m_pPrevNode)
		{
			pNodeBefore->m_pPrevNode->m_pNextNode = pNode;
		}
		pNode->m_pPrevNode = pNodeBefore->m_pPrevNode;
		pNodeBefore->m_pPrevNode = pNode;
		pNode->m_pNextNode = pNodeBefore;
		//是否插入到链表头?
		if(m_pHeadNode == pNodeBefore)
		{
			m_pHeadNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE void InsertNodeAfter(TNODE* pNode,TNODE* pNodeAfter)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		if(NULL == pNodeAfter)
		{
			PushBack(pNode);
			return ;
		}
		if(NULL != pNodeAfter->m_pNextNode)
		{
			pNodeAfter->m_pNextNode->m_pPrevNode = pNode;
		}
		pNode->m_pNextNode = pNodeAfter->m_pNextNode;
		pNodeAfter->m_pNextNode = pNode;
		pNode->m_pPrevNode = pNodeAfter;
		//是否插入到链表尾?
		if(m_pTailNode == pNodeAfter)
		{
			m_pTailNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE TNODE* GetNth(int nNodeNum) const
	{
		TNODE* pNode;
		if(nNodeNum >= 0)
		{
			pNode = m_pHeadNode;
			while (nNodeNum-- > 0 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pNextNode;
			}
		}
		else
		{
		    pNode = m_pTailNode;
			while (nNodeNum++ < -1 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pPrevNode;
			}		
		}
		return pNode;
	}
	FAST_INLINE BOOL IsEmpty() const
	{
		return (0 == m_nListCount);
	}
	FAST_INLINE void Clear()
	{
		m_nListCount = 0;
		m_pHeadNode = m_pTailNode = NULL;
	}
	//交换两个相邻的节点
	FAST_INLINE void SwapNode(TNODE* pNode,TNODE* pNextNode)
	{
		FASTASSERT(NULL != pNode && NULL != pNextNode);

		TNODE* pPrevNodeV;
		TNODE* pPrevNode;
		TNODE* pNextNodeY;

		//交换节点
		pPrevNodeV = pNode->m_pPrevNode;
		pPrevNode = pNode;
		pNextNodeY = pNextNode->m_pNextNode;

		//之前：P=pNodePointer
		//       V         P          N        Y
		//       *=>p   v<=*=>n    p<=*=>y  n<=*
		//之后：
		//       V         N          P        Y    
		//       *=>n   v<=*=>p    n<=*=>y  p<=*
		
		//V(*=>p) = V(*=>n)
		if(NULL != pPrevNodeV)
		{
			pPrevNodeV->m_pNextNode = pNextNode;
		}
		//N(p<=*) = N(v<=*)
		pNextNode->m_pPrevNode = pPrevNodeV;
		//N(*=>y) = N(*=>p)
		pNextNode->m_pNextNode = pPrevNode;
		//P(v<=*) = P(n<=*)
		pPrevNode->m_pPrevNode = pNextNode;
		//P(*=>n) = P(*=>y)
		pPrevNode->m_pNextNode = pNextNodeY;
		//Y(n<=*) = Y(p<=*)
		if(NULL != pNextNodeY)
		{
			pNextNodeY->m_pPrevNode = pPrevNode;
		}
		//是否移动了头指针?
		if(pPrevNode == m_pHeadNode)
		{
			m_pHeadNode = pNextNode;
		}
		//是否移动了尾指针?
		if(pNextNode == m_pTailNode)
		{
			m_pTailNode = pPrevNode;
		}				
	}
private:
//public:
	int				m_nListCount;			//链表个数
	TNODE*			m_pHeadNode;			//头节点
	TNODE*			m_pTailNode;			//尾节点
};

//链表冒泡排序算法
template<class TNODE,typename TParam>
void FastListSortT(CFastListT<TNODE>& _list,
				int (*FuncNodeCompare)(TNODE* ,TNODE*,TParam),
				TParam nParam)
{
	//以下是对链表的冒泡排序算法：
	TNODE* pNodePointer;
	//取得最后一个节点
	TNODE* pTailNode = _list.GetTail();

	//冒泡排序
	while(_list.GetHead() != pTailNode)
	{
		pNodePointer = _list.GetHead();
		while(pNodePointer != pTailNode)
		{
			if(NULL == pNodePointer->m_pNextNode)
			{
				break;
			}

			//需要交换链表?
			if( (*FuncNodeCompare)(pNodePointer,pNodePointer->m_pNextNode,nParam) > 0)	
			{
				if(pTailNode == pNodePointer->m_pNextNode)
				{
					pTailNode = pNodePointer;
				}
				//交换前后两个节点
				_list.SwapNode(pNodePointer,pNodePointer->m_pNextNode);
				
			}
			else	//不需要交换链表
			{
				pNodePointer = pNodePointer->m_pNextNode;
			}

		} //while(pNodePointer != pTailNode)

		//前进一个位移
		pTailNode = pNodePointer->m_pPrevNode;
	}
}

//链表冒泡排序算法(支持两个参数)
template<class TNODE,typename TParam1,typename TParam2>
void FastListSort2T(CFastListT<TNODE>& _list,
				int (*FuncNodeCompare)(TNODE* ,TNODE*,TParam1,TParam2),
				TParam1 nParam1,TParam1 nParam2)
{
	//以下是对链表的冒泡排序算法：
	TNODE* pNodePointer;
	//取得最后一个节点
	TNODE* pTailNode = _list.GetTail();

	//冒泡排序
	while(_list.GetHead() != pTailNode)
	{
		pNodePointer = _list.GetHead();
		while(pNodePointer != pTailNode)
		{
			if(NULL == pNodePointer->m_pNextNode)
			{
				break;
			}

			//需要交换链表?
			if( (*FuncNodeCompare)(pNodePointer,pNodePointer->m_pNextNode,nParam1,nParam2) > 0)	
			{
				if(pTailNode == pNodePointer->m_pNextNode)
				{
					pTailNode = pNodePointer;
				}
				//交换前后两个节点
				_list.SwapNode(pNodePointer,pNodePointer->m_pNextNode);
				
			}
			else	//不需要交换链表
			{
				pNodePointer = pNodePointer->m_pNextNode;
			}

		} //while(pNodePointer != pTailNode)

		//前进一个位移
		pTailNode = pNodePointer->m_pPrevNode;
	}
}

//FAST多重链表节点
template<class T,int nMultiCount>
class CFastMultiNodeT
{
public:
	FAST_INLINE CFastMultiNodeT()
	{
		InitNode();
	}
	FAST_INLINE void InitNode()
	{
		for(int i=0;i<nMultiCount;i++)
		{
			m_pNextNode[i] = m_pPrevNode[i] = NULL;
		}
	}
	T*				m_pNextNode[nMultiCount];		//下一个节点
	T*				m_pPrevNode[nMultiCount];		//前一个节点
};

//FAST单个链表:
template<class TNODE,int nIndex>
class CFastMultiListT
{
public:
	FAST_INLINE CFastMultiListT():m_nListCount(0),m_pHeadNode(NULL),m_pTailNode(NULL)
	{}
	FAST_INLINE int GetCount() const
	{ 
		return m_nListCount;	
	}
	FAST_INLINE TNODE* GetHead()
	{
		return m_pHeadNode;
	}
	FAST_INLINE TNODE* GetTail()
	{
		return m_pTailNode;
	}
	FAST_INLINE void PushBack(TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		pNode->m_pNextNode[nIndex] = NULL;
		pNode->m_pPrevNode[nIndex] = m_pTailNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_pTailNode)
		{
			m_pHeadNode = pNode;
		}
		else	//list not empty
		{
			m_pTailNode->m_pNextNode[nIndex] = pNode;
		}
		m_pTailNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE void PushFront(TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		pNode->m_pPrevNode[nIndex] = NULL;
		pNode->m_pNextNode[nIndex] = m_pHeadNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_pHeadNode)
		{
			m_pTailNode = pNode;
		}
		else
		{
			m_pHeadNode->m_pPrevNode[nIndex] = pNode;
		}
		m_pHeadNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE TNODE* PopBack()
	{	
		//得到弹出的Node
		TNODE* pNode = m_pTailNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_pTailNode = (TNODE*)pNode->m_pPrevNode[nIndex];

			if(NULL != m_pTailNode)
			{
				m_pTailNode->m_pNextNode[nIndex] = NULL;
			}
			else
			{
				m_pHeadNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE TNODE* PopFront()
	{
		//得到弹出的Node
		TNODE* pNode = m_pHeadNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_pHeadNode = (TNODE*)pNode->m_pNextNode[nIndex];

			if(NULL != m_pHeadNode)
			{
				m_pHeadNode->m_pPrevNode[nIndex] = NULL;
			}
			else
			{
				m_pTailNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE void DeleteNode(TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		if(NULL != pNode->m_pNextNode[nIndex])
		{
			pNode->m_pNextNode[nIndex]->m_pPrevNode[nIndex] = pNode->m_pPrevNode[nIndex];
		}
		if(NULL != pNode->m_pPrevNode[nIndex])
		{
			pNode->m_pPrevNode[nIndex]->m_pNextNode[nIndex] = pNode->m_pNextNode[nIndex];
		}
		if(m_pTailNode == pNode)	//delete tail?
		{
			m_pTailNode = (TNODE*)pNode->m_pPrevNode[nIndex];
		}
		if(m_pHeadNode == pNode) //delete head?
		{
			m_pHeadNode = (TNODE*)pNode->m_pNextNode[nIndex];
		}
		m_nListCount --;
	}
	FAST_INLINE void InsertNodeBefore(TNODE* pNode,TNODE* pNodeBefore)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}
		
		if(NULL == pNodeBefore)
		{
			PushFront(pNode);
			return ;
		}
		if(NULL != pNodeBefore->m_pPrevNode[nIndex])
		{
			pNodeBefore->m_pPrevNode[nIndex]->m_pNextNode[nIndex] = pNode;
		}
		pNode->m_pPrevNode[nIndex] = pNodeBefore->m_pPrevNode[nIndex];
		pNodeBefore->m_pPrevNode[nIndex] = pNode;
		pNode->m_pNextNode[nIndex] = pNodeBefore;
		//是否插入到链表头?
		if(m_pHeadNode == pNodeBefore)
		{
			m_pHeadNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE void InsertNodeAfter(TNODE* pNode,TNODE* pNodeAfter)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		if(NULL == pNodeAfter)
		{
			PushBack(pNode);
			return ;
		}
		if(NULL != pNodeAfter->m_pNextNode[nIndex])
		{
			pNodeAfter->m_pNextNode[nIndex]->m_pPrevNode[nIndex] = pNode;
		}
		pNode->m_pNextNode[nIndex] = pNodeAfter->m_pNextNode[nIndex];
		pNodeAfter->m_pNextNode[nIndex] = pNode;
		pNode->m_pPrevNode[nIndex] = pNodeAfter;
		//是否插入到链表尾?
		if(m_pTailNode == pNodeAfter)
		{
			m_pTailNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE TNODE* GetNth(int nNodeNum) const
	{
		TNODE* pNode;
		if(nNodeNum >= 0)
		{
			pNode = m_pHeadNode;
			while (nNodeNum-- > 0 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pNextNode[nIndex];
			}
		}
		else
		{
		    pNode = m_pTailNode;
			while (nNodeNum++ < -1 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pPrevNode[nIndex];
			}		
		}
		return pNode;
	}
	FAST_INLINE BOOL IsEmpty() const
	{
		return (0 == m_nListCount);
	}
	FAST_INLINE void Clear()
	{
		m_nListCount = 0;
		m_pHeadNode = m_pTailNode = NULL;
	}
	//交换两个相邻的节点
	FAST_INLINE void SwapNode(TNODE* pNode,TNODE* pNextNode)
	{
		FASTASSERT(NULL != pNode && NULL != pNextNode);

		TNODE* pPrevNodeV;
		TNODE* pPrevNode;
		TNODE* pNextNodeY;

		//交换节点
		pPrevNodeV = pNode->m_pPrevNode[nIndex];
		pPrevNode = pNode;
		pNextNodeY = pNextNode->m_pNextNode[nIndex];

		//之前：P=pNodePointer
		//       V         P          N        Y
		//       *=>p   v<=*=>n    p<=*=>y  n<=*
		//之后：
		//       V         N          P        Y    
		//       *=>n   v<=*=>p    n<=*=>y  p<=*
		
		//V(*=>p) = V(*=>n)
		if(NULL != pPrevNodeV)
		{
			pPrevNodeV->m_pNextNode[nIndex] = pNextNode;
		}
		//N(p<=*) = N(v<=*)
		pNextNode->m_pPrevNode[nIndex] = pPrevNodeV;
		//N(*=>y) = N(*=>p)
		pNextNode->m_pNextNode[nIndex] = pPrevNode;
		//P(v<=*) = P(n<=*)
		pPrevNode->m_pPrevNode[nIndex] = pNextNode;
		//P(*=>n) = P(*=>y)
		pPrevNode->m_pNextNode[nIndex] = pNextNodeY;
		//Y(n<=*) = Y(p<=*)
		if(NULL != pNextNodeY)
		{
			pNextNodeY->m_pPrevNode[nIndex] = pPrevNode;
		}
		//是否移动了头指针?
		if(pPrevNode == m_pHeadNode)
		{
			m_pHeadNode = pNextNode;
		}
		//是否移动了尾指针?
		if(pNextNode == m_pTailNode)
		{
			m_pTailNode = pPrevNode;
		}				
	}
private:
	int				m_nListCount;			//链表个数
	TNODE*			m_pHeadNode;			//头节点
	TNODE*			m_pTailNode;			//尾节点
};

//多重链表冒泡排序算法
template<class TNODE,int nIndex,typename TParam>
void FastMultiListSortT(CFastMultiListT<TNODE,nIndex>& _list,
				int (*FuncNodeCompare)(TNODE* ,TNODE*,TParam),
				TParam nParam)
{
	//以下是对链表的冒泡排序算法：
	TNODE* pNodePointer;
	//取得最后一个节点
	TNODE* pTailNode = _list.GetTail();

	//冒泡排序
	while(_list.GetHead() != pTailNode)
	{
		pNodePointer = _list.GetHead();
		while(pNodePointer != pTailNode)
		{
			if(NULL == pNodePointer->m_pNextNode[nIndex])
			{
				break;
			}

			//需要交换链表?
			if( (*FuncNodeCompare)(pNodePointer,pNodePointer->m_pNextNode[nIndex],nParam) > 0)	
			{
				if(pTailNode == pNodePointer->m_pNextNode[nIndex])
				{
					pTailNode = pNodePointer;
				}
				//交换前后两个节点
				_list.SwapNode(pNodePointer,pNodePointer->m_pNextNode[nIndex]);
			}
			else	//不需要交换链表
			{
				pNodePointer = pNodePointer->m_pNextNode[nIndex];
			}

		} //while(pNodePointer != pTailNode)

		//前进一个位移
		pTailNode = pNodePointer->m_pPrevNode[nIndex];
	}
}

//Hash链表定义
template<class TNODE,int nHashLen>
class CFastHashListT
{
public:
	FAST_INLINE CFastHashListT():m_nListCount(0)
	{
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
	}
	FAST_INLINE int GetCount() const
	{ 
		return m_nListCount;	
	}
	FAST_INLINE TNODE* GetHead(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		return m_HashNode[nHashIndex].m_pHeadNode;
	}
	FAST_INLINE TNODE* GetTail(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		return m_HashNode[nHashIndex].m_pTailNode;
	}
	FAST_INLINE void PushBack(int nHashIndex,TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		pNode->m_pNextNode = NULL;
		pNode->m_pPrevNode = m_HashNode[nHashIndex].m_pTailNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_HashNode[nHashIndex].m_pTailNode)
		{
			m_HashNode[nHashIndex].m_pHeadNode = pNode;
		}
		else	//list not empty
		{
			m_HashNode[nHashIndex].m_pTailNode->m_pNextNode = pNode;
		}
		m_HashNode[nHashIndex].m_pTailNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE void PushFront(int nHashIndex,TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		pNode->m_pPrevNode = NULL;
		pNode->m_pNextNode = m_HashNode[nHashIndex].m_pHeadNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_HashNode[nHashIndex].m_pHeadNode)
		{
			m_HashNode[nHashIndex].m_pTailNode = pNode;
		}
		else
		{
			m_HashNode[nHashIndex].m_pHeadNode->m_pPrevNode = pNode;
		}
		m_HashNode[nHashIndex].m_pHeadNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE TNODE* PopBack(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		//得到弹出的Node
		TNODE* pNode = m_HashNode[nHashIndex].m_pTailNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_HashNode[nHashIndex].m_pTailNode = (TNODE*)pNode->m_pPrevNode;

			if(NULL != m_HashNode[nHashIndex].m_pTailNode)
			{
				m_HashNode[nHashIndex].m_pTailNode->m_pNextNode = NULL;
			}
			else
			{
				m_HashNode[nHashIndex].m_pHeadNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE TNODE* PopFront(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		//得到弹出的Node
		TNODE* pNode = m_HashNode[nHashIndex].m_pHeadNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_HashNode[nHashIndex].m_pHeadNode = (TNODE*)pNode->m_pNextNode;

			if(NULL != m_HashNode[nHashIndex].m_pHeadNode)
			{
				m_HashNode[nHashIndex].m_pHeadNode->m_pPrevNode = NULL;
			}
			else
			{
				m_HashNode[nHashIndex].m_pTailNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE void DeleteNode(int nHashIndex,TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		if(NULL != pNode->m_pNextNode)
		{
			pNode->m_pNextNode->m_pPrevNode = pNode->m_pPrevNode;
		}
		if(NULL != pNode->m_pPrevNode)
		{
			pNode->m_pPrevNode->m_pNextNode = pNode->m_pNextNode;
		}
		if(m_HashNode[nHashIndex].m_pTailNode == pNode)	//delete tail?
		{
			m_HashNode[nHashIndex].m_pTailNode = (TNODE*)pNode->m_pPrevNode;
		}
		if(m_HashNode[nHashIndex].m_pHeadNode == pNode) //delete head?
		{
			m_HashNode[nHashIndex].m_pHeadNode = (TNODE*)pNode->m_pNextNode;
		}
		m_nListCount --;
	}
	FAST_INLINE void InsertNodeBefore(int nHashIndex,TNODE* pNode,TNODE* pNodeBefore)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		if(NULL == pNodeBefore)
		{
			PushFront(nHashIndex,pNode);
			return ;
		}
		if(NULL != pNodeBefore->m_pPrevNode)
		{
			pNodeBefore->m_pPrevNode->m_pNextNode = pNode;
		}
		pNode->m_pPrevNode = pNodeBefore->m_pPrevNode;
		pNodeBefore->m_pPrevNode = pNode;
		pNode->m_pNextNode = pNodeBefore;
		//是否插入到链表头?
		if(m_HashNode[nHashIndex].m_pHeadNode == pNodeBefore)
		{
			m_HashNode[nHashIndex].m_pHeadNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE void InsertNodeAfter(int nHashIndex,TNODE* pNode,TNODE* pNodeAfter)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		if(NULL == pNodeAfter)
		{
			PushBack(nHashIndex,pNode);
			return ;
		}
		if(NULL != pNodeAfter->m_pNextNode)
		{
			pNodeAfter->m_pNextNode->m_pPrevNode = pNode;
		}
		pNode->m_pNextNode = pNodeAfter->m_pNextNode;
		pNodeAfter->m_pNextNode = pNode;
		pNode->m_pPrevNode = pNodeAfter;
		//是否插入到链表尾?
		if(m_HashNode[nHashIndex].m_pTailNode == pNodeAfter)
		{
			m_HashNode[nHashIndex].m_pTailNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE TNODE* GetNth(int nHashIndex,int nNodeNum) const
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		TNODE* pNode;
		if(nNodeNum >= 0)
		{
			pNode = m_HashNode[nHashIndex].m_pHeadNode;
			while (nNodeNum-- > 0 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pNextNode;
			}
		}
		else
		{
		    pNode = m_HashNode[nHashIndex].m_pTailNode;
			while (nNodeNum++ < -1 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pPrevNode;
			}		
		}
		return pNode;
	}
	FAST_INLINE BOOL IsHashEmpty(int nHashIndex) const
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		return (NULL == m_HashNode[nHashIndex].m_pHeadNode);
	}
	FAST_INLINE BOOL IsEmpty() const
	{
		return (0 == m_nListCount);
	}
	FAST_INLINE void Clear()
	{
		m_nListCount = 0;
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
	}
	//交换两个相邻的节点
	FAST_INLINE void SwapNode(int nHashIndex,TNODE* pNode,TNODE* pNextNode)
	{
		FASTASSERT(NULL != pNode && NULL != pNextNode);

		TNODE* pPrevNodeV;
		TNODE* pPrevNode;
		TNODE* pNextNodeY;

		//交换节点
		pPrevNodeV = pNode->m_pPrevNode;
		pPrevNode = pNode;
		pNextNodeY = pNextNode->m_pNextNode;

		//之前：P=pNodePointer
		//       V         P          N        Y
		//       *=>p   v<=*=>n    p<=*=>y  n<=*
		//之后：
		//       V         N          P        Y    
		//       *=>n   v<=*=>p    n<=*=>y  p<=*
		
		//V(*=>p) = V(*=>n)
		if(NULL != pPrevNodeV)
		{
			pPrevNodeV->m_pNextNode = pNextNode;
		}
		//N(p<=*) = N(v<=*)
		pNextNode->m_pPrevNode = pPrevNodeV;
		//N(*=>y) = N(*=>p)
		pNextNode->m_pNextNode = pPrevNode;
		//P(v<=*) = P(n<=*)
		pPrevNode->m_pPrevNode = pNextNode;
		//P(*=>n) = P(*=>y)
		pPrevNode->m_pNextNode = pNextNodeY;
		//Y(n<=*) = Y(p<=*)
		if(NULL != pNextNodeY)
		{
			pNextNodeY->m_pPrevNode = pPrevNode;
		}
		//是否移动了头指针?
		if(pPrevNode == m_HashNode[nHashIndex].m_pHeadNode)
		{
			m_HashNode[nHashIndex].m_pHeadNode = pNextNode;
		}
		//是否移动了尾指针?
		if(pNextNode == m_HashNode[nHashIndex].m_pTailNode)
		{
			m_HashNode[nHashIndex].m_pTailNode = pPrevNode;
		}				
	}
private:
	typedef struct tagHashList
	{
		TNODE*		m_pHeadNode;					//头节点
		TNODE*		m_pTailNode;					//尾节点
	}HASHLIST;	
	int				m_nListCount;					//链表个数
	HASHLIST		m_HashNode[nHashLen];			//Hash节点
};


//Hash链表冒泡排序算法
template<class TNODE,int nHashLen,typename TParam>
void FastHashListSortT(CFastHashListT<TNODE,nHashLen>& _list,
					   int nHashIndex,
					   int (*FuncNodeCompare)(TNODE* ,TNODE*,TParam),
					  TParam nParam)
{
	//以下是对链表的冒泡排序算法：
	TNODE* pNodePointer;
	//取得最后一个节点
	TNODE* pTailNode = _list.GetTail(nHashIndex);

	//冒泡排序
	while(_list.GetHead(nHashIndex) != pTailNode)
	{
		pNodePointer = _list.GetHead(nHashIndex);
		while(pNodePointer != pTailNode)
		{
			if(NULL == pNodePointer->m_pNextNode)
			{
				break;
			}

			//需要交换链表?
			if( (*FuncNodeCompare)(pNodePointer,pNodePointer->m_pNextNode,nParam) > 0)	
			{
				if(pTailNode == pNodePointer->m_pNextNode)
				{
					pTailNode = pNodePointer;
				}
				//交换前后两个节点
				_list.SwapNode(nHashIndex,pNodePointer,pNodePointer->m_pNextNode);
			}
			else	//不需要交换链表
			{
				pNodePointer = pNodePointer->m_pNextNode;
			}

		} //while(pNodePointer != pTailNode)

		//前进一个位移
		pTailNode = pNodePointer->m_pPrevNode;
	}
}

//多重Hash链表定义
template<class TNODE,int nHashLen,int nIndex>
class CFastMultiHashListT
{
public:
	FAST_INLINE CFastMultiHashListT():m_nListCount(0)
	{
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
	}
	FAST_INLINE int GetCount() const
	{ 
		return m_nListCount;	
	}
	FAST_INLINE TNODE* GetHead(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		return m_HashNode[nHashIndex].m_pHeadNode;
	}
	FAST_INLINE TNODE* GetTail(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		return m_HashNode[nHashIndex].m_pTailNode;
	}
	FAST_INLINE void PushBack(int nHashIndex,TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		pNode->m_pNextNode[nIndex] = NULL;
		pNode->m_pPrevNode[nIndex] = m_HashNode[nHashIndex].m_pTailNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_HashNode[nHashIndex].m_pTailNode)
		{
			m_HashNode[nHashIndex].m_pHeadNode = pNode;
		}
		else	//list not empty
		{
			m_HashNode[nHashIndex].m_pTailNode->m_pNextNode[nIndex] = pNode;
		}
		m_HashNode[nHashIndex].m_pTailNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE void PushFront(int nHashIndex,TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		pNode->m_pPrevNode[nIndex] = NULL;
		pNode->m_pNextNode[nIndex] = m_HashNode[nHashIndex].m_pHeadNode;
		//if(0 == m_nListCount)	//First Insert
		if(NULL == m_HashNode[nHashIndex].m_pHeadNode)
		{
			m_HashNode[nHashIndex].m_pTailNode = pNode;
		}
		else
		{
			m_HashNode[nHashIndex].m_pHeadNode->m_pPrevNode[nIndex] = pNode;
		}
		m_HashNode[nHashIndex].m_pHeadNode = pNode;
		m_nListCount ++;
	}
	FAST_INLINE TNODE* PopBack(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		//得到弹出的Node
		TNODE* pNode = m_HashNode[nHashIndex].m_pTailNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_HashNode[nHashIndex].m_pTailNode = (TNODE*)pNode->m_pPrevNode[nIndex];

			if(NULL != m_HashNode[nHashIndex].m_pTailNode)
			{
				m_HashNode[nHashIndex].m_pTailNode->m_pNextNode[nIndex] = NULL;
			}
			else
			{
				m_HashNode[nHashIndex].m_pHeadNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE TNODE* PopFront(int nHashIndex)
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		//得到弹出的Node
		TNODE* pNode = m_HashNode[nHashIndex].m_pHeadNode;
		if(NULL != pNode)	//弹出不为空
		{
			m_HashNode[nHashIndex].m_pHeadNode = (TNODE*)pNode->m_pNextNode[nIndex];

			if(NULL != m_HashNode[nHashIndex].m_pHeadNode)
			{
				m_HashNode[nHashIndex].m_pHeadNode->m_pPrevNode[nIndex] = NULL;
			}
			else
			{
				m_HashNode[nHashIndex].m_pTailNode = NULL;
			}
			m_nListCount --;
		}
		return pNode;
	}
	FAST_INLINE void DeleteNode(int nHashIndex,TNODE* pNode)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		if(NULL != pNode->m_pNextNode[nIndex])
		{
			pNode->m_pNextNode[nIndex]->m_pPrevNode[nIndex] = pNode->m_pPrevNode[nIndex];
		}
		if(NULL != pNode->m_pPrevNode[nIndex])
		{
			pNode->m_pPrevNode[nIndex]->m_pNextNode[nIndex] = pNode->m_pNextNode[nIndex];
		}
		if(m_HashNode[nHashIndex].m_pTailNode == pNode)	//delete tail?
		{
			m_HashNode[nHashIndex].m_pTailNode = (TNODE*)pNode->m_pPrevNode[nIndex];
		}
		if(m_HashNode[nHashIndex].m_pHeadNode == pNode) //delete head?
		{
			m_HashNode[nHashIndex].m_pHeadNode = (TNODE*)pNode->m_pNextNode[nIndex];
		}
		m_nListCount --;
	}
	FAST_INLINE void InsertNodeBefore(int nHashIndex,TNODE* pNode,TNODE* pNodeBefore)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		if(NULL == pNodeBefore)
		{
			PushFront(nHashIndex,pNode);
			return ;
		}
		if(NULL != pNodeBefore->m_pPrevNode[nIndex])
		{
			pNodeBefore->m_pPrevNode[nIndex]->m_pNextNode[nIndex] = pNode;
		}
		pNode->m_pPrevNode[nIndex] = pNodeBefore->m_pPrevNode[nIndex];
		pNodeBefore->m_pPrevNode[nIndex] = pNode;
		pNode->m_pNextNode[nIndex] = pNodeBefore;
		//是否插入到链表头?
		if(m_HashNode[nHashIndex].m_pHeadNode == pNodeBefore)
		{
			m_HashNode[nHashIndex].m_pHeadNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE void InsertNodeAfter(int nHashIndex,TNODE* pNode,TNODE* pNodeAfter)
	{
		FASTASSERT(NULL != pNode);
//		if(NULL == pNode)
//		{
//			return;
//		}

		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		if(NULL == pNodeAfter)
		{
			PushBack(nHashIndex,pNode);
			return ;
		}
		if(NULL != pNodeAfter->m_pNextNode[nIndex])
		{
			pNodeAfter->m_pNextNode[nIndex]->m_pPrevNode[nIndex] = pNode;
		}
		pNode->m_pNextNode[nIndex] = pNodeAfter->m_pNextNode[nIndex];
		pNodeAfter->m_pNextNode[nIndex] = pNode;
		pNode->m_pPrevNode[nIndex] = pNodeAfter;
		//是否插入到链表尾?
		if(m_HashNode[nHashIndex].m_pTailNode == pNodeAfter)
		{
			m_HashNode[nHashIndex].m_pTailNode = pNode;
		}
		m_nListCount ++;
	}
	FAST_INLINE TNODE* GetNth(int nHashIndex,int nNodeNum) const
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		TNODE* pNode;
		if(nNodeNum >= 0)
		{
			pNode = m_HashNode[nHashIndex].m_pHeadNode;
			while (nNodeNum-- > 0 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pNextNode[nIndex];
			}
		}
		else
		{
		    pNode = m_HashNode[nHashIndex].m_pTailNode;
			while (nNodeNum++ < -1 && NULL != pNode)
			{
				pNode = (TNODE*)pNode->m_pPrevNode[nIndex];
			}		
		}
		return pNode;
	}
	FAST_INLINE BOOL IsHashEmpty(int nHashIndex) const
	{
		FASTASSERT(nHashIndex >= 0 && nHashIndex <nHashLen );
		return (NULL == m_HashNode[nHashIndex].m_pHeadNode);
	}
	FAST_INLINE BOOL IsEmpty() const
	{
		return (0 == m_nListCount);
	}
	FAST_INLINE void Clear()
	{
		m_nListCount = 0;
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
		memset(m_HashNode,0,sizeof(HASHLIST)*nHashLen);
	}
	//交换两个相邻的节点
	FAST_INLINE void SwapNode(int nHashIndex,TNODE* pNode,TNODE* pNextNode)
	{
		FASTASSERT(NULL != pNode && NULL != pNextNode);

		TNODE* pPrevNodeV;
		TNODE* pPrevNode;
		TNODE* pNextNodeY;

		//交换节点
		pPrevNodeV = pNode->m_pPrevNode[nIndex];
		pPrevNode = pNode;
		pNextNodeY = pNextNode->m_pNextNode[nIndex];

		//之前：P=pNodePointer
		//       V         P          N        Y
		//       *=>p   v<=*=>n    p<=*=>y  n<=*
		//之后：
		//       V         N          P        Y    
		//       *=>n   v<=*=>p    n<=*=>y  p<=*
		
		//V(*=>p) = V(*=>n)
		if(NULL != pPrevNodeV)
		{
			pPrevNodeV->m_pNextNode[nIndex] = pNextNode;
		}
		//N(p<=*) = N(v<=*)
		pNextNode->m_pPrevNode[nIndex] = pPrevNodeV;
		//N(*=>y) = N(*=>p)
		pNextNode->m_pNextNode[nIndex] = pPrevNode;
		//P(v<=*) = P(n<=*)
		pPrevNode->m_pPrevNode[nIndex] = pNextNode;
		//P(*=>n) = P(*=>y)
		pPrevNode->m_pNextNode[nIndex] = pNextNodeY;
		//Y(n<=*) = Y(p<=*)
		if(NULL != pNextNodeY)
		{
			pNextNodeY->m_pPrevNode[nIndex] = pPrevNode;
		}
		//是否移动了头指针?
		if(pPrevNode == m_HashNode[nHashIndex].m_pHeadNode)
		{
			m_HashNode[nHashIndex].m_pHeadNode = pNextNode;
		}
		//是否移动了尾指针?
		if(pNextNode == m_HashNode[nHashIndex].m_pTailNode)
		{
			m_HashNode[nHashIndex].m_pTailNode = pPrevNode;
		}				
	}
private:
	typedef struct tagHashList
	{
		TNODE*		m_pHeadNode;					//头节点
		TNODE*		m_pTailNode;					//尾节点
	}HASHLIST;	
	int				m_nListCount;					//链表个数
	HASHLIST		m_HashNode[nHashLen];			//Hash节点
};


//多重Hash链表冒泡排序算法
template<class TNODE,int nHashLen,int nIndex,typename TParam>
void FastMultiHashListSortT(CFastMultiHashListT<TNODE,nHashLen,nIndex>& _list,
					    int nHashIndex,
						int (*FuncNodeCompare)(TNODE* ,TNODE*,TParam),
						TParam nParam)
{
	//以下是对链表的冒泡排序算法：
	TNODE* pNodePointer;
	//取得最后一个节点
	TNODE* pTailNode = _list.GetTail(nHashIndex);

	//冒泡排序
	while(_list.GetHead(nHashIndex) != pTailNode)
	{
		pNodePointer = _list.GetHead(nHashIndex);
		while(pNodePointer != pTailNode)
		{
			if(NULL == pNodePointer->m_pNextNode[nIndex])
			{
				break;
			}

			//需要交换链表?
			if( (*FuncNodeCompare)(pNodePointer,pNodePointer->m_pNextNode[nIndex],nParam) > 0)	
			{
				if(pTailNode == pNodePointer->m_pNextNode[nIndex])
				{
					pTailNode = pNodePointer;
				}
				//交换前后两个节点
				_list.SwapNode(nHashIndex,pNodePointer,pNodePointer->m_pNextNode[nIndex]);
			}
			else	//不需要交换链表
			{
				pNodePointer = pNodePointer->m_pNextNode[nIndex];
			}

		} //while(pNodePointer != pTailNode)

		//前进一个位移
		pTailNode = pNodePointer->m_pPrevNode[nIndex];
	}
}

#endif //__cplusplus

/*lint -restore */

#endif //__FASTLIST_H_INCLUDED

