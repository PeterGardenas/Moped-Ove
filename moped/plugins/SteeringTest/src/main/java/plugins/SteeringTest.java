package plugins;

import java.io.IOException;
import com.sun.squawk.VM;
import sics.port.PluginPPort;
import sics.port.PluginRPort;
import sics.plugin.PlugInComponent;

public class SteeringTest extends PlugInComponent {
   
  private PluginPPort st;
   
  public SteeringTest() {}
   
  public SteeringTest(String[] args) {
   super(args);
   }
   
  public static void main(String[] args) {
   SteeringTest plugin = new SteeringTest(args);
   plugin.run();
   }

  public void init() {
   st = new PluginPPort(this, "st");
   }
   
  public void doFunction() throws InterruptedException {
       int[] testDeviation = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5,
               5, 5, 5, 5, 5, 5, 5, 15, 15, 15, 15, 15, 15, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -5, 5,
               -5, -5, -5, -5, 5, 5, 5, 15, -15, -15, -15, 15, 15, 15, };
       int steeringValue;
       //double percentage = 0.1;
       for (int i = 0; i < testDeviation.length; i++) {
           steeringValue = testDeviation[i];
           st.write(steeringValue);
           Thread.sleep(1000);
          
       }
   }

  public void run() {
   init();

   try {
       doFunction();
   } catch (InterruptedException e) {
       VM.println("**************** Interrupted.");
       return;
   }
   }
}
