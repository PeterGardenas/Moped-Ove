package se.chalmers.moppe.ovecontrol;

import android.widget.SeekBar;

/**
 * Created by Erik on 2017-09-24.
 */

public class ControlSeekBarListener implements SeekBar.OnSeekBarChangeListener {

    private boolean vertical;

    ControlSeekBarListener(boolean vertical){
        this.vertical = vertical;
    }

    @Override
    public synchronized void onProgressChanged(SeekBar seekBar, int i, boolean b) {
        String orientation = vertical ? "V" : "H";
        String message = orientation + (i - 100);
        SocketHandler.send(message);
        System.out.println(message);
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    //Return control to middle after release
    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        seekBar.setProgress(seekBar.getMax() / 2);
    }
}
