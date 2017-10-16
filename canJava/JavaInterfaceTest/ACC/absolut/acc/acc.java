package absolut.acc;

import absolut.can.CanReader;


public class acc implements Runnable {

    private CanReader can;
    private Regulator reg;

    public acc() {
    }

    @Override
    public void run() {
        init();
        doFunction();
    }

    private void init() {
        reg = new Regulator();
        can = CanReader.getInstance();
        System.out.println("Gurr ACC in action");
    }

    private void doFunction() {
        //double dist = 0;
        int newControlSignal = 0;
        try {
            can.sendSteering((byte) 10);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        while (true) {
            try {
                newControlSignal = reg.calcNewSpeed();
                can.sendMotorSpeed((byte) newControlSignal);
            } catch(InterruptedException ie){
                ie.printStackTrace();
            }
        }
    }
}