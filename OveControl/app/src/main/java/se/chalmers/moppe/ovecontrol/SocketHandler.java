package se.chalmers.moppe.ovecontrol;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

/**
 * Created by Erik on 2017-09-24.
 */

public abstract class SocketHandler {

    private static PrintWriter out = null;
    public static Socket socket = null;


    /*
	 * Initialize the output stream for the socket.
	 */
    public static void init(Socket socket) {
        SocketHandler.socket = socket;
        try {
            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(
                    socket.getOutputStream())), true);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    /*
	 * Send a message through the socket.
	 */
    public static void send(Object message) {
        if (socket != null){
            out.println(message);
        }
    }

    public static void disconnect() {
        try {
            if (socket != null) {
                socket.close();
                socket = null;
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static boolean isConnected() {
        return socket == null || !socket.isConnected();
    }
}
