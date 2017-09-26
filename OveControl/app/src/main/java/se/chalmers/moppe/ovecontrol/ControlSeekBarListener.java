package se.chalmers.moppe.ovecontrol;

import android.widget.SeekBar;

/**
 * Created by Erik on 2017-09-24.
 */

public class ControlSeekBarListener implements SeekBar.OnSeekBarChangeListener {

    private boolean vertical;
    private int count = 0;
    private int v = 0;
    private int h = 0;

    ControlSeekBarListener(boolean vertical){
        this.vertical = vertical;
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
        if (vertical){
            v = i - 100;
        } else {
            h = i - 100;
        }
        String message = "V" + intToString(v) + "H" + intToString(h);

        System.out.println(message);
        if (count > 20){
            SocketHandler.send(message);
            count = 0;
        }
        count++;

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    //Return control to middle after release
    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        seekBar.setProgress(seekBar.getMax() / 2);
    }

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
