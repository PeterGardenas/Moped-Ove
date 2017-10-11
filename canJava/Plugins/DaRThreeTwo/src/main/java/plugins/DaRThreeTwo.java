package plugins;

import java.io.IOException;
import com.sun.squawk.VM;
import sics.port.PluginPPort;
import sics.port.PluginRPort;
import sics.plugin.PlugInComponent;

public class DaRThreeTwo extends PlugInComponent {
	public PluginPPort ws;
	public PluginPPort steer;

	public DaRThreeTwo(String[] args) {
		super(args);
	}

	public DaRThreeTwo() {}

	public static void main(String[] args) {
		DaRThreeTwo plugin = new DaRThreeTwo(args);
		plugin.run();
	}

	public void init() {
		ws = new PluginPPort(this, "ws");
		steer = new PluginPPort(this, "st");
	}
	// Simple with interruptedexception and return
	//I want this one to work perfectly
	public void doFunction() throws InterruptedException {
		while (true) {
			steer.write(-50);
			ws.write(7);
			Thread.sleep(1000);
			steer.write(50);
			ws.write(-10);
			Thread.sleep(1000);
			steer.write(0);
			ws.write(7);
			Thread.sleep(1000);
			ws.write(0);
			Thread.sleep(5000);
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
