package se.chalmers.moppe.ovecontrol;

import android.os.AsyncTask;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.charset.Charset;

/**
 * @author Erik Magnusson
 */

class PostRequester extends AsyncTask <String, Void, Void>{
    private static URL url;

    //Creates url to be used when sending post requests
    public static void connect(String address, String port){
        String url = "http://" + address + ":" + port + "/app";
        try {
            PostRequester.url = new URL(url);
        } catch (MalformedURLException e){
            e.printStackTrace();
        }
    }

    public static void disconnect(){
        url = null;
    }

    //Sends messages to the Http server on the moped, currently sends ACC and platooning on/off
    @Override
    protected Void doInBackground(String... strings) {
        if (url == null){
            System.out.println("No java server connection");
        } else {
                try {
                    HttpURLConnection con = (HttpURLConnection) url.openConnection();

                    con.setRequestMethod("POST");
                    System.out.println(con);

                    // Send post request
                    con.setDoOutput(true);
                    DataOutputStream wr = new DataOutputStream(con.getOutputStream());
                    String message = "startM" + strings[0] + "endM";
                    wr.write(message.getBytes(Charset.forName("UTF-8")));
                    wr.flush();
                    wr.close();
                    //Potential efficiency boost
//                    con.setFixedLengthStreamingMode(strings[0].getBytes().length);

                    System.out.println("Sent: " + message);
                    con.connect();
                    con.getResponseCode();
                    con.disconnect();

                }catch (IOException e){
                    e.printStackTrace();
                }
            }
        return null;
    }
}
