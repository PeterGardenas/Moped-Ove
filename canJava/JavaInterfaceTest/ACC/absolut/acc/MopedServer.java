package absolut.acc;

import java.io.*;

import absolut.can.CanReader;

import java.io.IOException;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

public class MopedServer {
	public static void main(String[] args) throws Exception {
		//Copied from some stack overflow thread.
	    try {
	    	HttpServer server = HttpServer.create(new InetSocketAddress(9090), 0);
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
	        String message = getMessage(t.getRequestBody());
	        t.close();
	        if (!message.equals("false")) {
	        	double hai = Double.parseDouble(message);
	        	int temp = (int) Math.floor(hai);
	        	try {
					CanReader.getInstance().sendSteering((byte) temp);
				} catch (Exception e) {
	        		e.printStackTrace();
				}
			}
	    }
	}

	public static String getMessage(InputStream is) {
		BufferedReader in = new BufferedReader(
				new InputStreamReader(is));
		String inputLine;
		StringBuffer response = new StringBuffer();

		try {
			while ((inputLine = in.readLine()) != null) {
				response.append(inputLine);
			}
			in.close();
		} catch (IOException e) {
			e.printStackTrace();
		}

		String temp = response.toString();
		return temp.substring(temp.indexOf("startM") + "startM".length(), temp.indexOf("endM"));

	}
}
