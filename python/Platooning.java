import java.io.IOException;
import java.util.Random;

public final class Platooning {
  private CAN can;
  //private ACC acc;
  //private ALC alc;
  //private Simulator simulator; //TODO make interchangeable with CAN

  private Platooning(CAN can/*, ACC acc, ALC alc*/) {
    this.can = can;
    //this.acc = acc;
    //this.alc = alc;
  }

  public static void main(String args[]) throws IOException, InterruptedException {
    CAN can = CAN.getInstance();
    //ACC acc = new ACC(); //TODO update when constructor of ACC is done
    //ALC alc = new ALC(); //TODO update when constructor of ALC is done
    Platooning platooning = new Platooning(can/*, acc, alc*/);
    //platooning.start() //TODO entry point for platooning logic
    //TODO user interface (command-line)

    byte[] randomMotorBytes = new byte[100];
    new Random().nextBytes(randomMotorBytes);

    for (int i = 0; i < 100; i++) {
      can.sendMotorValue(randomMotorBytes[i]);
    }

    Thread.sleep(1000*60);
    System.exit(0);
  }

}
