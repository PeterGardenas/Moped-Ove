package absolut.acc;

import absolut.can.CanReader;
import java.io.IOException;
import org.omg.SendingContext.RunTime;

public class Main {

    public static void main(String args[]) throws IOException {
        ACC acc = new ACC();
	    MopedServer.init(acc);
        Thread accThread = new Thread(acc);
        accThread.start();
        Runtime.getRuntime().addShutdownHook(new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    CanReader.getInstance().sendMotorSpeed((byte) 0);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }));
    }
}
