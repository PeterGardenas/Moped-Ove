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
    //En funktion som raknar ut ultimata distance, utbyte mot perfdist konstanten.
    public void doFunction(){
        int oldDist, minPerfDist, perfDist, dist;
        int[] speedValues = new int[]{0, 7, 11, 15, 19, 23, 27, 37, 41, 45, 49, 53, 57, 73, 77, 85, 89, 93, 97, 100};
       	//int [] speedValues = new int[]{0, 1, 3, 5, 7, 10, 12, 17, 21, 23, 25};
	perfDist = 100;
        minPerfDist = 70;

        oldDist = (int) sensor.getDistance();
        try {
            can.sendSteering((byte) 0);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true) {
            try{
                dist = (int) sensor.getDistance();
                if (shouldBreak(dist, oldDist)) {
                    crucialBreak(dist, oldDist);
                }
                checkPlatoon(dist, perfDist, minPerfDist, speedValues);
                oldDist = dist;
                Thread.sleep(25);

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
   public void crucialBreak(int dist, int oldDist) {
        System.out.println("ACTIVATE CRUCIAL BREAK");
        try {
            while (dist < 50 && (i>3)) {
                //VM.println("Distance right now: " + dist);
                can.sendMotorSpeed((byte) -100);
                Thread.sleep(25);

                oldDist = dist;
                dist = (int) sensor.getDistance();
            }
		while (dist < 30){
		can.sendMotorSpeed((byte) -10);
		Thread.sleep(25);
		oldDist = dist;
		dist = (int) sensor.getDistance();
}
            //VM.println("Deactivate crucial break ");
            i = 0;
        } catch (InterruptedException ie) {
            ie.printStackTrace();
        }
    }

    public boolean shouldBreak(int dist, int oldDist){
        if (dist < 30) {
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
       try {
           if (dist < (perfDist + 10) && dist > (perfDist - 10)) {
          //     System.out.println("perf dist");
           } else if (dist < minDist) {
               can.sendMotorSpeed((byte) 0);
               if (i > 0) {
                   i--;
               }
	//	System.out.println("not perf dist");
           } else if (dist > minDist && dist < perfDist) {
               if (i > 0) {
                   i--;
               }
               can.sendMotorSpeed((byte) speedValues[i]);
               //VM.println("Decrease speedValue ");
               //VM.println("Distance: " + dist);
               // VM.println("Speed: " + speedValues[i]);
           } else if (dist > perfDist) {
               if (i < speedValues.length - 12) {
                   i++;
               }
               can.sendMotorSpeed((byte) speedValues[i]);
               //VM.println("Increase speedValue ");
               //VM.println("Distance: " + dist);
               //VM.println("Speed: " + speedValues[i]);
           }
       } catch(InterruptedException ie) {
           ie.printStackTrace();

       }
    }
}
