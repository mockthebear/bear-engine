package com.tutorial.game;
import android.content.res.AssetManager;
import org.libsdl.app.SDLActivity;


import android.app.*;
import android.content.*;
import android.view.*;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.widget.AbsoluteLayout;
import android.os.*;
import android.util.Log;
import android.graphics.*;
import android.media.*;
import android.hardware.*;

public class HelloSDL2Activity extends SDLActivity
{

    private static Vibrator vibrator = null;


    //private static native void load(AssetManager mgr);
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        AssetManager mgr = getResources().getAssets();
        vibrator = (Vibrator)this.getApplicationContext().getSystemService(Context.VIBRATOR_SERVICE);
        //load(mgr);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);




    }


}

