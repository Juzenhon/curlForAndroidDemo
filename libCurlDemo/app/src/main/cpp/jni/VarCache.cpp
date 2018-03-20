#include <memory.h>
#include "VarCache.h"
#include "ScopeJEnv.h"

template<typename T>
T &getListByClass(JNIEnv *env, const jclass &clz, std::map<jclass, T> &map) {
    for (typename std::map<jclass, T>::iterator iter = map.begin(); iter != map.end(); ++iter) {
        if (env->IsSameObject(clz, (*iter).first))
            return (*iter).second;
    }

    jclass globalClz = (jclass) env->NewGlobalRef(clz);
    std::pair<typename std::map<jclass, T>::iterator, bool> retPair = map.insert(
            std::pair<jclass, T>(globalClz, T()));
    ASSERT(retPair.second);

    return retPair.first->second;
}


struct method_struct {
    std::string methodName;
    std::string signature;
    jmethodID mid;
};

struct field_struct {
    std::string fieldName;
    std::string signature;
    jfieldID fid;
};

VarCache *VarCache::_instance = NULL;

VarCache::VarCache() : _vm(NULL) {
}

VarCache::~VarCache() {
    ScopeJEnv scopeJEnv(_vm);
    JNIEnv *env = scopeJEnv.GetEnv();

    //ScopedSpinLock lock(_class_map_lock);
    std::unique_lock<std::mutex> lock(_class_map_lock);
    for (auto iter = _class_map.begin(); iter != _class_map.end(); ++iter) {
        env->DeleteGlobalRef(iter->second);
    }
}

VarCache *VarCache::Singleton() {
    if (_instance == NULL) {
        _instance = new VarCache();
    }

    return _instance;
}

void VarCache::Release() {
    if (_instance != NULL) {
        delete _instance;
        _instance = NULL;
    }
}

JavaVM *VarCache::GetJvm() {
    return _vm;
}

void VarCache::SetJvm(JavaVM *vm) {
    //ASSERT(_vm);
    _vm = vm;
}

/**
 * get the class, if the class does not exist in the cache,find and cache it
 * @param _class_path        find the class
 */
jclass VarCache::GetClass(JNIEnv *env, const char *const class_path) {
    if (env->ExceptionOccurred()) {
        return NULL;
    }

    ASSERT(env != NULL);
    ASSERT(class_path != NULL);

    std::unique_lock<std::mutex> lock(_class_map_lock);
    auto iter = _class_map.find(class_path);
    if (iter != _class_map.end()) {
        return iter->second;
    }

    jclass clz = /*newClass(env, classPath);  // */env->FindClass(class_path);

    ASSERT2(clz != NULL, "classpath:%s", class_path);

    if (clz == NULL) {
        if (env->ExceptionOccurred()) {
            env->ExceptionClear();
            char err_msg[512] = {0};
            snprintf(err_msg, sizeof(err_msg), "classPath:%s", class_path);
            env->ThrowNew(env->FindClass("java/lang/UnsatisfiedLinkError"), err_msg);
        }
        return NULL;
    }

    jclass globalClz = (jclass) env->NewGlobalRef(clz);
    env->DeleteLocalRef(clz);

    _class_map.insert(std::pair<std::string, jclass>(class_path, globalClz));

    return globalClz;
}

void VarCache::CacheClass(const char *const class_path, jclass clz) {
    ASSERT(class_path != NULL);
    ASSERT(clz != NULL);

    if (class_path == NULL || clz == NULL) {
        return;
    }

    std::unique_lock<std::mutex> lock(_class_map_lock);
    auto iter = _class_map.find(class_path);

    if (iter == _class_map.end()) {
        _class_map.insert(std::pair<std::string, jclass>(class_path, clz));
    }
}

/**
 * description: get the jmethodID, if it does not exist in the cache,find and cache it.
 * @param _clz    find the jmethodID in the class
 * @param _method_name     the name of the method
 * @param _signature        the signature of the method's parameter
 */


jmethodID VarCache::GetStaticMethodId(JNIEnv *env, jclass clz,
                                      const char *const method_name, const char *const signature) {
    if (env->ExceptionOccurred()) {
        return NULL;
    }

    ASSERT(_env != NULL);
    ASSERT(_clz != NULL);
    ASSERT(_method_name != NULL);
    ASSERT(_signature != NULL);

    if (0 == clz) {
        return NULL;
    }

#ifdef USE_JNI_METHOD_CACHE
    std::unique_lock<std::mutex> lock(_static_method_map_lock);
    std::list<method_struct> &methodStructList = getListByClass(env, clz, _static_method_map);

    for (auto iter = methodStructList.begin(); iter != methodStructList.end(); ++iter) {
        method_struct &mStruct = *iter;

        if (strcmp(mStruct.methodName.c_str(), method_name) == 0
            && strcmp(mStruct.signature.c_str(), signature) == 0) {
            return mStruct.mid;
        }
    }

#endif

    jmethodID mid = env->GetStaticMethodID(clz, method_name, signature);

    ASSERT2(mid != NULL, "NULL sig:%s, mid:%s", signature, method_name);

#ifdef USE_JNI_METHOD_CACHE

    if (mid != NULL) {
        method_struct methodStruct;
        methodStruct.methodName.assign(method_name);

        methodStruct.signature.assign(signature);
        methodStruct.mid = mid;
        methodStructList.push_back(methodStruct);
    } else
#endif
    {
        // the lib may be out of date.
        // If we GetStaticMethodID fail, throw "UnsatisfiedLinkError".
        // Then Java code catch the error, copy  the correct lib to dest path
        if (env->ExceptionOccurred()) {
            env->ExceptionClear();
            char err_msg[512] = {0};
            snprintf(err_msg, sizeof(err_msg), "method:%s, sig:%s", method_name, signature);
            env->ThrowNew(env->FindClass("java/lang/UnsatisfiedLinkError"), err_msg);
        }
    }

    return mid;
}

jmethodID VarCache::GetStaticMethodId(JNIEnv *env, const char *const class_path,
                                      const char *const method_name, const char *const signature) {
    ASSERT(env != NULL);
    ASSERT(class_path != NULL);
    ASSERT(method_name != NULL);
    ASSERT(signature != NULL);

    jclass clz = GetClass(env, class_path);
    return GetStaticMethodId(env, clz, method_name, signature);
}

jmethodID VarCache::GetMethodId(JNIEnv *env, jclass clz,
                                const char *const method_name, const char *const signature) {
    if (env->ExceptionOccurred()) {
        return NULL;
    }

    ASSERT(env != NULL);
    ASSERT(clz != NULL);
    ASSERT(method_name != NULL);
    ASSERT(signature != NULL);

    if (NULL == clz) {
        return NULL;
    }

#ifdef USE_JNI_METHOD_CACHE
    std::unique_lock<std::mutex> lock(_method_map_lock);
    std::list<method_struct> &methodStructList = getListByClass(env, clz, _method_map);

    for (auto iter = methodStructList.begin(); iter != methodStructList.end(); ++iter) {
        method_struct &mStruct = *iter;

        if (strcmp(mStruct.methodName.c_str(), method_name) == 0
            && strcmp(mStruct.signature.c_str(), signature) == 0) {
            return mStruct.mid;
        }
    }

#endif

    jmethodID mid = env->GetMethodID(clz, method_name, signature);

    ASSERT2(mid != NULL, "method:%s, sig:%s", method_name, signature);

#ifdef USE_JNI_METHOD_CACHE

    if (mid != NULL) {
        method_struct methodStruct;
        methodStruct.methodName.assign(method_name);
        methodStruct.signature.assign(signature);
        methodStruct.mid = mid;
        methodStructList.push_back(methodStruct);
    } else
#endif
    {
        if (env->ExceptionOccurred()) {
            env->ExceptionClear();
            char err_msg[512] = {0};
            snprintf(err_msg, sizeof(err_msg), "method:%s, sig:%s", method_name, signature);
            env->ThrowNew(env->FindClass("java/lang/UnsatisfiedLinkError"), err_msg);
        }
    }


    return mid;
}

jmethodID VarCache::GetMethodId(JNIEnv *env, const char *const class_path,
                                const char *const method_name, const char *const signature) {
    ASSERT(env != NULL);
    ASSERT(class_path != NULL);
    ASSERT(method_name != NULL);
    ASSERT(signature != NULL);

    jclass clz = GetClass(env, class_path);

    return GetMethodId(env, clz, method_name, signature);
}


jfieldID
VarCache::GetStaticFieldId(JNIEnv *env, const char *const class_path, const char *const name,
                           const char *const sig) {
    ASSERT(NULL != env);
    ASSERT(NULL != class_path);
    ASSERT(NULL != name);
    ASSERT(NULL != sig);

    jclass clazz = GetClass(env, class_path);

    if (NULL == clazz) {
        return NULL;
    }

    return GetStaticFieldId(env, clazz, name, sig);
}

jfieldID VarCache::GetStaticFieldId(JNIEnv *env, jclass clazz, const char *const name,
                                    const char *const sig) {
    if (env->ExceptionOccurred()) {
        return NULL;
    }

    ASSERT(NULL != env);
    ASSERT(NULL != clazz);
    ASSERT(NULL != name);
    ASSERT(NULL != sig);

    if (NULL == clazz) {
        return NULL;
    }

#ifdef USE_JNI_FIELD_CACHE
    std::unique_lock<std::mutex> lock(_field_map_lock);
    std::list<field_struct> &fieldStructList = getListByClass(env, clazz, _field_map);

    for (auto iter = fieldStructList.begin();
         iter != fieldStructList.end(); ++iter) {
        field_struct &fStruct = *iter;

        if (strcmp(fStruct.fieldName.c_str(), name) == 0
            && strcmp(fStruct.signature.c_str(), sig) == 0) {
            return fStruct.fid;
        }
    }

#endif

    jfieldID fid = env->GetStaticFieldID(clazz, name, sig);
    ASSERT2(fid != NULL, "field:%s, sig:%s", name, sig);

#ifdef USE_JNI_FIELD_CACHE

    if (NULL != fid) {
        field_struct fieldStruct;
        fieldStruct.fieldName = name;
        fieldStruct.signature = sig;
        fieldStruct.fid = fid;
        fieldStructList.push_back(fieldStruct);
    } else
#endif
    {
        if (env->ExceptionOccurred()) {
            env->ExceptionClear();
            char err_msg[512] = {0};
            snprintf(err_msg, sizeof(err_msg), "static field:%s, sig:%s", name, sig);
            env->ThrowNew(env->FindClass("java/lang/UnsatisfiedLinkError"), err_msg);
        }
    }

    return fid;
}


/**
 * description: get the jfieldID, if it does not exist in the cache,find and cache it.
 * @param clz    find the jfieldID in the class
 * @param field_name     the name of the field
 * @param signature        the signature of the filed's type
 */
jfieldID VarCache::GetFieldId(JNIEnv *env, jclass clz, const char *const field_name,
                              const char *const signature) {
    if (env->ExceptionOccurred()) {
        return NULL;
    }

    ASSERT(env != NULL);
    ASSERT(clz != NULL);
    ASSERT(field_name != NULL);
    ASSERT(signature != NULL);

    if (NULL == clz) {
        return NULL;
    }

#ifdef USE_JNI_FIELD_CACHE
    std::unique_lock<std::mutex> lock(_field_map_lock);
    std::list<field_struct> &fieldStructList = getListByClass(env, clz, _field_map);

    for (auto iter = fieldStructList.begin(); iter != fieldStructList.end(); ++iter) {
        field_struct &fStruct = *iter;

        if (strcmp(fStruct.fieldName.c_str(), field_name) == 0
            && strcmp(fStruct.signature.c_str(), signature) == 0) {
            return fStruct.fid;
        }
    }

#endif

    jfieldID fid = env->GetFieldID(clz, field_name, signature);

    ASSERT2(fid != NULL, "field:%s, sig:%s", field_name, signature);

#ifdef USE_JNI_FIELD_CACHE

    if (fid != NULL) {
        field_struct fieldStruct;
        fieldStruct.fieldName.assign(field_name);
        fieldStruct.signature.assign(signature);
        fieldStruct.fid = fid;
        fieldStructList.push_back(fieldStruct);
    } else
#endif
    {
        if (env->ExceptionOccurred()) {
            env->ExceptionClear();
            char err_msg[512] = {0};
            snprintf(err_msg, sizeof(err_msg), "field:%s, sig:%s", field_name, signature);
            env->ThrowNew(env->FindClass("java/lang/UnsatisfiedLinkError"), err_msg);
        }
    }

    return fid;
}

jfieldID VarCache::GetFieldId(JNIEnv *env, const char *const class_path,
                              const char *const field_name, const char *const signature) {
    ASSERT(_env != NULL);
    ASSERT(_class_path != NULL);
    ASSERT(_field_name != NULL);
    ASSERT(_signature != NULL);

    jclass clz = GetClass(env, class_path);

    return GetFieldId(env, clz, field_name, signature);
}


/***************************************************************************************/
#include <set>

static std::set<std::string> &__GetClassNameSet() {
    static std::set<std::string> class_name_set;
    return class_name_set;
}

bool LoadClass(JNIEnv *env) {
    ASSERT(NULL != env);
    std::set<std::string> &class_name_set = __GetClassNameSet();

    for (auto it = class_name_set.begin(); it != class_name_set.end(); ++it) {
        jclass clz = VarCache::Singleton()->GetClass(env, (*it).c_str());

        if (NULL == clz) {
            class_name_set.clear();
            return false;
        }
    }

    class_name_set.clear();
    return true;
}

bool AddClass(const char *const class_path) {
    ASSERT(_class_path != NULL);
    std::set<std::string> &classNameSet = __GetClassNameSet();
    return classNameSet.insert(std::string(class_path)).second;
}

/*********************************************************************************************/

static std::set<JniMethodInfo> &__GetStaticMethodInfoSet() {
    static std::set<JniMethodInfo> method_info_set;
    return method_info_set;
}

static std::set<JniMethodInfo> &__GetMethodInfoSet() {
    static std::set<JniMethodInfo> methodInfoSet;
    return methodInfoSet;
}

bool LoadStaticMethod(JNIEnv *env) {
    ASSERT(NULL != env);

    std::set<JniMethodInfo> &methoNameSet = __GetStaticMethodInfoSet();

    for (std::set<JniMethodInfo>::iterator iter = methoNameSet.begin();
         iter != methoNameSet.end(); ++iter) {
        jmethodID mid = VarCache::Singleton()->GetStaticMethodId(env, (*iter)._classname.c_str(),
                                                                 (*iter)._methodname.c_str(),
                                                                 (*iter)._methodsig.c_str());

        if (NULL == mid) {
            return false;
        }
    }

    return true;
}

bool LoadMethod(JNIEnv *env) {
    ASSERT(NULL != env);

    std::set<JniMethodInfo> &methodNameSet = __GetMethodInfoSet();

    for (auto iter = methodNameSet.begin(); iter != methodNameSet.end(); ++iter) {
        jmethodID mid = VarCache::Singleton()->GetMethodId(env, (*iter)._classname.c_str(),
                                                           (*iter)._methodname.c_str(),
                                                           (*iter)._methodsig.c_str());

        if (NULL == mid) {
            return false;
        }
    }

    return true;
}


bool AddStaticMethod(const char *const classname, const char *const methodname,
                     const char *const methodsig) {
    ASSERT(_classname != NULL);
    ASSERT(_methodname != NULL);
    ASSERT(_methodsig != NULL);

    std::set<JniMethodInfo> &method_name_set = __GetStaticMethodInfoSet();

    return method_name_set.insert(JniMethodInfo(classname, methodname, methodsig)).second;
}

bool
AddMethod(const char *const classname, const char *const methodname, const char *const methodsig) {
    ASSERT(_classname != NULL);
    ASSERT(_methodname != NULL);
    ASSERT(_methodsig != NULL);

    std::set<JniMethodInfo> &methodNameSet = __GetMethodInfoSet();

    return methodNameSet.insert(JniMethodInfo(classname, methodname, methodsig)).second;
}

