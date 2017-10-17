package absolut.acc;

public class Regulator {

    private Sensor sensor;
    private int preferredDistance;
    private double Kp;
    private double Ki;
    private double Kd;
    private double Dt;
    private double lastError;


    public Regulator(){
        init();
    }

    private void init(){
        preferredDistance = 40;
        Kp = 0.85;
        Ki = 0.00001;
        Kd = 0.00001;
        Dt = 100;
        lastError = 0;
        sensor = new Sensor();
    }


    public int calcNewSpeed(){

        double sensorValue = sensor.getDistance();
        double error = sensorValue - preferredDistance;
        double controlSignal;
        double integral = 0;
        double derivate = 0;

        integral = integral + (error * Dt);
        derivate = (error - lastError) / Dt;

        controlSignal = Kp * error + (Ki * integral) + (Kd * derivate);

        controlSignal = clamp(Math.round(controlSignal), 0, 100);

        if(controlSignal > 20){
            controlSignal = 20;
        }
        if(controlSignal < -20){
            controlSignal = -20;
        }


        System.out.println(controlSignal);

        lastError = error;

        return (int) Math.round(controlSignal);
    }

    private double clamp(double in, double min, double max) {
        return Math.max(min, Math.min(in, max));
    }

}