package absolut.acc;

import absolut.can.CanReader;


public class ACC implements Runnable {

    private CanReader can;
    private Sensor sensor;

    public ACC() {
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
    //int[] speedValues = new int[]{0, 7, 11, 15, 19, 23, 27, 37, 41, 45, 49, 53, 57, 73, 77, 85, 89, 93, 97, 100};
    int[] speedValues = new int[]{0, 9, 11, 15, 19};
    //int[] speedValues = new int[]{0,9,10,11,12,13,14,15,16,17,18,19,20};
    int currentSpeed;

    //En funktion som raknar ut ultimata distance, utbyte mot perfdist konstanten.
    public void doFunction(){
        int lastDistance,  currentDistance;
        //int minPerfDist, perfDist;
        //int [] speedValues = new int[]{0, 1, 3, 5, 7, 10, 12, 17, 21, 23, 25};
        //perfDist = 100;
        //minPerfDist = 70;

        lastDistance = (int) sensor.getDistance();
        try {
            can.sendSteering((byte) 0);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true) {
            //try{
                currentDistance = (int) sensor.getDistance();
                if (shouldBrake(currentDistance, lastDistance)) {
                    crucialBrake(currentDistance, lastDistance);
                }
                adaptSpeed(currentDistance);
                lastDistance = currentDistance;
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
    public void crucialBrake(int currentDistance, int lastDistance) {
        try {
            boolean brake = true;
            switch (brakeCase) {
                case 1:
                    while (brake || currentDistance + 10 < lastDistance) {
                        System.out.println("ACTIVATE CRUCIAL BRAKE");
                        currentSpeed = -100;
                        can.sendMotorSpeed((byte) currentSpeed);
                        //Thread.sleep(5);
                        lastDistance = currentDistance;
                        currentDistance = (int) sensor.getDistance();
                        i = 0;
                        brake = false;
                    }
                    break;
                case 2:
                    while ( brake || currentDistance + 5 < lastDistance) {
                        System.out.println("ACTIVATE semi-CRUCIAL BRAKE");
                        currentSpeed = -40;
                        can.sendMotorSpeed((byte) currentSpeed);
                        //Thread.sleep(5);
                        lastDistance = currentDistance;
                        currentDistance = (int) sensor.getDistance();
                        i = 0;
                        brake = false;
                    }
                    break;
                case 3:
                    while (currentDistance < 15){
                        System.out.println("Reverse!");
                        currentSpeed = -10;
                        can.sendMotorSpeed((byte) currentSpeed);
                        //Thread.sleep(5);
                        currentDistance = (int) sensor.getDistance();
                        i = 0;
                        brake = false;
                    }
                    break;
                default:
                    break;
            }

            if (!brake) {
                currentSpeed = 0;
                can.sendMotorSpeed((byte) currentSpeed);
                Thread.sleep(100);
            }
        } catch (InterruptedException ie) {
            ie.printStackTrace();
        }
    }

    public boolean shouldBrake(int dist, int oldDist){
        int safetyDistance = 10;

        if (dist < currentSpeed + safetyDistance * 2 && this.currentSpeed > 20) {
            System.out.println("If Case 1");
            brakeCase = 1;
            return true;
        } else if (dist < currentSpeed + safetyDistance && this.currentSpeed > 0 ) {
            System.out.println("If Case 2");
           brakeCase = 1;
            return true;
        } else if (dist < oldDist-dist + safetyDistance && oldDist < 150 && this.currentSpeed > 20) {
            System.out.println("If Case 3");
            brakeCase = 1;
            return true;
        } else if ( oldDist - dist > 40 && oldDist < 170) {
            System.out.println("If Case 4");
            brakeCase = 2;
            return true;
        } else if (dist < 15) {
            System.out.println("If Case 5");
            brakeCase = 3;
            return true;
        }
        return false;
    }

    /**
     * Adjusts the distance to the MOPED in front by accelerating or decelerating
     * @param currentDistance is the current distance from the MOPED/Object to the MOPED in front
     */
    public void adaptSpeed(int currentDistance) {
        try {
            if (currentDistance < (currentSpeed * 3 + 10) && currentDistance > (currentSpeed * 3 - 10)) {
                currentSpeed = speedValues[i];
                can.sendMotorSpeed((byte) currentSpeed);
            } else if (currentDistance < currentSpeed * 2) {
                currentSpeed = 0;
                can.sendMotorSpeed((byte) currentSpeed);
                if (i > 0) {
                    i--;
                }
            } else if (currentDistance > currentSpeed * 2 && currentDistance < currentSpeed * 3) {
                if (i > 0) {
                    i--;
                }
                currentSpeed = speedValues[i];
                can.sendMotorSpeed((byte) currentSpeed);
            } else if (currentDistance > currentSpeed * 3 && currentDistance > 30 ) {
                    if (i < speedValues.length - 1) {
                        i++;
                    }
                    currentSpeed = speedValues[i];
                    can.sendMotorSpeed((byte) currentSpeed);
            }
        } catch(InterruptedException ie) {
            ie.printStackTrace();

        }
    }
}
