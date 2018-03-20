#include <jni.h>
#include "CTNNetWork.h"
#include "jni/ScopedJstring.h"
#include "JNILog.h"
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>

class MyRequest : public HttpRequest {

public:

    MyRequest(const char *url) {
        _url = url;
    }

    /**
     * 处理response
     * @param code 响应代码
     */
    void handResponse(HttpResponse &response) {
        LOGD("--- handResponse ----- %d", response._httpCode);
    }
};


extern "C"{

#define PID_FILE "/var/run/sampled.pid"

int sampled_running() {
    FILE *pidfile = fopen(PID_FILE, "r");
    pid_t pid;
    int ret;
    if (!pidfile) {
        return 0;
    }

    ret = fscanf(pidfile, "%d", &pid);
    if (ret == EOF && ferror(pidfile) != 0) {
        LOGD("Error open pid file %s", PID_FILE);
    }

    fclose(pidfile);

    // 检测进程是否存在
    if (kill(pid, 0)) {
       LOGD("Remove a zombie pid file %s", PID_FILE);
        unlink(PID_FILE);
        return 0;
    }
    return pid;
}

pid_t sampled() {
    pid_t pid;
    struct rlimit rl;
    int fd, i;

    // 创建子进程，并退出当前父进程
    if ((pid = fork()) < 0) {
        LOGD( "sampled : fork error");
        return -1;
    }
    if (pid != 0) {
        //  父进程直接退出
        exit(0);
    }
    // 新建会话，成功返回值是会话首进程id，进程组id ，首进程id
    pid = setsid();

    if (pid < -1) {
        LOGD( "sampled : setsid error");
        return -1;
    }
    // 将工作目录切换到根目录
    if (chdir("/") < 0) {
        LOGD( "sampled : chidr error");
        return -1;
    }
    // 关闭所有打开的句柄，如果确定父进程未打开过句柄，此步可以不做
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for (i = 0; i < rl.rlim_max; i++) {
        close(i);
    }

    // 重定向输入输出错误
    fd = open("/dev/null", O_RDWR, 0);
    if (fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > 2) {
            close(fd);
        }
    }

    // 消除文件掩码
    umask(0);
    return 0;
}

int pidfile_write() {
    // 这里不用fopen直接打开文件是不想创建666权限的文件
    FILE *pidfile = NULL;
    int pidfilefd = creat(PID_FILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (pidfilefd != -1) {
        pidfile = fdopen(pidfilefd, "w");
    }

    if (!pidfile) {
        LOGD( "pidfile write : can't open pidfile:%s", PID_FILE);
        return 0;
    }
    fprintf(pidfile, "%d", getpid());
    fclose(pidfile);
    return 1;
}


JNIEXPORT void JNICALL Java_com_ralph_demo_NetWork_request(JNIEnv *env, jclass clz, jstring url) {

    ScopedJstring sc_url(env, url);
    HttpRequest *request = new MyRequest(sc_url.GetChar());
    request->enqueue();

}


JNIEXPORT jint JNICALL Java_com_ralph_demo_NetWork_testDivZero(JNIEnv *env, jclass clz, jint x,jint y) {

    pid_t pid;
    struct rlimit rl;
    int fd, i;

    // 创建子进程，并退出当前父进程
    if ((pid = fork()) < 0) {
        LOGD( "sampled : fork error");
        return -1;
    }
    if(pid != 0){
        LOGD( "sampled : fork parent");
    } else{
        LOGD( "sampled : child process");
        pid_t pid1;
        if ((pid1 = fork()) < 0) {
            LOGD( "sampled : sub process fork error");
            return -1;
        }
        if(pid1 != 0){
            LOGD( "sampled : sub process it's self");
            exit(0);
        } else{
            LOGD( "sampled : sub process's sub process");
            // 新建会话，成功返回值是会话首进程id，进程组id ，首进程id
            pid1 = setsid();

            if (pid1 < -1) {
                LOGD( "sampled :  sub process's sub process setSid failed");
                return -1;
            }
            // 将工作目录切换到根目录
            if (chdir("/") < 0) {
                LOGD( "sampled : chidr error");
                return -1;
            }


        }


    }
    return 0;
}

}


//int main() {
//    int err, signo;
//    sigset_t mask;
//    if (sampled_running() > 0) {
//        exit(0);
//    }
//    if (sampled() != 0) {
//    }
//    // 写记录锁文件
//    if (pidfile_write() <= 0) {
//        exit(0);
//    }
//    while (1) {
//        // 捕捉信号
//        err = sigwait(&mask, &signo);
//        if (err != 0) {
//            syslog(LOG_INFO, "sigwait error : %d", err);
//            exit(1);
//        }
//        switch (signo) {
//            default :
//                syslog(LOG_INFO, "unexpected signal %d \n", signo);
//                break;
//            case SIGTERM:
//                syslog(LOG_INFO, "got SIGTERM. exiting");
//                exit(0);
//        }
//    }
//}
