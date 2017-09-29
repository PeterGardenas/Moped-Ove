package se.chalmers.moppe.ovecontrol;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageSwitcher;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.VerticalSeekBar;
import android.widget.ViewSwitcher;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.Observable;
import java.util.Observer;

public class ControlActivity extends AppCompatActivity implements ObserverStatic{
    private Menu menu = null;

    private SeekBar steerSeekBar;
    private VerticalSeekBar speedSeekBar;
    //private ImageSwitcher connectedImage;
    private ImageView connectedImage;
    private TextView connectedText;

    private static final int DISCONNECT_INDEX = 0;	// Menu bar: disconnect
    private static final int CONFIG_INDEX = 1	;	// Menu bar: WiFi configuration


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_control);

        steerSeekBar = (SeekBar) findViewById(R.id.steerSeekBar);
        speedSeekBar = (VerticalSeekBar) findViewById(R.id.speedSeekBar);
        connectedImage = (ImageView) findViewById(R.id.connectedImage);
        connectedImage.setImageResource(R.drawable.if_circle_orange_10281);
        connectedText = (TextView) findViewById(R.id.connectedText);
        connectedText.setText("Disconnected");
        steerSeekBar.setOnSeekBarChangeListener(new ControlSeekBarListener(false));
        speedSeekBar.setOnSeekBarChangeListener(new ControlSeekBarListener(true));
        steerSeekBar.setMax(200);
        speedSeekBar.setMax(200);
        steerSeekBar.setProgress(100);
        speedSeekBar.setProgress(100);
        SocketHandler.addObserver(this);
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
            SocketHandler.disconnect();
            menu.getItem(DISCONNECT_INDEX).setVisible(false); // Hide the disconnect option
            //TODO uncomment
            //view.invalidate(); // Repaint (to show "not connected" in the main view)
        }
        else if (item.getItemId() == CONFIG_INDEX) {
            Intent i = new Intent(ControlActivity.this, SocketConnector.class);
            startActivity(i);
        }

        return super.onOptionsItemSelected(item);
    }

    /*
	 * Checks if a socket connection has been established and updates
	 * the visibility of the "disconnect" menu option accordingly.
	 */
    private void updateMenuVisibility() {
        if (menu != null) {
            if (SocketHandler.isConnected())
                menu.getItem(DISCONNECT_INDEX).setVisible(true);
            else
               menu.getItem(DISCONNECT_INDEX).setVisible(false);
        }
    }

    public void updateConnectedImage(Boolean arg){
        if (arg){
            connectedImage.setImageResource(R.drawable.if_circle_green_10280);
            connectedText.setText("Connected");

        }
        else{
            connectedImage.setImageResource(R.drawable.if_circle_orange_10281);
            connectedText.setText("Disconnected");
        }
    }

    @Override
    public void update(Boolean arg) {
        updateConnectedImage(arg);
    }
}
