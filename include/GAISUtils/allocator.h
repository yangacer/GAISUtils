#ifndef _GUTils_STLSmallObjAllocator_H
#define _GUTils_STLSmallObjAllocator_H

#include <memory>
#include <loki/Singleton.h>
#include <loki/SmallObj.h>

template<typename T>
class STLSmallObjAllocator {
	public : 
		//    typedefs

		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef std::size_t size_type;
		typedef std::ptrdiff_t difference_type;
		
		typedef Loki::AllocatorSingleton<LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL, 128> allocator_;

	public : 
		//    convert an allocator<T> to allocator<U>

		template<typename U>
		struct rebind {
			typedef STLSmallObjAllocator<U> other;
		};

	public : 
		STLSmallObjAllocator(){}
		~STLSmallObjAllocator() {}
		STLSmallObjAllocator(STLSmallObjAllocator const&) {}
		//inline explicit STLSmallObjAllocator(STLSmallObjAllocator) {}
		//inline explicit STLSmallObjAllocator(STLSmallObjAllocator&) {}

		template<typename U>
		STLSmallObjAllocator(STLSmallObjAllocator<U> const&) {}

		//    address

		inline pointer address(reference r) { return &r; }
		inline const_pointer address(const_reference r) { return &r; }

		//    memory allocation

		inline pointer allocate(size_type cnt, 
			typename std::allocator<void>::const_pointer = 0) 
		{ 
			return reinterpret_cast<T*>(allocator_::Instance().Allocate(sizeof(T) * cnt, true));
		}

		inline void deallocate(pointer p, size_type cnt) 
		{
			allocator_::Instance().Deallocate(p, sizeof(T) * cnt);
		}

		//    size

		inline size_type max_size() const { 
			return std::numeric_limits<size_type>::max() / sizeof(T);
		}

		//    construction/destruction

		inline void construct(pointer p, const T& t) { new(p) T(t); }
		inline void destroy(pointer p) { p->~T(); }

		inline bool operator==(STLSmallObjAllocator const&) { return true; }
		inline bool operator!=(STLSmallObjAllocator const& a) { return !operator==(a); }
};    //    end of class STLSmallObjAllocator 

#endif
