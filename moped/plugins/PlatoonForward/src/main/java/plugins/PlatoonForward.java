package plugins;

import java.io.IOException;
import java.util.Random;

import com.sun.squawk.VM;
import sics.port.PluginPPort;
import sics.port.PluginRPort;
import sics.plugin.PlugInComponent;

public class PlatoonForward extends PlugInComponent {
    private PluginPPort ws, ll, st; // ws = write speed, ll = light LED, st= steering
    private PluginRPort rfws, ab ; // rfws = read front wheel speed, rd = read distance from forwardlooking sensor

    public PlatoonForward() {}

    public PlatoonForward(String[] args) {
        super(args);
    }

    public static void main(String[] args) {
        PlatoonForward plugin = new PlatoonForward(args);
        plugin.run();
    }

    public void init() {
        ws = new PluginPPort(this, "ws");
        ll = new PluginPPort(this, "ll");
        st = new PluginPPort(this, "st");

        rfws = new PluginRPort(this, "rfws");
        ab = new PluginRPort(this, "ab");

        VM.println("init");

    }
    /**
     * Calls some methods for platooning to work perfectly, see more on other comments
     * @throws InterruptedException
     */

    public int readDist(int y) {
        int x = y;
    	/*Object obj = ab.receive();
    	if (obj != null) {
    	    String s = (String) obj;
    	    try {
    		x = Integer.parseInt(s);
    	    } catch (NumberFormatException e) {
    		//VM.println("format exception (" + s + ")");
    		x = Integer.parseInt(s.substring(1));
    		x = ((int) s.charAt(0)) * 1000 + x;
    	    }
    	} else {
    	    x = -1;
    	}*/
        VM.println("dist = " + x);
        return x;
    }
    int i = 0;
    //En funktion som räknar ut ultimata avståndet, utbyte mot perfdist konstanten. 
    public void doFunction() throws InterruptedException {
        int oldDist, maxDist, minDist, perfDist, dist;
        int[] speedValues = new int[]{0, 7, 11, 15, 19, 23, 27, 37, 41, 45, 49, 53, 57, 73, 77, 85, 89, 93, 97, 100};
        //Test values below
        int[] testSpeedValues = new int [] {100, 100, -1, -1, 100, 100, 0, 0, 0, 100, 100, 75, 50, 25 ,0, 0};
        int[] distValues = new int [] {100, 90, 80, 70, 60, 50, 40, 30, 5, 15, 25, 35, 45, 55, 65, 75, 85, 95};
        int[] testMaxSpeedAndBreak = new int [] {60,60,60,60,60,60,60,60,60,60,60,20,60,60,60,60,60,60,60,60,60,60,60,60,
                5, 5, 35, 35 ,35 ,35 ,35 ,35 ,35 ,35 ,35 ,35 ,35 ,35, 35 ,35, 35, 35, 35, 35 ,35 ,35 ,35 ,35, 35, 35};
        int[] elitesTest = new int [] {45, 45, 30, 31, 38, 48, 44, 56, 60, 10, 13, 55, 77, 88, 100, 50, 21 };
        maxDist = 50;
        minDist = 30;
        perfDist = 40;
        oldDist = readDist(elitesTest[0]);

        //VM.println("" + st);
        //st.write(0);
        //VM.println("steering= " + st);

        while (true) {
            for(int j = 0; j < elitesTest.length; j++) {
                dist = readDist(elitesTest[j]);
                if (shouldBreak(dist, oldDist)) {
                    crucialBreak(dist, oldDist);
                }
                checkPlatoon(dist, perfDist, minDist, speedValues);
                oldDist = dist;
                Thread.sleep(2000);
            }
        }

    }
    /**
     * Checks if the MOPED is approaching the MOPED/Object in front too fast and then brakes if necessary
     * @param dist is the current distance from the MOPED/Object to the MOPED in front
     * @param oldDist is the previous distance from the MOPED/Object to the MOPED in front
     */
    public void crucialBreak(int dist, int oldDist) {
        int[] testDist = new int[] {1,1,2,3,5,8,13,21,34,8};
        VM.println("ACTIVATE CRUCIAL BREAK");
        int j = 0;
        try {
            while (oldDist >= dist || dist < 10) {
                VM.println("Distance right now: " + dist);
                ws.write(-100);
                Thread.sleep(50);

                oldDist = dist;
                dist = readDist(testDist[j]);
                j++;
            }
            VM.println("Deactivate crucial break ");
            i = 0;
        } catch (InterruptedException e) {
            VM.println("**************** Interrupted.");
            return;
        }
    }

    public boolean shouldBreak(int dist, int oldDist){
        if (dist < 10) {
            return true;
        } else if (oldDist*0.6 > dist) {
            return true;
        }
        return false;
    }

    /**
     * Adjusts the distance to the MOPED in front by accelerating or decelerating
     * @param dist is the current distance from the MOPED/Object to the MOPED in front
     * @param perfDist is the perfect distance to the MOPED in front for platooning
     * @param minDist is the minimum distance to the MOPED in front for platooning
     * @param speedValues is the ideal values when settning the speed for the MOPED
     */
    public void checkPlatoon(int dist, int perfDist, int minDist, int[] speedValues) {
        if (dist < (perfDist+2) && dist > (perfDist-2)) {
            ll.write("1|1");
            VM.println("Perfect distance, keep same speed ");
            VM.println("Distance: " + dist);
            VM.println("Speed: " + speedValues[i]);
        } /*else if (dist < 10) {
			ws.write(-100);
			ll.write("1|0");
            VM.println("Break " + dist);
    		} */else if (dist < minDist) {
            ws.write(0);
            if (i > 0) {
                i--;
            }
            ll.write("1|1");
            VM.println("Too close, turn off engine and decrease speed ");
            VM.println("Distance: " + dist);
            VM.println("Speed: " + speedValues[i]);

        } else if (dist > minDist && dist < perfDist) {
            if (i > 0) {
                i--;
            }
            ws.write(speedValues[i]);
            ll.write("1|1");
            VM.println("Decrease speedValue ");
            VM.println("Distance: " + dist);
            VM.println("Speed: " + speedValues[i]);
        } else if (dist > perfDist) {
            if (i < speedValues.length-1) {
                i++;
            }
            ws.write(speedValues[i]);
            ll.write("1|1");
            VM.println("Increase speedValue ");
            VM.println("Distance: " + dist);
            VM.println("Speed: " + speedValues[i]);
        }
    }

    public void run() {
        init();

        try {
            doFunction();
        } catch (InterruptedException e) {
            VM.println("**************** Interrupted.");
            return;
        }
    }
}