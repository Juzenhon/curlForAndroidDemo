
#ifndef SCOPEDJSTRING_H_
#define SCOPEDJSTRING_H_

#include <jni.h>
#define ASSERT(...)
class ScopedJstring {
  public:
    ScopedJstring(JNIEnv* _env, jstring _jstr);
    ScopedJstring(JNIEnv* _env, const char* _char);

    ~ScopedJstring();

    const char* GetChar() const;
    jstring GetJstr() const;

  private:
    ScopedJstring();
    ScopedJstring(const ScopedJstring&);
    ScopedJstring& operator=(const ScopedJstring&);

  private:
    JNIEnv* _env;
    jstring _jstr;
    char* _char;
    bool jstr2char_;
};



#endif /* SCOPEDJSTRING_H_ */
