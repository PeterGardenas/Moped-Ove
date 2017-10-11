package plugins;

import com.sun.squawk.VM;
import java.lang.Math;
import sics.plugin.PlugInComponent;
import sics.port.PluginPPort;
import sics.port.PluginRPort;

public class BigCircle extends PlugInComponent {
    public PluginPPort speed;
    public PluginPPort steering;
	
    public BigCircle(String[] args) {
	super(args);
    }
	
    public BigCircle() {
    }
	
    public static void main(String[] args) {
	VM.println("BigCircle.main()\r\n");
	BigCircle ap = new BigCircle(args);
	ap.run();
	VM.println("BigCircle-main done");
    }

    public void init() {
	// Initiate PluginPPort
	VM.println("init 1");
	speed = new PluginPPort(this, "sp");
	VM.println("init 2");
	steering = new PluginPPort(this, "st");
	VM.println("init 3");
    }
	
    public void doFunction() { 
    	int st1 = 0;
    	
	if (false) {
	    while (true) {
		try {
		    VM.println("circle 1");
		    Thread.sleep(2000);
		    speed.write(0);
		    steering.write(st1);

		    VM.println("circle 2");
		    Thread.sleep(2000);
		    speed.write(0);
		    steering.write(st1);
		    Thread.sleep(2000);

		    // was 11 before
		    for (int i = 1; i < 6; i++) {
			VM.println("circle " + (i + 2));
			speed.write(10*i);
			steering.write(st1);
			Thread.sleep(10000);
		    }
		} catch (InterruptedException e) {
		    //VM.println("Interrupted.");
		}
	    }
	}

	if (true) {
	    try {
	    	steering.write(0);
	    	speed.write(1);
	    	Thread.sleep(1000);
		st1 = 30;
		VM.println("bigcircle 1");
		Thread.sleep(2000);
		speed.write(0);
		steering.write(st1);

		VM.println("bigcircle 2");
		Thread.sleep(2000);
		speed.write(0);
		steering.write(st1);
		Thread.sleep(2000);
	    } catch (InterruptedException e) {
		//VM.println("Interrupted.");
	    }
		
	    speed.write(10*1);
	    steering.write(st1);

	    while (true) {
		try {
		    Thread.sleep(10000);
		} catch (InterruptedException e) {
		    //VM.println("Interrupted.");
		}
	    }
	}
    }

    public void run() {
	init();
	doFunction();
    }
}
