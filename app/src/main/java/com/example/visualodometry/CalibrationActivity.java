package com.example.visualodometry;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Timer;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class CalibrationActivity extends AppCompatActivity {

    private Button btnClose;
    private Button btnCalibrate;
    private SurfaceView surfaceView;
    private SurfaceHolder surfaceHolder;
    private TextView txt;
    SharedPreferences sharedPref;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_calibration);

        btnClose = findViewById(R.id.btnClose);
        btnClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(CalibrationActivity.this, MainActivity.class);
                startActivity(intent);
            }
        });
        sharedPref = getSharedPreferences("cameraCalibrationParams", MODE_PRIVATE);

        // save your string in SharedPreferences


        btnCalibrate = findViewById(R.id.btnCalibrate);
        btnCalibrate.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String config = nativeCalibrate();
                Log.i("main", config);
                if(config.equals("Failed")) {
                    Toast.makeText(getApplicationContext(), "Camera calibration failed", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(getApplicationContext(), "Camera calibrated successfuly", Toast.LENGTH_SHORT).show();
                    sharedPref.edit().putString("params", config).commit();
                }
            }
        });

        surfaceView = findViewById(R.id.calibCameraPreview);
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.setFormat(PixelFormat.RGB_888);

        surfaceHolder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                //TODO: function called to early when requesting permissions
                setSurface(holder.getSurface());
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

            }
        });
        nativeOnCreate();

        txt = findViewById(R.id.nimages);
    }


    private Handler mHandler = new Handler();
    private Runnable timerTask = new Runnable() {
        @Override
        public void run() {
            txt.setText(Integer.toString(getImageNumber()));
            mHandler.postDelayed(timerTask,1000);
        }
    };

    @Override
    protected void onResume() {
        super.onResume();
        nativeOnResume();
        mHandler.post(timerTask);
    }

    @Override
    protected void onPause() {
        super.onPause();
        nativeOnPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        nativeOnDestroy();
    }

    public static native void nativeOnCreate();
    public static native void nativeOnPause();
    public static native void nativeOnResume();
    public static native void nativeOnDestroy();
    public static native void setSurface(Surface surface);
    public static native int getImageNumber();
    public static native String nativeCalibrate();
}