package main.steering;

/**
 * Created by Elina Olsson on 2017-10-09.
 */
public class Steering {

    public int steerWheelsTest(int deviation) {
        int steerValue;
        //double percentage = 0.1;
        steerValue = deviation/* * percentage*/;
        return steerValue;
    }

    /* Update steering after every new picture from the camera

    public void updateSteeringWheels(int deviation) {
        while(deviation > 5 || deviation < -5) {
            steerWheels(deviation);
        }
    }*/

    //Is called when we know that the MOPED in front starts to turn
    private void sleepUntilTurn (double dist, double velocity) {
        double time = dist / velocity;
        long timeToTurn = (long) time * 1000;

        try {
            Thread.sleep(timeToTurn);
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    //Need to know the angle of the turn
    public double steerWheels(int deviation) {
        return steerWheelsTest(deviation);
    }

    /*
    public double distance(double sensorValue) {
        double dist = 0; //cm
        return dist;
    }*/

}
