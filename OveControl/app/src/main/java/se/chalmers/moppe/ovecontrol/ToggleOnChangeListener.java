package se.chalmers.moppe.ovecontrol;

import android.widget.CompoundButton;
import android.widget.ToggleButton;

/**
 * Created by Erik on 2017-09-29.
 */

public class ToggleOnChangeListener implements ToggleButton.OnCheckedChangeListener {
    String tag;
    ToggleOnChangeListener(String tag){
        this.tag = tag;
    }

    //Sends message of type "PT", "PF" for platoon mode or "AT" AT" for ACC on off
    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
        String message = tag + booleanToChar(b);
        new JavaServerMessage().execute(message);
    }

    private char booleanToChar(boolean b){
        return b ? 'T' : 'F';
    }
}
