package com.wangxingxing.secretkeysave;

import androidx.appcompat.app.AppCompatActivity;

import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.wangxingxing.secretkeysave.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    static {
        System.loadLibrary("secretkeysave");
    }

    private ActivityMainBinding binding;

    public String text = "A you OK?";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }

    public native String stringFromJNI();

    public native void func1();

    public native void func2(String data);

    public native String func3();

    public native void func4();

    public native void func5();

    public static native String getKey();

    public native void setAntiBiBCallback(IAntiDebugCallback callback);

    public static native boolean init();

    public void func1(View view) {
        func1();
    }

    public void func2(View view) {
        func2("data");
    }

    public void func3(View view) {
        Log.i(TAG, "func3: " + func3());
    }

    public void func4(View view) {
        func4();
        binding.sampleText.setText(text);
    }

    public void func5(View view) {
        func5();
    }

    /**
     * 获取Native密钥
     * @param view
     */
    public void func6(View view) {
        init();
        binding.sampleText.setText(getKey());
        Log.i(TAG, "func5: " + getSignInfo());
        setAntiBiBCallback(new IAntiDebugCallback() {
            @Override
            public void beInjectedDebug() {
                Log.d(TAG, "beInjectedDebug: ");
            }
        });
    }

    public void callBack(int code){
        Toast.makeText(this, "native层回调  " + code, Toast.LENGTH_SHORT).show();
    }
    public void callBack(byte[] code){
        Toast.makeText(this, "native层回调  " + code, Toast.LENGTH_SHORT).show();
    }
    //
    public String callBack(String code,int code1){
        Toast.makeText(this, "native层回调  " + code, Toast.LENGTH_SHORT).show();
        return null;
    }

    public String getSignInfo() {
        try {
            PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), PackageManager.GET_SIGNATURES);
            Signature[] signs = packageInfo.signatures;
            Signature sign = signs[0];
            return sign.toCharsString();
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        return null;
    }
}