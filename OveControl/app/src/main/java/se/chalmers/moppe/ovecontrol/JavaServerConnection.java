package se.chalmers.moppe.ovecontrol;

import android.os.AsyncTask;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * Created by Erik on 2017-10-11.
 */

public class JavaServerConnection extends AsyncTask <String, Void, Void> {
    private URL obj;


    public void connect(String address, String port){
        String url = "http://" + address + ":" + port + "/app";
        try {
            obj = new URL(url);
        } catch (MalformedURLException e){
            e.printStackTrace();
        }
    }

    public void disconnect(){
        obj = null;
    }


    @Override
    protected Void doInBackground(String... strings) {
        connect(strings[0], strings[1]);
        return null;
    }
}
