package se.chalmers.moppe.ovecontrol;

import android.widget.SeekBar;

/**
 * Created by Erik on 2017-09-24.
 */

public class ControlSeekBarListener implements SeekBar.OnSeekBarChangeListener {

    private boolean vertical;
    private int v = 0;
    private int h = 0;

    ControlSeekBarListener(boolean vertical){
        this.vertical = vertical;
    }
    ControlSeekBarListener(){
        this.vertical = false;
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    //Return control to middle after release, doesn't work for the vertical seekBar for some reason
    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        seekBar.setProgress(seekBar.getMax() / 2);
        newPos(seekBar.getProgress());
    }
    @Override

    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
        newPos(i);
    }

    private void newPos(int i) {
        if (vertical){
            v = i - 100;
        } else {
            h = i - 100;
        }
        SocketHandler.send(getMessage());
    }

    private String getMessage() {
        return "V" + intToString(v) + "H" + intToString(h);
    }


    //Makes all numbers 3 characters long
    private String intToString(int x) {
        String padding = "0";
        if (x < 0) {
            padding = "-";
        }

        x = Math.abs(x);
        if (x < 100) {
            padding += "0";

            if (x < 10) {
                padding += "0";
            }
        }

        return padding + x;
    }
}
