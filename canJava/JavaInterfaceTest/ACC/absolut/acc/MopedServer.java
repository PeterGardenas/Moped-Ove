package absolut.acc;

import java.io.*;

import absolut.can.CanReader;

import java.io.IOException;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

public class MopedServer {



	public static void init() {
		//Copied from some stack overflow thread.
	    try {
	    	HttpServer server = HttpServer.create(new InetSocketAddress(9090), 0);
		    server.createContext("/response", new ImageHandler());
		    server.setExecutor(null); // creates a default executor
		    server.start();
		    System.out.println("Server up");
	    } catch (RuntimeException e) {
	    	e.printStackTrace();
	    } catch (IOException e) {}
	}
	
	//Receives a post request, handles it and sends a response. 
	static class ImageHandler implements HttpHandler {

		private double prevOffset;

		public void handle(HttpExchange t) throws IOException {
	        t.sendResponseHeaders(200, 1);
	        String message = getMessage(t.getRequestBody());
	        t.close();
	        if (!message.equals("false")) {
	        	double steerValue = 0;
	        	double lowPercentage = 0.4;
	        	double highPercentage = 0.8;
	        	double offset = Double.parseDouble(message);
	        	if (((offset >= 0 && prevOffset >= 0) || (offset < 0 && prevOffset < 0)) && Math.abs(offset - prevOffset) > 50) {
	        		steerValue = offset * highPercentage;
				} else if (((offset >= 0 && prevOffset < 0) || (offset < 0 && prevOffset >= 0)) && ((Math.abs(offset) + Math.abs(prevOffset)) > 50)) {
	        		steerValue = offset * highPercentage;
				} else {
	        		steerValue = offset * lowPercentage;
				}
				int steerValueTmp = (int) Math.floor(steerValue);
	        	try {
	        		prevOffset = offset;
					CanReader.getInstance().sendSteering((byte) steerValueTmp);
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
