package se.chalmers.moppe.ovecontrol;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

/**
 * A lot of code is from original WirelessIno app
 * @author Erik Magnusson
 */

abstract class SocketHandler{

    private static PrintWriter out = null;
    private static Socket socket = null;
    private static List<ObserverStatic> observerList = new ArrayList();


    /*
	 * Initialize the output stream for the socket.
	 */
    public static void init(Socket socket) {
        SocketHandler.socket = socket;
        try {
            out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(
                    socket.getOutputStream(), Charset.forName("UTF-8"))), true);
        } catch (IOException e) {
            e.printStackTrace();
        }
        notifyObservers();
    }
    /*
	 * Send a message through the socket.
	 */
    public static void send(final Object message) {
        if (socket != null){
            new Thread(new Runnable() {
                @Override
                public void run() {
                    out.println(message);
                }
            }).start();
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
        notifyObservers();
    }

    public static boolean isConnected() {
        return socket != null && socket.isConnected();
    }

    public static void addObserver(ObserverStatic o){
        observerList.add(o);
    }

    private static void notifyObservers(){
        for (ObserverStatic observer : observerList){
            observer.update(isConnected());
        }
    }
}
