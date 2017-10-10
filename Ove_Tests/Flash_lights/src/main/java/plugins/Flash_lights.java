package plugins;

import java.io.IOException;
import com.sun.squawk.VM;
import sics.port.PluginPPort;
import sics.port.PluginRPort;
import sics.plugin.PlugInComponent;

public class Flash_lights extends PlugInComponent {
    private PluginPPort led;
	
    public Flash_lights() {}
	
    public Flash_lights(String[] args) {
	super(args);
    }
	
    public static void main(String[] args) {
	Flash_lights plugin = new Flash_lights(args);
	plugin.run();
    }

    public void init() {
	led = new PluginPPort(this, "led");
    }
	
    private void sleep(int ms) {
    	try {
    		Thread.sleep(ms);
    	} catch (InterruptedException e) {
    		VM.println("Interrupted. \r\n");
    	}
    }
    
    private void setLed(String str) {
    	led.write(str);
    	/*String status;
    	
    	if(str.charAt(2) == "0")
    		status = "on";
    	else if (str.charAt(2) == "1") {
    		status = "off";
    	} 
    	VM.println("Light number " + str.charAt(0) + " is " + status); */
    	
    	VM.println("Flash " + str);
    	sleep(1000);
    }
    
    public void doFunction() throws InterruptedException {
    	while(true) {
    		setLed("2|0");
    		setLed("2|1");
    		setLed("1|0");
    		setLed("1|1");
    		setLed("3|0");
    		setLed("3|1");
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
