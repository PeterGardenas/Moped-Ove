package plugins;

import java.io.IOException;
import com.sun.squawk.VM;
import sics.port.PluginPPort;
import sics.port.PluginRPort;
import sics.plugin.PlugInComponent;

public class Read_distance extends PlugInComponent {
    private PluginRPort ff;
	
    public Read_distance() {}
	
    public Read_distance(String[] args) {
	super(args);
    }
	
    public static void main(String[] args) {
    Read_distance plugin = new Read_distance(args);
	plugin.run();
    }

    public void init() {
	ff = new PluginRPort(this, "ff");
    }
	
    public void doFunction() throws InterruptedException {
    int distance = ff.readInt();
    VM.println("Distance to object in front: " + distance + " cm");
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
