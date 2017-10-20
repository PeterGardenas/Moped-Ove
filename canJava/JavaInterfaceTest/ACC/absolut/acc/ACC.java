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
    int[] speedValues = new int[]{0, 9, 11, 15, 19}; //Safe values
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
        System.out.println(sensor.getDistance());
        try {
            can.sendSteering((byte) 0);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true){
            if (accEnabled) {
                currentDistance = (int) sensor.getDistance();
                if (shouldBrake(currentDistance, lastDistance)) {
                    crucialBrake();
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
     */
    public void crucialBrake() {
        try {
                System.out.println("ACTIVATE CRUCIAL BRAKE");
                currentSpeed = -100;
                can.sendMotorSpeed((byte) currentSpeed);
                i = 0;
                currentSpeed = 0;
                can.sendMotorSpeed((byte) currentSpeed);
                Thread.sleep(100);

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
        
        if (currentDistance < currentSpeed * 1.5 && currentSpeed > 0) {
            return true;
        } else if (currentDistance < lastDistance-currentDistance + safetyDistance * 2 && lastDistance < 200 && currentSpeed > 0) {
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
            if (currentDistance < (currentSpeed * 3 + currentSpeed * 0.5) && currentDistance > (currentSpeed * 3 - currentSpeed * 0.5)) {
                currentSpeed = speedValues[i];
                can.sendMotorSpeed((byte) currentSpeed);
            } else if (currentDistance < currentSpeed * 1.5) {
                currentSpeed = 0;
                can.sendMotorSpeed((byte) currentSpeed);
                if (i > 0) {
                    i--;
                }
            } else if (currentDistance > currentSpeed * 1.5 && currentDistance < currentSpeed * 3) {
                if (i > 0) {
                    i--;
                }
                currentSpeed = speedValues[i];
                can.sendMotorSpeed((byte) currentSpeed);
            } else if (currentDistance > currentSpeed * 3 ) {
                if (currentDistance > 30) {
                    if (i < speedValues.length - 1) {
                        i++;
                    }
                } else {
                    i = 0;
                }
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
