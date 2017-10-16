package absolut.acc;

import java.io.IOException;
import absolut.can.CanReader;


public class acc2 implements Runnable {

    private CanReader can;
    private Sensor sensor;

    public acc2() {
        sensor = new Sensor();
    }

    @Override
    public void run() {
        init();
        doFunction();
    }

    private void init() {
        can = CanReader.getInstance();
    }


    int i = 0;
    int[] speedValues = new int[]{0, 7, 11, 15, 19, 23, 27, 37, 41, 45, 49, 53, 57, 73, 77, 85, 89, 93, 97, 100};

    //En funktion som raknar ut ultimata distance, utbyte mot perfdist konstanten.
    public void doFunction(){
        int oldDist,  dist;
        //int minPerfDist, perfDist;
        //int [] speedValues = new int[]{0, 1, 3, 5, 7, 10, 12, 17, 21, 23, 25};
        //perfDist = 100;
        //minPerfDist = 70;

        oldDist = (int) sensor.getDistance();
        try {
            can.sendSteering((byte) 0);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true) {
            //try{
                dist = (int) sensor.getDistance();
                if (shouldBreak(dist, oldDist)) {
                    crucialBreak(dist, oldDist);
                }
                checkPlatoon(dist);
                oldDist = dist;
                //Thread.sleep(25);

            //} catch(InterruptedException ie){
              //  ie.printStackTrace();
            //}
        }

    }
    /**
     * Checks if the MOPED is approaching the MOPED/Object in front too fast and then brakes if necessary
     * @param dist is the current distance from the MOPED/Object to the MOPED in front
     * @param oldDist is the previous distance from the MOPED/Object to the MOPED in front
     */
    public void crucialBreak(int dist, int oldDist) {
        try {
            while (dist < speedValues[i] && (i>3) && dist < oldDist) {
                System.out.println("ACTIVATE CRUCIAL BREAK");
                can.sendMotorSpeed((byte) -100);
                //Thread.sleep(5);
                oldDist = dist;
                dist = (int) sensor.getDistance();
            }
            while (dist < 15){
                System.out.println("Reverse!");
                can.sendMotorSpeed((byte) -10);
                //Thread.sleep(5);
                dist = (int) sensor.getDistance();
            }
            i = 0;
        } catch (InterruptedException ie) {
            ie.printStackTrace();
        }
    }

    public boolean shouldBreak(int dist, int oldDist){
        if (dist < speedValues[i]  || dist < 15 && i>3) {
            return true;
        }
        return false;
    }

    /**
     * Adjusts the distance to the MOPED in front by accelerating or decelerating
     * @param dist is the current distance from the MOPED/Object to the MOPED in front
     */
    public void checkPlatoon(int dist) {
        try {
            if (dist < (speedValues[i] * 3 + 10) && dist > (speedValues[i] * 3 - 10)) {
                can.sendMotorSpeed((byte)speedValues[i]);
            } else if (dist < speedValues[i] * 2) {
                can.sendMotorSpeed((byte) 0);
                if (i > 0) {
                    i--;
                }
            } else if (dist > speedValues[i] * 2 && dist < speedValues[i] * 3) {
                if (i > 0) {
                    i--;
                }
                can.sendMotorSpeed((byte) speedValues[i]);
            } else if (dist > speedValues[i] * 3) {
                if (i < speedValues.length - 12) {
                    i++;
                }
                can.sendMotorSpeed((byte) speedValues[i]);
            }
        } catch(InterruptedException ie) {
            ie.printStackTrace();

        }
    }
}
