package com.ralph.demo;

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
