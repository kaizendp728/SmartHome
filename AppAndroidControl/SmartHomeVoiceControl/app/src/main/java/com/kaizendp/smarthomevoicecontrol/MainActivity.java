package com.kaizendp.smarthomevoicecontrol;

import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.speech.RecognizerIntent;
import android.speech.tts.TextToSpeech;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.View;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;

import org.json.JSONArray;

import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Locale;

public class MainActivity extends AppCompatActivity {
    private TextView txtSpeechInput;
    private ImageButton btnSpeak;
    private final int REQ_CODE_SPEECH_INPUT = 100;
    TextToSpeech textToSpeech;
    private Socket mSocket;
    {
        try {
            mSocket = IO.socket("http://192.168.1.7:3000/client");
        } catch (URISyntaxException e) {}
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mSocket.connect();
        txtSpeechInput = (TextView) findViewById(R.id.txtSpeechInput);
        btnSpeak = (ImageButton) findViewById(R.id.btnSpeak);
        btnSpeak.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                promptSpeechInput();
            }
        });

    }
    private void promptSpeechInput() {
        Intent intent = new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL, RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE, Locale.getDefault());
        intent.putExtra(RecognizerIntent.EXTRA_PROMPT, getString(R.string.speech_prompt));
        try {
            startActivityForResult(intent, REQ_CODE_SPEECH_INPUT);
        } catch (ActivityNotFoundException a) {
            Toast.makeText(getApplicationContext(), getString(R.string.speech_not_supported), Toast.LENGTH_SHORT).show();
        }
    }
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case REQ_CODE_SPEECH_INPUT: {
                if (resultCode == RESULT_OK && null != data) {
                    ArrayList<String> result = data.getStringArrayListExtra(RecognizerIntent.EXTRA_RESULTS);
                    String noidungInput = result.get(0);
                    String noidungOutput = "";
                    int led = 0;
                    boolean ledStt = false;
                    if(noidungInput.equals(noidungInput))
                        switch (noidungInput){
                            case "em ơi": noidungOutput = "dạ em nghe"; break;
                            case "bật đèn phòng khách": noidungOutput = "em đã bật đèn phòng khách"; led = 1; ledStt = true; break;
                            case "bật đèn phòng ngủ 1": noidungOutput = "em đã bật đèn phòng ngủ 1"; led = 2; ledStt = true; break;
                            case "bật đèn phòng ngủ 2": noidungOutput = "em đã bật đèn phòng ngủ 2"; led = 3; ledStt = true; break;
                            case "bật đèn bếp": noidungOutput = "em đã bật đèn bếp"; led = 4; ledStt = true; break;
                            case "Tắt đèn phòng khách": noidungOutput = "em đã tắt đèn phòng khách"; led = 1; ledStt = false; break;
                            case "Tắt đèn phòng ngủ 1": noidungOutput = "em đã tắt đèn phòng ngủ 1"; led = 2; ledStt = false; break;
                            case "Tắt đèn phòng ngủ 2": noidungOutput = "em đã tắt đèn phòng ngủ 2"; led = 3; ledStt = false; break;
                            case "tắt đèn bếp": noidungOutput = "em đã tắt đèn bếp"; led = 4; ledStt = false; break;
                            case "đi ngủ thôi": noidungOutput = "chúc anh ngủ ngon"; break;
                            case "Chúc em ngủ ngon": noidungOutput = "cảm ơn anh"; break;
                        }
                    JSONArray jsonArray = new JSONArray();
                    jsonArray.put(led);
                    jsonArray.put(ledStt);
                    final String noidungSpeak = noidungOutput;
                    mSocket.emit("send", jsonArray);
                    txtSpeechInput.setText(noidungInput);
                    textToSpeak(noidungSpeak);
                }break;
            }
        }
    }
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    private void textToSpeak(final String noidung){
        textToSpeech = new TextToSpeech(MainActivity.this, new TextToSpeech.OnInitListener() {
            @Override
            public void onInit(int i) {
                if(i != TextToSpeech.ERROR){
                    textToSpeech.setLanguage(Locale.getDefault());
                    textToSpeech.speak(noidung, TextToSpeech.QUEUE_FLUSH, null);
                }
            }
        });
    }
}
