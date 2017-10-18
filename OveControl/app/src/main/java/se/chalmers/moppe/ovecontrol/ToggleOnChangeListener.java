package se.chalmers.moppe.ovecontrol;

import android.widget.CompoundButton;
import android.widget.ToggleButton;

/**
 * @author Erik Magnusson
 */

class ToggleOnChangeListener implements ToggleButton.OnCheckedChangeListener {
    private String tag;
    ToggleOnChangeListener(String tag){
        this.tag = tag;
    }

    //Sends message of type "PT", "PF" for platoon mode or "AT" AT" for ACC on off
    @Override
    public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
        String message = tag + booleanToChar(b);
        new PostRequester().execute(message);
    }

    private char booleanToChar(boolean b){
        return b ? 'T' : 'F';
    }
}
