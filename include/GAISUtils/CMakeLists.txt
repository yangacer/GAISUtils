
set(	files dll.h lexUtils.h lexUtils.inl fileUtils.h 
	container.h container.inl mem_helper.h mem_helper.inl 
	hash_helper.h timeUtils.h profiler.h allocator.h
	rstream.h record.h small_vector.h rschema.h rserialize.h) 

install (FILES ${files} DESTINATION include/GAISUtils)

