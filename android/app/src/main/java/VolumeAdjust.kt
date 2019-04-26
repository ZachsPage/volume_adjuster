package com.example.zach.pc_volume

import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button


class VolumeAdjustActivity : AppCompatActivity() {

     /** Lifecycle event handlers */
    override fun onCreate(savedInstanceState: Bundle?) {
         super.onCreate(savedInstanceState)
         setContentView(R.layout.activity_volume_adjust)

         val btnValUp = findViewById<Button>(R.id.btn_vol_up)
         btnValUp.setOnClickListener( View.OnClickListener {
             btnValUp.text = "You just clicked me"
         })
    }

    /**
    override fun onResume() {
        super.onResume()
    }*/

    /**
    override fun onPause() {
        super.onPause()
    }*/

}