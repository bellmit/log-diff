#ifndef VTDIFFJNIUTIL_HPP_INCLUDED
#define VTDIFFJNIUTIL_HPP_INCLUDED

/**  2015.07 Dehua Tan **/

#include <jni.h>

namespace vtmergejni{

    class ScopeStringUTFCharsReleaser{
        public:
            inline ScopeStringUTFCharsReleaser(JNIEnv* pEnv, jstring& jstr, const char*& cstr): pEnv(pEnv), jstr(jstr), cstr(cstr){
            }
            inline ~ScopeStringUTFCharsReleaser(){
                if(pEnv != (JNIEnv*)NULL && cstr != (char*)NULL && jstr != (jstring)NULL)
                    pEnv->ReleaseStringUTFChars(jstr, cstr);
            }
        private:
            JNIEnv* pEnv;
            jstring& jstr;
            const char*& cstr;
    };

    template <typename LocalRefType>
    class ScopeLocalRefDeleter{
        public:
            inline ScopeLocalRefDeleter(JNIEnv* pEnv, LocalRefType& localRef): pEnv(pEnv), localRef(localRef){
            }
            inline ~ScopeLocalRefDeleter(){
                if(pEnv != NULL && localRef != (LocalRefType)NULL){
                    pEnv->DeleteLocalRef(localRef);
                }
            }
        private:
            JNIEnv* pEnv;
            LocalRefType& localRef;
    };
}

#endif // VTDIFFJNIUTIL_HPP_INCLUDED
