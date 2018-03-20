package com.ralph.demo;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

/**
 * Description :
 * Created by zhuxinhong on 2018/3/15.
 * Job number：135198
 * Phone ：13520295328
 * Email：zhuxinhong@syswin.com
 * Person in charge : zhuxinhong
 * Leader：zhuxinhong
 */
public class PushService extends Service {

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d("ralph","service oncreate");
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d("ralph","service onStartCommand " + startId + ",flags = " + flags);
        if(intent != null){
            Log.d("ralph","service onStartCommand intent = " + intent.toUri(Intent.URI_INTENT_SCHEME) );
        }
        return super.onStartCommand(intent, flags, startId);
    }


    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onDestroy() {
        Log.d("ralph","service onDestroy");
        super.onDestroy();
    }
}
