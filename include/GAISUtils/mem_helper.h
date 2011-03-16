#ifndef _MEM_HELPER_H
#define _MEM_HELPER_H

#include <memory>
#include "loki/SmallObj.h"

#ifndef WIN32
	#include "ext/pool_allocator.h"
#endif 

namespace STL
{
#ifndef WIN32
	using __gnu_cxx::__pool_alloc;
#else
	template<class T> 
	struct __pool_alloc :public std::allocator<T>{};
	//typedef template<class T> std::allocator<T> __pool_alloc;
#endif
}

// Desc: A functor that makes a safe buffer which guarantees
// 	1. The 'end' (the element after the last element) is managed and set to zero
// 	2. Every element will be sat to zero if zeroOut flag is true
// Method:
//	Elem* operator()(unsigned int size, bool zeroOut = true);
// Remark:
// 	I offer a example to release this buffer safely.
// 	
// 	char * buf = safeBuffer<char>(1024);
//	delete []buf;
//	buf = 0;
template<typename Elem>
struct safeBuffer
{
	
	// Params:
	// 	size - buffer size
	// 	zeroOut - set this flag let the buffer to be zero-outted
	// Return:
	// 	pointer to the buffer
	Elem* operator()(Elem** pptr, unsigned int size, bool zeroOut = false)
	{
		if(!size) return 0;
		//Elem* tmp = new Elem[size+1];
		if(*pptr) delete [] *pptr;
		*pptr  = new Elem[size+1]; //(Elem*)realloc(*pptr, (size + 1) * sizeof(Elem) );
		if(zeroOut)
			memset(*pptr, 0, (1+size)*sizeof(Elem));
		return *pptr;
	}
	
	// Params:
	// 	size - buffer size
	// 	zeroOut - set this flag let the buffer to be zero-outted
	// Return:
	// 	pointer to the buffer
	Elem* operator()(unsigned int size, bool zeroOut = false)
	{
		Elem *ptr;
		if(!size) return 0;
		ptr = new Elem[size+1];
		//ptr  = (Elem*) malloc( (size+1) *sizeof(Elem) ); //new Elem[size+1];
		if(zeroOut)
			memset(ptr, 0, (1+size)*sizeof(Elem));
		return ptr;
	}
};


template<typename Elem>
void safeFreeBuffer(Elem** buf);


template
<	
	typename T,
	template <class, class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
        std::size_t chunkSize = LOKI_DEFAULT_CHUNK_SIZE,
        std::size_t maxSmallObjectSize = LOKI_MAX_SMALL_OBJECT_SIZE,
        std::size_t objectAlignSize = LOKI_DEFAULT_OBJECT_ALIGNMENT,
        template <class> class LifetimePolicy = LOKI_DEFAULT_SMALLOBJ_LIFETIME,
        class MutexPolicy = LOKI_DEFAULT_MUTEX

>
class smallobj_allocator
{
public:
	typedef Loki::AllocatorSingleton< ThreadingModel, chunkSize,
		maxSmallObjectSize, objectAlignSize, LifetimePolicy > ObjAllocatorSingleton;
private:
	typedef ThreadingModel< ObjAllocatorSingleton, MutexPolicy > MyThreadingModel;
	typedef typename ObjAllocatorSingleton::MyAllocatorSingleton MyAllocatorSingleton;
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef T const * const_pointer;
	typedef T& reference;
	typedef T const & const_reference;
	typedef T value_type;

	template<typename T1>
	struct rebind
	{ typedef smallobj_allocator<T1> other; };

	smallobj_allocator() throw() {}

	smallobj_allocator(smallobj_allocator const&) throw() {}

	template<typename T1>
	smallobj_allocator(smallobj_allocator<T1> const &) throw() {}

	~smallobj_allocator() throw() {}
	
	pointer
	address(reference x) const { return &x; }

	const_pointer
	address(const_reference x) const { return &x; }

	pointer
	allocate(size_type n, void const* = 0)
	{
		typename MyThreadingModel::Lock lock;
		(void)lock; // get rid of warning
		return static_cast<pointer>(MyAllocatorSingleton::Instance().Allocate( sizeof(T) * n, true ));
	}

	void
	deallocate(pointer p, size_type s)
	{
		typename MyThreadingModel::Lock lock;
            	(void)lock; // get rid of warning
            	MyAllocatorSingleton::Instance().Deallocate(p, sizeof(T)*s );
	}
	
	size_type
	max_size() const throw()
	{ return size_t(-1) / sizeof(T); }

	void
	construct(pointer p, T const & val)
	{ new(p) T(val); }
	
	void destroy(pointer p)
	{ p->~T(); }
};

template<typename T>
inline bool
operator == (smallobj_allocator<T> const &, smallobj_allocator<T> const &)
{ return true; }

template<typename T>
inline bool
operator != (smallobj_allocator<T> const &, smallobj_allocator<T> const &)
{ return false; }


#include "mem_helper.inl"

#endif

