package com.ralph.demo;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;

public class MainActivity extends Activity {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d("ralph","before requst");
        NetWork.request("http://www.sina.com.cn");

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                int x = 100,y = 0;
                NetWork.testDivZero(x,y);
                Log.d("ralph","add" + y);
            }
        },5000);
        Intent it = new Intent(this,PushService.class);
        startService(it);
    }
}
