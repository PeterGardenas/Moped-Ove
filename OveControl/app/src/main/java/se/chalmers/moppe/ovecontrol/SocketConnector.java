package se.chalmers.moppe.ovecontrol;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.AsyncTask;
import java.net.InetSocketAddress;
import java.net.Socket;

/*
     * This asynchronous task is responsible for creating a socket connection
     * (typically such tasks should not be done from the UI-thread, otherwise a
     * NetworkOnMainThreadException exception may be thrown by the Android runtime tools).
     */
class SocketConnector extends AsyncTask<String, Void, String> {
    private final static int CONNECTION_TIMEOUT = 3000;

    private Socket socket = null;
    private String msg = "def message";
    private Activity activity;

    SocketConnector(Activity activity){
        this.activity = activity;
    }

    /*
     * Establish a socket connection in the background.
     */
    protected String doInBackground(String... params) {
        System.out.println("init connect");
        try {
            msg = params[0] + ":" + params[1];

				/* Close any previously used socket
				 * (for example to prevent double-clicks leading to multiple connections) */
            if (socket != null && !socket.isClosed()){
                socket.close();
            }

            socket = new Socket();
            socket.connect(new InetSocketAddress(params[0], // host ip
                            Integer.parseInt(params[1])), 	// port
                    CONNECTION_TIMEOUT);
        } catch (IllegalArgumentException e) {
            msg = "Invalid port value (" + params[1] + "), type an integer between 0 and 65535";
        } catch (Exception e) {
            msg = e.getMessage();
        }

        return null;
    }

    /*
     * Once the background operation is finished, pass the socket reference to the
     * Main class and exit from this view. If something went wrong, notify the user.
     */
    @Override
    protected void onPostExecute(String result) {
        if (socket != null && socket.isConnected()) {
            SocketHandler.init(socket);
            activity.finish();
        }
        else {
            new AlertDialog.Builder(activity)
                    .setTitle("notification")
                    .setMessage(msg)
                    .setPositiveButton("ok", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                        }
                    })
                    .show();
        }
    }
}