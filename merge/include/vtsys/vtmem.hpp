#ifndef VTMEM_HPP_INCLUDED
#define VTMEM_HPP_INCLUDED

/**  2015.07 Dehua Tan **/

#include <utility>
#include <memory>

#include "vtsys/vtmem.h"

namespace vtsys{

    template <typename VAR_T>
    class ScopeObject{
        public:

            inline ScopeObject(){
                pObj = new(VAR_T);
            }

            inline virtual ~ScopeObject(){
                if(pObj != (VAR_T*)NULL){
                    delete pObj;
                }
            }

            inline operator VAR_T&(){
                return *pObj;
            }

            inline operator const VAR_T&() const{
                return *pObj;
            }

        protected:
            VAR_T* pObj;

        private:
            inline ScopeObject(const ScopeObject& scopeRef){
            };
            inline ScopeObject& operator=(const ScopeObject& scopeRef){
                return *this;
            };
            inline void* operator new(const size_t){
                return (void*)NULL;
            }
    };

    template <typename VAR_T>
    class ScopeObjectArray{
        public:
            inline ScopeObjectArray(const size_t memberCount){
                pObjs = new VAR_T[memberCount];
            }
            inline virtual ~ScopeObjectArray(){
                if(pObjs != (VAR_T*)NULL){
                    delete [] pObjs;
                }
            }
            inline operator VAR_T*&(){
                return pObjs;
            }
            inline operator const VAR_T*&() const{
                return pObjs;
            }

        protected:
            VAR_T* pObjs;

        private:
            inline ScopeObjectArray(const ScopeObjectArray& scopeArrayRef){
            };
            inline ScopeObjectArray& operator=(const ScopeObjectArray& scopeArrayRef){
                return *this;
            };
            inline void* operator new(const size_t){
                return (void*)NULL;
            }
    };

	/**
	 * Make the resource more generic.
	 *
	 * use cases:
	 * 	void foo(){

	 * 		// use case 1
	 *      //
	 * 		char* ptr = (char*)malloc(1024);
	 * 		ScopePointerDeallocator<char, void(*)(void*)> ptrAutoDeallocator(ptr, ::free);
	 *
	 * 		// use case 2
	 *      //
	 * 		int fd = ::open("/tmp/a.txt", ...);
	 * 		ScopeHandleDeallocator<int, int(*)(int)> fdAutoDeallocator(fd, ::close);
	 *
	 * 		// use case 3,
	 *      // the resource type is not primitive type
	 *      //
	 * 		//	The resource type is:			MyResource
	 * 		//	The resource create func:		MyResource& myResource = myResourceCreate(...);
	 * 		//	The resource release function:	void myResourceRelease(MyResource& resource);
	 * 		//
	 * 		MyResource& myResource = myResourceFactory.create();
	 * 		ResScopedDeallocator<MyResource, void (*)(MyResource&)> myResourceDeallocator(myResource, myResourceRelease);
	 *
	 * 		//do your operations against ptr, fd, myResource
	 * 		//
	 * 		// fd will be closed by function ::close automatically while this function returns.
	 * 		// ptr will be freed by function ::free automatically while this function returns.
	 * 		// myResource will be deallocated by function myResourceRelease while this function resturns.
	 * 	}
	 **/
	template <typename ObjectType, typename Deallocator>
	class ScopePointerDeallocator{
	    /**
	     * This class is usually for pointer based object, such as memory
	    **/
		public:
			inline ScopePointerDeallocator(ObjectType*& pObject, Deallocator deallocator): pObject(pObject), deallocator(deallocator)
			{
			    needPerformDeallocator = true;
			}

			inline ~ScopePointerDeallocator(){
				if(needPerformDeallocator && pObject != (ObjectType*)0 && pObject != (ObjectType*)-1){
					deallocator(pObject);
				}
			}

			inline void keepObjectUndeallocated(){
			    needPerformDeallocator = false;
			}

		private:
			inline ScopePointerDeallocator(const ScopePointerDeallocator& autoDeallocator){
			}
			inline ScopePointerDeallocator& operator=(const ScopePointerDeallocator& autoDeallocator){
			    return *this;
			}
			inline void* operator new(const size_t){
			    return (void*)0;
			}

			ObjectType*& pObject;
			Deallocator deallocator;
			bool needPerformDeallocator;
	};

	template <typename ObjectType, typename Deallocator>
	class ScopeHandleDeallocator{
	    /**
	     * This class is usually for handle(integer) based object, such as file descriptor
	    **/
		public:
			inline ScopeHandleDeallocator(ObjectType& object, Deallocator deallocator): object(object), deallocator(deallocator)
			{
			    needPerformDeallocator = true;
			}

			inline ~ScopeHandleDeallocator(){
				if(needPerformDeallocator && object != (ObjectType)-1){
					deallocator(object);
				}
			}

			inline void keepObjectUndeallocated(){
			    needPerformDeallocator = false;
			}

		private:
			inline ScopeHandleDeallocator(const ScopeHandleDeallocator& autoDeallocator){
			}
			inline ScopeHandleDeallocator& operator=(const ScopeHandleDeallocator& autoDeallocator){
			    return *this;
			}
			inline void* operator new(const size_t){
			    return (void*)0;
			}

			ObjectType& object;
			Deallocator deallocator;
			bool needPerformDeallocator;
	};

	template <typename ObjectType>
	struct ScopeObjectNoopResetter{
	    inline virtual void operator()(ObjectType& object){
	    }
	    inline virtual ~ScopeObjectNoopResetter(){
	    }
	};
	template <typename PointerType>
	struct ScopePointerResetter : ScopeObjectNoopResetter<PointerType> {
	    inline void operator()(PointerType& ptr){
	        ptr = (PointerType)0;
	    }
	};
	template <typename HandleType>
	struct ScopeHandleResetter : ScopeObjectNoopResetter<HandleType> {
	    inline void operator()(HandleType& hnd){
	        hnd = (HandleType)(-1);
	    }
	};

	template <typename ObjectType>
	struct ScopeObjectIsValid{
	    inline virtual bool operator()(const ObjectType& object){
	        return true;
	    }
	    inline virtual ~ScopeObjectIsValid(){
	    }
	};
	template <typename PointerType>
	struct IsScopePointerValid : ScopeObjectIsValid<PointerType> {
	    inline bool operator()(const PointerType& ptr){
	        return (ptr != (PointerType)0 && ptr != (PointerType)(-1));
	    }
	};
	template <typename HandleType>
	struct IsScopeHandleValid : ScopeObjectIsValid<HandleType> {
	    inline bool operator()(const HandleType& hnd){
	        return (hnd != (HandleType)(-1));
	    }
	};

	template <
        typename ObjectType,
        typename Deallocator,
        typename Validator = ScopeObjectIsValid<ObjectType>,
        typename Resetter = ScopeObjectNoopResetter<ObjectType>
    >
	class ScopeObjectDeallocator{
	    /**
	     * This class is for generic purpose.
	    **/
		public:
			inline ScopeObjectDeallocator(
                ObjectType& object,
                Deallocator deallocator,
                Validator validator = Validator(),
                Resetter resetter = Resetter()
            ):  object(object),
                deallocator(deallocator),
                validator(validator),
                resetter(resetter)
			{
			    needPerformDeallocator = true;
			}

			inline virtual ~ScopeObjectDeallocator(){
			    if(needPerformDeallocator && validator(object)){
                    deallocator(object);
                    resetter(object);
			    }
			}

			inline void keepObjectUndeallocated(){
			    needPerformDeallocator = false;
			}

		private:
			inline ScopeObjectDeallocator(const ScopeObjectDeallocator& autoDeallocator){
			}
			inline ScopeObjectDeallocator& operator=(const ScopeObjectDeallocator& autoDeallocator){
			    return *this;
			}
			inline void* operator new(const size_t){
			    return (void*)0;
			}

			ObjectType& object;
			Deallocator deallocator;
			Validator validator;
			Resetter resetter;
			bool needPerformDeallocator;
	};


	template <typename ObjectType, typename Deallocator >
	class ScopePointerDeallocator2 : public ScopeObjectDeallocator<ObjectType, Deallocator, IsScopePointerValid<ObjectType>, ScopePointerResetter<ObjectType> >{
	};

	template <typename ObjectType, typename Deallocator >
	class ScopeHandleDeallocator2 : public ScopeObjectDeallocator<ObjectType, Deallocator, IsScopeHandleValid<ObjectType>, ScopeHandleResetter<ObjectType> >{
	};

	/**
	 * Due to new OBJ_T(args) is hard to trace memory automatically.
	 * So this is defined for the purpose.
	 *
	 * pair to freeObj(pObj)
	**/
    #define mallocObj(pObj, OBJ_T, args...) \
        OBJ_T* pObj = (OBJ_T*)vt_malloc(sizeof(OBJ_T)); \
        if (pObj != (OBJ_T*)0 && pObj != (OBJ_T*)-1){ \
            new (pObj) OBJ_T(args); \
        }

	/**
	 * Due to new OBJ_T[size] with non default constructor is impossible
	 * and hard to trace memory automatically.
	 * So this is defined for the purpose.
	 *
	 * pair to freeObjArray(pArray)
	**/
    #define mallocObjArray(pArray, OBJ_T, numOfObj, args...) \
        OBJ_T* pArray = (OBJ_T*)vt_malloc(sizeof(OBJ_T) * (size_t)numOfObj + sizeof(size_t)); \
        if(pArray != (OBJ_T*)0 && pArray != (OBJ_T*)-1){ \
            *(size_t*)pArray = (size_t)numOfObj; \
            pArray = (OBJ_T*)(((char*)pArray) + sizeof(size_t)); \
            for(size_t i = 0; i < (size_t)numOfObj; i ++){ \
                new (pArray + i) OBJ_T(args); \
            } \
        }

    /**
     * pair to: mallocObj(...)
    **/
    template <typename ObjectType>
    void freeObj(ObjectType* pObj){
	    if(pObj != (ObjectType*)0 && pObj != (ObjectType*)-1){
            pObj->~ObjectType();
            vt_free(pObj);
	    }
    }

    /**
     * pair to: mallocObjArray(...)
    **/
    template <typename ObjectType>
    void freeObjArray(ObjectType* pObjArray){
	    if(pObjArray != (ObjectType*)0 && pObjArray != (ObjectType*)-1){
            char* pArray = (char*)pObjArray;
            pArray -= sizeof(size_t);
            const size_t numOfObj = *(size_t*)pArray;
            for(size_t i = 0; i < numOfObj; i ++){
                (pObjArray + i)->~ObjectType();
            }
            vt_free(pArray);
	    }
    }

    /**
     * pair to: new ObjctType(...)
    **/
	template <typename ObjectType>
	void deleteObj(ObjectType* pObj){
	    if(pObj != (ObjectType*)0 && pObj != (ObjectType*)-1){
            delete pObj;
	    }
	}

	/**
	 * pair to: new ObjectType[NumOfObjects]
	**/
	template <typename ObjectType>
	void deleteArray(ObjectType* pArray){
	    if(pArray != (ObjectType*)0 && pArray != (ObjectType*)-1){
            delete [] pArray;
	    }
	}

}

#endif // VTMEM_HPP_INCLUDED
