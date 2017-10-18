package autoStart;

import java.io.IOException;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

public class StartServer {
	public static void main(String[] args) {
		try {
	    	HttpServer server = HttpServer.create(new InetSocketAddress(9090), 0);
		    server.createContext("/start", new ImageHandler());
		    server.setExecutor(null); // creates a default executor
		    server.start();
		    System.out.println("Start server up");
	    } catch (RuntimeException | IOException e) {
	    	e.printStackTrace();
	    }		
	}
	
	private static void runApp() {
		System.out.println("Huumm");
		ProcessBuilder pb = new ProcessBuilder("../../canJava/JavaInterfaceTest/ACC/run.sh", "192.168.137.1");
		System.out.println(pb.directory().getAbsolutePath());
		try {
			Process p = pb.start();
			Thread.sleep(5000l);
			p.destroy();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
	}

	
	static class ImageHandler implements HttpHandler {
		public void handle(HttpExchange t) throws IOException {
	        t.sendResponseHeaders(200, 1);
	        t.close();
	        runApp();
	    }

	}
}
