#ifndef _FAST_VECTOR_H_INCLUDED
#define _FAST_VECTOR_H_INCLUDED


//ASSERT的定义
#ifdef BISPASSERT
#define VECTORASSERT(f) BISPASSERT(f)
#else 
#define VECTORASSERT(f) assert(f)
#endif //BISPASSERT


#define DEFAULT_STEP_COUNT 32
#define VECTOR_MALLOC(size) malloc(size)
#define VECTOR_FREE(p) free(p)


//内联及寄存器定义
#define VECTOR_INLINE inline
#define VECTOR_REGISTER register

VECTOR_INLINE void* operator new(size_t size, void* p ,int /*nouse*/) 
{ return p; }

/*lint -save -efunc(1532,operator delete) */
VECTOR_INLINE void operator delete(void* p, void* pPlace , int /*nouse*/)
{ }
/*lint -restore */

template<class TYPE>
VECTOR_INLINE void ConstructElements(TYPE* pElements,int nCount)
{
	// first do bit-wise zero initialization
	memset((void*)pElements, 0, nCount * sizeof(TYPE));

	// then call the constructor(s)
	for (; nCount--; pElements++)
		::new((void*)pElements,0) TYPE;
}

template<class TYPE>
VECTOR_INLINE void DestructElements(TYPE* pElements,int nCount)
{
	// call the destructor(s)
	for (; nCount--; pElements++)
		pElements->~TYPE();
}

template<class TYPE>
VECTOR_INLINE void CopyElements(TYPE* pDest, const TYPE* pSrc,int nCount)
{
	// default is element-copy using assignment
	while (nCount--)
		*pDest++ = *pSrc++;
}


template<class TYPE ,class ARG_TYPE ,int nStepCount = DEFAULT_STEP_COUNT>
class CFastVectorT
{
public:
	CFastVectorT():m_nCurrenCount(0),m_nAllocCount(0),m_pData(NULL)
	{
		
	}

	~CFastVectorT()
	{
		RemoveAll();
	}

	void Init()
	{
		m_nCurrenCount	= 0		;
		m_nAllocCount	= 0		;
		m_pData			= NULL	;
	}

	TYPE& ElementAt( int nIndex )
	{
		VECTORASSERT( (nIndex >= 0)&&( (UINT)nIndex < m_nCurrenCount) );
		return m_pData[nIndex];
	}
	TYPE GetAt( int nIndex ) const
	{
		VECTORASSERT( (nIndex >= 0)&&( (UINT)nIndex < m_nCurrenCount) );
		return m_pData[nIndex];
	}
	TYPE& operator[](int nIndex)
	{
		return ElementAt(nIndex);
	}
	TYPE operator []( int nIndex ) const
	{
		return GetAt(nIndex);
	}
	int GetSize( ) const
	{
		return m_nCurrenCount;
	}
	void SetSize(UINT nNewSize)
	{
		if(0 == nNewSize)
		{
			if(NULL != m_pData)
			{
				DestructElements(m_pData,m_nCurrenCount);
				VECTOR_FREE(m_pData);
				m_pData = NULL;
			}
			m_nAllocCount = 0;
		}
		else if(m_nAllocCount < nNewSize)
		{
	
			m_nAllocCount = (nNewSize / nStepCount + 1)*nStepCount;

			VECTORASSERT(m_nAllocCount >= nNewSize);

			TYPE* pNewData = (TYPE*)VECTOR_MALLOC(sizeof(TYPE)*m_nAllocCount );
			if(NULL != m_pData)
			{
				memcpy(pNewData,m_pData,sizeof(TYPE)*m_nCurrenCount);
				VECTOR_FREE(m_pData);
			}
			m_pData = pNewData;
			ConstructElements(m_pData + m_nCurrenCount,nNewSize - m_nCurrenCount);	
		}
		else if(m_nCurrenCount > nNewSize)
		{
			if(NULL != m_pData)
			{
				DestructElements(m_pData + nNewSize, m_nCurrenCount - nNewSize);
			}
		}
		else //if (nNewSize >= m_nCurrenCount && nNewSize <= m_nAllocCount)
		{
			VECTORASSERT(nNewSize >= m_nCurrenCount && nNewSize <= m_nAllocCount);

			if(NULL != m_pData)
			{
				ConstructElements(m_pData + m_nCurrenCount,nNewSize - m_nCurrenCount);	
			}
		}
		m_nCurrenCount = nNewSize;
	}
	int Add(ARG_TYPE newElement)
	{
		int nIndex = m_nCurrenCount;
		SetSize((UINT)nIndex + 1);
		VECTORASSERT(m_pData);
		m_pData[nIndex] = newElement;
		return nIndex;
	}

	// Clean up
	void RemoveAll()
	{
		SetSize(0);
	}
	void Copy(const CFastVectorT& src)
	{
		if(&src != this)
		{
			SetSize(src.m_nCurrenCount);
			CopyElements<TYPE>(m_pData, src.m_pData, src.m_nCurrenCount);
		}
	}
	CFastVectorT& operator=(const CFastVectorT& src)
	{
		Copy(src);
		return *this;
	}
	TYPE* GetData()
	{
		return m_pData;
	}

protected:	
	UINT	m_nCurrenCount;
	UINT	m_nAllocCount;
	TYPE*	m_pData;

};



#endif	//_FAST_VECTOR_H_INCLUDED


