set( HEADERS
	AbstractFactory.h
	Allocator.h
	AssocVector.h
	CMakeLists.txt
	CachedFactory.h
	CheckReturn.h
	Checker.h
	ConstPolicy.h
	DataGenerators.h
	EmptyType.h
	Factory.h
	ForEachType.h
	Function.h
	Functor.h
	HierarchyGenerators.h
	Key.h
	LevelMutex.h
	LockingPtr.h
	LokiExport.h
	LokiTypeInfo.h
	MultiMethods.h
	NullType.h
	OrderedStatic.h
	Pimpl.h
	RefToValue.h
	Register.h
	SPCachedFactory.h
	SafeBits.h
	SafeFormat.h
	ScopeGuard.h
	Sequence.h
	Singleton.h
	SmallObj.h
	SmartPtr.h
	StrongPtr.h
	ThreadLocal.h
	Threads.h
	Tuple.h
	TypeManip.h
	TypeTraits.h
	Typelist.h
	TypelistMacros.h
	Visitor.h )

install (FILES ${HEADERS} DESTINATION include/loki)
