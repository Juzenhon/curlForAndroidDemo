
#ifndef _VAR_CACHE_H_
#define _VAR_CACHE_H_

#include <jni.h>
#include <map>
#include <list>
#include <string>

#include <mutex>
#define ASSERT(p)
#define ASSERT2(...)

#define USE_JNI_METHOD_CACHE
#define USE_JNI_FIELD_CACHE

struct method_struct;
struct field_struct;

struct JniMethodInfo {
    JniMethodInfo(const std::string& classname, const std::string& methodname, const std::string& methodsig)
        : _classname(classname), _methodname(methodname), _methodsig(methodsig) {}

    bool operator <(const JniMethodInfo& _info) const {
        if (_classname < _info._classname) {
            return true;
        }

        if (_classname == _info._classname && _methodname < _info._methodname) {
            return true;
        }

        if (_classname == _info._classname
                && _methodname == _info._methodname
                && _methodsig < _info._methodsig) {
            return true;
        }

        return false;
    }

    std::string _classname;
    std::string _methodname;
    std::string _methodsig;
};

class VarCache {
  public:
    static VarCache* Singleton();
    static void Release();
    ~VarCache();

    JavaVM* GetJvm();
    void SetJvm(JavaVM* vm);

    jclass GetClass(JNIEnv*, const char* const);
    void CacheClass(const char* const, jclass);

    jmethodID GetStaticMethodId(JNIEnv*, const char* const, const char* const, const char* const);
    jmethodID GetStaticMethodId(JNIEnv*, jclass, const char* const, const char* const);

    jmethodID GetMethodId(JNIEnv*, const char* const, const char* const, const char* const);
    jmethodID GetMethodId(JNIEnv*, jclass, const char* const, const char* const);

    jfieldID GetStaticFieldId(JNIEnv*, const char* const, const char* const, const char* const);
    jfieldID GetStaticFieldId(JNIEnv*, jclass, const char* const, const char* const);

    jfieldID GetFieldId(JNIEnv*, const char* const, const char* const, const char* const);
    jfieldID GetFieldId(JNIEnv*, jclass, const char* const, const char* const);

  private:
    VarCache();

  private:
    static VarCache* _instance;

    JavaVM* _vm;

    std::map<std::string, jclass> _class_map;
    std::map<jclass, std::list<method_struct> > _static_method_map;
    std::map<jclass, std::list<method_struct> > _method_map;
    std::map<jclass, std::list<field_struct> > _field_map;

    std::mutex _class_map_lock;
    std::mutex _static_method_map_lock;
    std::mutex _method_map_lock;
    std::mutex _field_map_lock;
};

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

bool LoadClass(JNIEnv* env);
bool AddClass(const char* const classPath);

#define DEFINE_FIND_CLASS(classname, classpath) \
    VARIABLE_IS_NOT_USED static bool b_##classname = AddClass(classpath);\
    VARIABLE_IS_NOT_USED static const char* classname = classpath;


bool LoadStaticMethod(JNIEnv* env);
bool AddStaticMethod(const char* const _classname, const char* const _methodname, const char* const _methodsig);

bool LoadMethod(JNIEnv* env);
bool AddMethod(const char* const _classname, const char* const _methodname, const char* const _methodsig);

#define DEFINE_FIND_STATIC_METHOD(methodid, classname, methodname, methodsig) \
    VARIABLE_IS_NOT_USED static bool b_static_##methodid = AddStaticMethod(classname, methodname, methodsig);\
    VARIABLE_IS_NOT_USED const static JniMethodInfo methodid = JniMethodInfo(classname, methodname, methodsig);

#define DEFINE_FIND_METHOD(methodid, classname, methodname, methodsig) \
    VARIABLE_IS_NOT_USED static bool b_##methodid = AddMethod(classname, methodname, methodsig);\
    VARIABLE_IS_NOT_USED const static JniMethodInfo methodid = JniMethodInfo(classname, methodname, methodsig);

#endif

