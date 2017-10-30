package com.qpidnetwork.livemodule.liveshow;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;

import com.qpidnetwork.livemodule.R;
import com.qpidnetwork.livemodule.framework.widget.magicfly.PeriscopeLayout;
import com.qpidnetwork.livemodule.httprequest.RequestJni;

public class MainActivity extends Activity implements View.OnClickListener{

    private PeriscopeLayout mPeriscopeLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_live_main);
        RequestJni.SetWebSite("192.168.88.17:8081");
//        RequestJniAuthorization.Login("P580502", "123456", new OnRequestLoginCallback(){
//            public void onRequestLogin(boolean isSuccess, String errCode, String errMsg, LoginItem item){
//                if(item != null){
//                    Log.i("hunter", "LadyId : " + item.userId + "~~~ FirstName: " + item.userName);
//                }
//            }
//        });
    }


    @Override
    public void onClick(View v) {
        int i = v.getId();
    }
}