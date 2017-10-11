package absolut.acc;

import java.io.IOException;
import absolut.can.CanReader;


public class acc2 implements Runnable {

    private CanReader can;

    public acc2() {}

    @Override
    public void run() {
        init();
        doFunction();
    }

    private void init() {
        can = CanReader.getInstance();
    }


    //int i = 0;
    //En funktion som raknar ut ultimata distance, utbyte mot perfdist konstanten.
    public void doFunction(){
        //int oldDist, , minPerfDist, perfDist, dist;
        int[] speedValues = new int[]{0, 7, 11, 15, 19, 23, 27, 37, 41, 45, 49, 53, 57, 73, 77, 85, 89, 93, 97, 100};
        //perfDist = 40;
        //minPerfDist = 30;

        //oldDist = //Read distance value*/
        try {
            can.sendSteering((byte) -40);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true) {
            try{
               /* dist = // Read distance value
                if (shouldBreak(dist, oldDist)) {
                    crucialBreak(dist, oldDist);
                }
                checkPlatoon(dist, perfDist, minPerfDist, speedValues);
                oldDist = dist;
                Thread.sleep(2000);*/
               for(int i = 0; i < speedValues.length-1; i++) {
                   can.sendMotorSpeed((byte) speedValues[i]);
               }
            } catch(InterruptedException ie){
                ie.printStackTrace();
            }
        }

    }
    /**
     * Checks if the MOPED is approaching the MOPED/Object in front too fast and then brakes if necessary
     * @param dist is the current distance from the MOPED/Object to the MOPED in front
     * @param oldDist is the previous distance from the MOPED/Object to the MOPED in front
     */
   /* public void crucialBreak(int dist, int oldDist) {
        int[] testDist = new int[] {1,1,2,3,5,8,13,21,34,8};
        //VM.println("ACTIVATE CRUCIAL BREAK");
        int j = 0;
        try {
            while (oldDist >= dist || dist < 10) {
                //VM.println("Distance right now: " + dist);
                //ws.write(-100);
                can.sendMotorSpeed((byte) newControlSignal);
                Thread.sleep(50);

                oldDist = dist;
                dist = readDist(testDist[j]);
                j++;
            }
            //VM.println("Deactivate crucial break ");
            i = 0;
        } catch (InterruptedException e) {
            ie.printStackTrace();
        }
    }

    public boolean shouldBreak(int dist, int oldDist){
        if (dist < 10) {
            return true;
        } else if (oldDist*0.6 > dist) {
            return true;
        }
        return false;
    }*/

    /**
     * Adjusts the distance to the MOPED in front by accelerating or decelerating
     * @param dist is the current distance from the MOPED/Object to the MOPED in front
     * @param perfDist is the perfect distance to the MOPED in front for platooning
     * @param minDist is the minimum distance to the MOPED in front for platooning
     * @param speedValues is the ideal values when settning the speed for the MOPED
     */
   /* public void checkPlatoon(int dist, int perfDist, int minDist, int[] speedValues) {
        if (dist < (perfDist+2) && dist > (perfDist-2)) {
           // ll.write("1|1");
            System.out.println("perf dist");
        }else if (dist < minDist) {
            can.sendMotorSpeed((byte) 0);
            if (i > 0) {
                i--;
            }

        } else if (dist > minDist && dist < perfDist) {
            if (i > 0) {
                i--;
            }
            can.sendMotorSpeed((byte) speedValues[i]);
            //VM.println("Decrease speedValue ");
            //VM.println("Distance: " + dist);
           // VM.println("Speed: " + speedValues[i]);
        } else if (dist > perfDist) {
            if (i < speedValues.length-1) {
                i++;
            }
            can.sendMotorSpeed((byte) speedValues[i]);
            //VM.println("Increase speedValue ");
            //VM.println("Distance: " + dist);
            //VM.println("Speed: " + speedValues[i]);
        }
    }*/
}