

import java.io.IOException;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

public class MopedServer {
	public static void main(String[] args) throws Exception {
		//Copied from some stack overflow thread.
	    try {
	    	HttpServer server = HttpServer.create(new InetSocketAddress(9000), 0);
		    server.createContext("/response", new ImageHandler());
		    server.setExecutor(null); // creates a default executor
		    server.start();
		    System.out.println("Server up");
	    } catch (RuntimeException e) {
	    	e.printStackTrace();
	    }
	}
	
	//Receives a post request, handles it and sends a response. 
	static class ImageHandler implements HttpHandler {
	    public void handle(HttpExchange t) throws IOException {
	    	System.out.println("Message recived");
	        String response = "hello world";
	        t.sendResponseHeaders(200, response.length());
	        t.close();
	    }
	}
}
