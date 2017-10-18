import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.InetSocketAddress;
import java.util.List;
import java.util.Map;

import com.sun.net.httpserver.Headers;
import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

public class MopedServer {
	public static void main(String[] args) throws Exception {
		//Copied from some stack overflow thread.
	    try {
	    	HttpServer server = HttpServer.create(new InetSocketAddress(9090), 0);
			server.createContext("/response", new ImageHandler());
			server.createContext("/app", new AppHandler());
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
	        System.out.println(getResult(t.getRequestBody()));
	        t.close();
	    }
	}
	
	public static String getResult(InputStream is) {
	    String response = streamToString(is);
	    return response.substring(response.indexOf("startResponse"), response.indexOf("endResponse"));
	}

	public static String streamToString(InputStream is) {
		BufferedReader in = new BufferedReader(
				new InputStreamReader(is));
		String inputLine;
		StringBuilder response = new StringBuilder();
		try {
			while ((inputLine = in.readLine()) != null) {
				response.append(inputLine);
			}
			in.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		return response.toString();
	}
	//Receives post request from app and handles it
	private static class AppHandler implements HttpHandler {
		@Override
		public void handle(HttpExchange t) throws IOException {
			System.out.println("APP rec");
			String message = streamToString(t.getRequestBody());

			System.out.println("Message received: " + message);
			if (message.charAt(0) == 'P'){
				System.out.println("Platoon " + (message.charAt(1) == 'T'));
//				setPlatoon((message.charAt(1) == 'T'))

			}else if (message.charAt(0) == 'A'){
				System.out.println("ACC " + (message.charAt(1) == 'T'));
//				setACC((message.charAt(1) == 'T'));
			}
			String response = "hello world";
			//Has to send a response, otherwise the app prints an error
			t.sendResponseHeaders(200, 1);
			t.close();
		}
	}
}
