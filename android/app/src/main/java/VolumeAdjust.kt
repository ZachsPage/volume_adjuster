package com.example.zach.pc_volume

import android.bluetooth.*
import android.content.Context
import android.graphics.Color
import android.support.v7.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.Toast
import android.content.Intent
import android.support.v4.app.ActivityCompat.startActivityForResult
import app.akexorcist.bluetotohspp.library.BluetoothSPP
import app.akexorcist.bluetotohspp.library.BluetoothState
import app.akexorcist.bluetotohspp.library.DeviceList
import android.app.Activity

class VolumeAdjustActivity : AppCompatActivity() {
    private val bt = BluetoothSPP(this)
    private val CMD_HDR : ByteArray = byteArrayOf(0x55, 0x54)
    private val VOL_UP_CMD : Byte = 0x01
    private val VOL_DOWN_CMD : Byte = 0x02
    private val VOL_MUTE_CMD : Byte = 0x03

    /***** Bluetooth Library Interface Implementations *****/
    inner class MyBtDataImpl : BluetoothSPP.OnDataReceivedListener {
        override fun onDataReceived(data: ByteArray, message: String) {
            Toast.makeText(applicationContext,
                "BT Received data" + data.toString(),
                    Toast.LENGTH_LONG).show()
        }
    }
    inner class MyBtConnImpl : BluetoothSPP.BluetoothConnectionListener {
        override fun onDeviceConnected(name: String, address: String) {
            Toast.makeText(applicationContext, "Connected to device",
                Toast.LENGTH_LONG).show()
        }

        override fun onDeviceDisconnected() {
            Toast.makeText(applicationContext, "Disconnected from device",
                Toast.LENGTH_LONG).show()
        }

        override fun onDeviceConnectionFailed() {
            Toast.makeText(applicationContext, "Failed connecting to device",
                Toast.LENGTH_LONG).show()
        }
    }

    /***** Activity Lifecycle Event Handlers *****/
    override fun onCreate(savedInstanceState: Bundle?) {
         // Create the layout
         super.onCreate(savedInstanceState)
        // Set Content view for R (Resource)
         setContentView(R.layout.activity_volume_adjust)

         // Set button listeners
         findViewById<Button>(R.id.cnct_bt).setOnClickListener{ onBluetoothConnectClick() }
         findViewById<Button>(R.id.btn_vol_up).setOnClickListener{ onVolUpClick() }
         findViewById<Button>(R.id.btn_vol_down).setOnClickListener{ onVolDownClick() }
         findViewById<Button>(R.id.btn_mute).setOnClickListener{ onVolMuteClick() }
         findViewById<Button>(R.id.discnct_bt).setOnClickListener{ onBluetoothDisconnectClick() }

         // Set up Bluetooth Callbacks
         bt.setOnDataReceivedListener( MyBtDataImpl() )
         bt.setBluetoothConnectionListener( MyBtConnImpl() )
    }
    override fun onResume() {
        super.onResume()
    }
    override fun onPause() {
        super.onPause()
    }

    /***** Bluetooth Connect Functions ****/
    private fun onBluetoothConnectClick()
    {
        if( ! bt.isBluetoothAvailable() ) {
            Toast.makeText(this, "Bluetooth not available...",
                Toast.LENGTH_LONG).show()
            return
        }

        if( ! bt.isBluetoothEnabled() ) {
            Toast.makeText(this, "Bluetooth not enabled... enable, then try again",
                Toast.LENGTH_LONG).show()
            startActivity( Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE) )
        } else {
            bt.setupService()
            bt.startService(BluetoothState.DEVICE_OTHER)
            val intent = Intent(applicationContext, DeviceList::class.java)
            startActivityForResult(intent, BluetoothState.REQUEST_CONNECT_DEVICE)
        }
    }
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        if (requestCode == BluetoothState.REQUEST_CONNECT_DEVICE) {
            if (resultCode == Activity.RESULT_OK && data != null) {
                Toast.makeText(
                    this, "Connecting...",
                    Toast.LENGTH_LONG
                ).show()
                bt.connect(data)
            }
        } else if (requestCode == BluetoothState.REQUEST_ENABLE_BT) {
            if (resultCode == Activity.RESULT_OK) {
                Toast.makeText(
                    this, "Enabling Bt...",
                    Toast.LENGTH_LONG
                ).show()
                bt.setupService()
                bt.startService(BluetoothState.DEVICE_OTHER)
            }
        } else { /* User pressed back */ }
    }

    /***** Simple Button Click Functions *****/
    private fun _sendBtCommand( cmd : Byte ) {
        if( bt.isServiceAvailable() ) {
            bt.send(CMD_HDR + cmd, false)
        } else {
            Toast.makeText(this, "Bluetooth service unavailable - please click `CONNECT BLUETOOTH`",
                Toast.LENGTH_LONG).show()
        }
    }
    private fun onVolUpClick() {
        _sendBtCommand( VOL_UP_CMD )
    }
    private fun onVolDownClick() {
        _sendBtCommand( VOL_DOWN_CMD )
    }
    private fun onVolMuteClick() {
        _sendBtCommand( VOL_MUTE_CMD )
    }
    private fun onBluetoothDisconnectClick() {
        bt.disconnect()
        bt.stopService()
        bt.getBluetoothAdapter().disable()
    }
}