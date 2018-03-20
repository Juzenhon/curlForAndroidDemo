package com.ralph.demo;

/**
 * Description :
 * Created by zhuxinhong on 2018/2/7.
 * Job number：135198
 * Phone ：13520295328
 * Email：zhuxinhong@syswin.com
 * Person in charge : zhuxinhong
 * Leader：zhuxinhong
 */
public class NetWork {

    static {
        try {
            System.loadLibrary("networkLib");
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public static native void request(String url);


    public static native int testDivZero(int x,int y);



}
