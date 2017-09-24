package se.chalmers.moppe.ovecontrol;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Toolbar;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

public class ControlActivity extends AppCompatActivity {
    private View mContentView;

    private static PrintWriter out = null;
    private Menu menu = null;
    public static Socket socket = null;


    private static final int DISCONNECT_INDEX = 0;	// Menu bar: disconnect
    private static final int CONFIG_INDEX = 1	;	// Menu bar: WiFi configuration


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_control);
    }

    /*
	 * Add a disconnect option when connected to a socket.
	 */
    protected void onResume() {
		/* Disable the disconnect option if no connection has been established */
        updateMenuVisibility();
        super.onResume();
    }

    /*
	 * Add menu options
	 */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        this.menu = menu;
        System.out.println("Creating menu");

		/* Add menu bars */
        menu.add(0, DISCONNECT_INDEX, DISCONNECT_INDEX, R.string.disconnect);
        menu.add(0, CONFIG_INDEX, CONFIG_INDEX, R.string.wifiConfig);

		/* To start with, disable the disconnect option if no connection has been established */
        updateMenuVisibility();

        return super.onCreateOptionsMenu(menu);
    }

    /*
	 * Handle different menu options
	 */
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == DISCONNECT_INDEX) {
            try {
                if (socket != null) {
                    socket.close();
                    socket = null;

                    menu.getItem(DISCONNECT_INDEX).setVisible(false); // Hide the disconnect option
                    //TODO uncomment
                    //view.invalidate(); // Repaint (to show "not connected" in the main view)
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        else if (item.getItemId() == CONFIG_INDEX) {
            Intent i = new Intent(ControlActivity.this, SocketConnector.class);
            startActivity(i);
        }

        return super.onOptionsItemSelected(item);
    }

    /*
	 * Initialize the output stream for the socket.
	 */
    public static void init(Socket socket) {
        ControlActivity.socket = socket;
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
        out.println(message);
    }

    /*
	 * Checks if a socket connection has been established and updates
	 * the visibility of the "disconnect" menu option accordingly.
	 */
    private void updateMenuVisibility() {
        if (menu != null) {
            if (socket == null || !socket.isConnected())
                menu.getItem(DISCONNECT_INDEX).setVisible(false);
            else
                menu.getItem(DISCONNECT_INDEX).setVisible(true);
        }
    }
}
