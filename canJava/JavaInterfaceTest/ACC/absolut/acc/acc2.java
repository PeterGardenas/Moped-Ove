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
    //Bad values: 7 and 27
    int[] speedValues = new int[]{0, 7, 11, 15, 19, 23, 27, 37, 41, 45, 49, 53, 57, 73, 77, 85, 89, 93, 97, 100};
    int speed;

    //En funktion som raknar ut ultimata distance, utbyte mot perfdist konstanten.
    public void doFunction(){
        int oldDist,  dist;
        //int minPerfDist, perfDist;
        //int [] speedValues = new int[]{0, 1, 3, 5, 7, 10, 12, 17, 21, 23, 25};
        //perfDist = 100;
        //minPerfDist = 70;

        oldDist = (int) sensor.getDistance();
        try {
            speed = 0;
            can.sendSteering((byte) speed);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true) {
            //try{
                dist = (int) sensor.getDistance();
                if (shouldBrake(dist, oldDist)) {
                    crucialBrake(dist, oldDist);
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
    int brakeCase;
    public void crucialBrake(int dist, int oldDist) {
        try {
            boolean brake = true;
            switch (brakeCase) {
                case 1:
                    while (brake || dist + 10 < oldDist) {
                        System.out.println("ACTIVATE CRUCIAL BRAKE");
                        speed = -100;
                        can.sendMotorSpeed((byte) speed);
                        //Thread.sleep(5);
                        oldDist = dist;
                        dist = (int) sensor.getDistance();
                        i = 0;
                        brake = false;
                    }
                    break;
                case 2:
                    while ( brake || dist + 5 < oldDist) {
                        System.out.println("ACTIVATE semi-CRUCIAL BRAKE");
                        speed = -40;
                        can.sendMotorSpeed((byte) speed);
                        //Thread.sleep(5);
                        oldDist = dist;
                        dist = (int) sensor.getDistance();
                        i = 0;
                        brake = false;
                    }
                    break;
                case 3:
                    while (dist < 15){
                        System.out.println("Reverse!");
                        speed = -10;
                        can.sendMotorSpeed((byte) speed);
                        //Thread.sleep(5);
                        dist = (int) sensor.getDistance();
                        i = 0;
                        brake = false;
                    }
                    break;
            }

            if (!brake) {
                speed = 0;
                can.sendMotorSpeed((byte) speed);
                Thread.sleep(100);
            }
        } catch (InterruptedException ie) {
            ie.printStackTrace();
        }
    }

    public boolean shouldBrake(int dist, int oldDist){
        if (dist < speedValues[i] + 20 && speed > 20) {
            brakeCase = 1;
            return true;
        } else if (dist < speedValues[i] && speed > 0 ) {
           brakeCase = 2;
            return true;
        } else if (dist < oldDist-dist + 10 && oldDist < 150) {
            brakeCase = 1;
            return true;
        } else if ( oldDist - dist > 40 && oldDist < 170) {
            brakeCase = 2;
            return true;
        } else if (dist < 15) {
            brakeCase = 5;
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
                speed = speedValues[i];
                can.sendMotorSpeed((byte) speed);
            } else if (dist < speedValues[i] * 2) {
                speed = 0;
                can.sendMotorSpeed((byte) speed);
                if (i > 0) {
                    i--;
                }
            } else if (dist > speedValues[i] * 2 && dist < speedValues[i] * 3) {
                if (i > 0) {
                    i--;
                }
                speed = speedValues[i];
                can.sendMotorSpeed((byte) speed);
            } else if (dist > speedValues[i] * 3) {
                if (i < speedValues.length - 12) {
                    i++;
                }
                speed = speedValues[i];
                can.sendMotorSpeed((byte) speed);
            }
        } catch(InterruptedException ie) {
            ie.printStackTrace();

        }
    }
}
