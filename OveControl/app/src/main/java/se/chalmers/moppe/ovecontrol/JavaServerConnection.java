package se.chalmers.moppe.ovecontrol;

import java.io.DataOutputStream;
import java.io.IOException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

/**
 * Created by Erik on 2017-10-11.
 */

public class JavaServerConnection {
    private static URL obj;

    public static void connect(String address, String port){
        String url = "http://" + address + ":" + port + "/app";
        try {
            obj = new URL(url);
        } catch (MalformedURLException e){
            e.printStackTrace();
        }
    }

    public static void disconnect(){
        obj = null;
    }


    public static void send(String message){
        if (obj == null){
            System.out.println("No java server connection");

        }else{
            try {
                HttpURLConnection con = (HttpURLConnection) obj.openConnection();

                con.setRequestMethod("POST");

                // Send post request
                con.setDoOutput(true);
                DataOutputStream wr = new DataOutputStream(con.getOutputStream());
                wr.write(message.getBytes());
                wr.flush();
                wr.close();
                System.out.println("Sent: " + message);

                con.getResponseCode();

            }catch (IOException e){
                e.printStackTrace();
            }
        }
    }
}
