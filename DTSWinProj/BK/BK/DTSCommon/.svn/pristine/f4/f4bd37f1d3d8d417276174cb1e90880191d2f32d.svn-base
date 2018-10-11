#ifndef BISPMULTICHILDTREE_H_INCLUDED
#define BISPMULTICHILDTREE_H_INCLUDED

#include "FastList.h"


template<class T>
class CFastTreeNodeT
{
public:
	FAST_INLINE CFastTreeNodeT():m_pNextNode(NULL),m_pPrevNode(NULL)
	{m_ListChild.Clear();}

	FAST_INLINE void InitNode()
	{
		m_pNextNode = m_pPrevNode = NULL;
		m_ListChild.Clear();
	}

	T*	m_pFatherNode	;		//父节点指针
	T*	m_pNextNode		;		//同级的下一个节点
	T*	m_pPrevNode		;		//同级的前一个节点
	CFastListT<CFastTreeNodeT>	m_ListChild;		//子节点列表
};


template<class TNODE>
class CFastTreeT
{
public:
	FAST_INLINE CFastTreeT(): m_nTreeCount(0)
	{
		//建立空的根节点
		m_pRootNode = &m_RootNode;
		m_pRootNode->m_pNextNode = NULL;
		m_pRootNode->m_pPrevNode = NULL;
		m_pRootNode->m_ListChild.Clear();
	}

	FAST_INLINE void Init()
	{
		//建立空的根节点
		m_pRootNode = &m_RootNode;
		m_pRootNode->m_pNextNode = NULL;
		m_pRootNode->m_pPrevNode = NULL;
		m_pRootNode->m_ListChild.Clear();

		m_nTreeCount = 0;
	}


	FAST_INLINE int GetCount() const
	{ 
		return (int)m_nTreeCount;	
	}

	FAST_INLINE TNODE* GetRoot() 
	{
		return m_pRootNode;
	}

	FAST_INLINE BOOL IsLeaf(TNODE* pNode) const
	{
		BISPASSERT(NULL!=pNode);
		return pNode->m_ListChild.IsEmpty();
	}

	//父节点必须在树里面，子节点不能已经在树里面(没有引用计数)
	//由调用者检测
	FAST_INLINE BOOL InsertNode(TNODE* pFatherNode, TNODE* pInsertNode)
	{
		if (NULL!=pFatherNode)
		{
			pFatherNode->m_ListChild.PushBack(pInsertNode);
			pInsertNode->m_pFatherNode = pFatherNode;
		}
		else
		{
			if (NULL==m_pRootNode)
			{
				m_pRootNode = pInsertNode;
				pInsertNode->m_pFatherNode = NULL; 
			}
			else
			{
				return FALSE;
			}
		}	
		m_nTreeCount++;
		return TRUE;
	}

	//由于存在空间回收的问题
	//只是将节点从父节点的列表中移出，迭代删除子节点由调用者完成
	FAST_INLINE void DeleteNode(TNODE* pDelNode)
	{
		if (pDelNode->m_pFatherNode!=NULL)
		{
			//删除父节点指向
			pDelNode->m_pFatherNode->m_ListChild.DeleteNode(pDelNode);
		}
		else
		{
			m_pRootNode = NULL;
		}	
		m_nTreeCount--;
	}

	//是否是子节点
	FAST_INLINE BOOL IsChild(TNODE* pFather, TNODE* pChild)
	{
		TNODE* pTmp;
		for (pTmp  = pFather->m_ListChild.GetHead();
			 pTmp != NULL;
			 pTmp  = pTmp->m_pNextNode)
		{
			if ( pChild == pTmp ) 
			{
				return TRUE;
			}
		}
		return FALSE;
	};

	//是否是后代，深度迭代搜索
	FAST_INLINE BOOL IsDescendant(TNODE* pFather, TNODE* pChild)
	{
		TNODE* pTmp;
		for (pTmp  = pFather->m_ListChild.GetHead();
			 pTmp != NULL;
			 pTmp  = pTmp->m_pNextNode)
		{
			if ( (pChild == pTmp) || (IsDescendant(pTmp,pChild)) )
			{
				return TRUE;
			}
		}
		return FALSE;
	}

	//深度搜索
	FAST_INLINE BOOL IsInTree(TNODE* pNode)
	{
		return IsDescendant(m_pRootNode,pNode);
	}	

	//回调函数返回TRUE则继续检索，返回FALSE则停止检索
	//深度检索
	FAST_INLINE BOOL DeepIterativeEnum(	TNODE* pNode,
										BOOL (*FuncNodeEnum)(TNODE* ,UINT,DWORD),
										UINT  nDepth,DWORD dwParam)
	{
		if (!FuncNodeEnum(pNode,nDepth,dwParam))
		{
			return FALSE;
		}
		
		TNODE* pTmp;
		for (pTmp = pNode->m_ListChild.GetHead();
			pTmp != NULL;
			pTmp  = pTmp->m_pNextNode)
		{
			if (!DeepIterativeEnum(pTmp,FuncNodeEnum,nDepth+1,dwParam))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	
	//广度检索

	UINT	m_nTreeCount;
	TNODE*	m_pRootNode;
	TNODE	m_RootNode;
};


#endif	//	BISPMULTICHILDTREE_H_INCLUDED



