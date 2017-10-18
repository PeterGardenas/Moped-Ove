package absolut.acc;

import absolut.can.CanReader;

/**
 * @author Sara Kitzing
 * @author Julia Ortheden
 * @author Johan Wennerbeck
 *
 * This class handles the distance to an object infront of the sensor. Increase the speed to come closer and decrease
 * the speed to increase the distance.
 */

public class ACC implements Runnable {

    private CanReader can;
    private Sensor sensor;
    int i = 0;
    //Values if the vehicle should travel faster
    //int[] speedValues = new int[]{0, 9, 11, 15, 19, 27, 37, 41, 45, 49, 53, 57, 73, 77, 85, 89, 93, 97, 100};
    int[] speedValues = new int[]{0, 9, 11, 15, 19}; //Safe values used right now
    int currentSpeed;
    private boolean accEnabled = true;

    public ACC() {
        sensor = new Sensor();
    }

    /**
     * Runs the necessary methods to initialize and run the class
     */
    @Override
    public void run() {
        System.out.println("Starting ACC");
        init();
        doFunction();
    }

    /**
     * Initialize CanReader
     */
    private void init() {
        can = CanReader.getInstance();
    }


    /**
     * Checks the sensor value and calls an appropriate method
     */
    public void doFunction(){
        int lastDistance,  currentDistance;

        lastDistance = (int) sensor.getDistance();
        try {
            can.sendSteering((byte) 0);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true){
            System.out.println("loooop");
            if (accEnabled) {
                System.out.println("inner looosp");
                currentDistance = (int) sensor.getDistance();
                if (shouldBrake(currentDistance, lastDistance)) {
                    crucialBrake(currentDistance, lastDistance);
                }
                adaptSpeed(currentDistance);
                lastDistance = currentDistance;
            } else {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

    }
    /**
     * Checks if the vehicle is approaching the object in front too fast and then brakes if necessary
     * @param currentDistance is the current distance from the vehicle to the object in front
     * @param lastDistance is the previous distance from the vehicle to the object in front
     */
    public void crucialBrake(int currentDistance, int lastDistance) {
        try {
            boolean brake = true;
            while (brake || currentDistance + 10 < lastDistance) {
                System.out.println("ACTIVATE CRUCIAL BRAKE");
                currentSpeed = -100;
                can.sendMotorSpeed((byte) currentSpeed);
                lastDistance = currentDistance;
                currentDistance = (int) sensor.getDistance();
                i = 0;
                brake = false;
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

    /**
     * Decides if and how the vehicle should brake based on speed right now and the values from the sensor
     * @param currentDistance is the current distance from the vehicle to the object in front
     * @param lastDistance is the previous distance from the vehicle to the object in front
     * @return true if the vehicle should brake and false if not
     */
    public boolean shouldBrake(int currentDistance, int lastDistance){
        int safetyDistance = 10;
        
        //if (currentDistance < currentSpeed * 2 + safetyDistance && this.currentSpeed > 0) {
        if (currentDistance < 50 && currentSpeed > 0){
            return true;
        } else if (currentDistance < lastDistance-currentDistance + safetyDistance * 2 && lastDistance < 200 && this.currentSpeed > 0) {
            return true;
        } else if ( lastDistance - currentDistance > 30 && lastDistance < 170 && currentSpeed > 0) {
            return true;
        }
        return false;
    }

    /**
     * Adjusts the distance to the object in front by accelerating or decelerating
     * @param currentDistance is the current distance from the vehicle to the object in front
     */
    public void adaptSpeed(int currentDistance) {
        try {
            //if (currentDistance < (currentSpeed * 4 + 10) && currentDistance > (currentSpeed * 4 - 10)) {
            if (currentDistance < 90 && currentDistance > 80) {
                currentSpeed = speedValues[i];
                can.sendMotorSpeed((byte) currentSpeed);
            } //else if (currentDistance < currentSpeed * 3) {
            else if (currentDistance < 60) {
                currentSpeed = 0;
                can.sendMotorSpeed((byte) currentSpeed);
                if (i > 0) {
                    i--;
                }
            } //else if (currentDistance > currentSpeed * 3 && currentDistance < currentSpeed * 4) {
            else if (currentDistance > 60 && currentDistance < 80) {
                if (i > 0) {
                    i--;
                }
                currentSpeed = speedValues[i];
                can.sendMotorSpeed((byte) currentSpeed);
            } //else if (currentDistance > currentSpeed * 4 ) {
            else if (currentDistance > 90) {
                //if (currentDistance > 30) {
                    if (i < speedValues.length - 1) {
                        i++;
                    }
                //} else {
                    i = 0;
                //}
                    currentSpeed = speedValues[i];
                    can.sendMotorSpeed((byte) currentSpeed);
            }
        } catch(InterruptedException ie) {
            ie.printStackTrace();

        }
    }
    public void setAccEnabled(boolean accEnabled){
        this.accEnabled = accEnabled;
    }
}
