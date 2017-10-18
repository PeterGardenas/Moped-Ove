package main;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.URL;
import java.nio.charset.Charset;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

/*
 * Author: Peter Gärdenäs
 * Description: A server which receives images, gets the analysed response and sends it back.
 * 
 */

public class ImageDetectionServer {
	static long messageRecived = System.currentTimeMillis();
	private static final String FILE_NAME = "img.jpg";
	private static int i = 11;
	
	//Start the server.
	public static void main(String[] args) throws Exception {
	    try {

	    	HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
		    server.createContext("/processimage", new ImageHandler());
		    server.setExecutor(null); // creates a default executor
		    server.start();

	    	
	    	//sendAnswer("127.0.0.1",new ImageDetector("img.jpg").getResult());
	    } catch (RuntimeException e) {
	    	e.printStackTrace();
	    }
	}
	
	//Receives a post request, handles it and sends a response. 
	static class ImageHandler implements HttpHandler {
	    public void handle(HttpExchange t) throws IOException {
	    	System.out.println("Message recived");
	        loadImage(t.getRequestBody());
	    	String response = new ImageDetector(FILE_NAME).getResult();
	    	t.sendResponseHeaders(200, response.length());
	    	//Get the ip adress of the sender.
	    	String adress = t.getRemoteAddress().getAddress().getHostAddress().trim();
		    t.close();
			try {
				sendAnswer(adress, response);
				System.out.println("Respone:" + response);
			} catch (Exception e) {
				e.printStackTrace();
			}
	    }
	}
	
	
	/*
     * The input stream contains irrelevant information which shouldn't be loaded to the image,
     * since it destroys the image's format. These method extracts the message and converts it to an image.
	 * Modification of: https://stackoverflow.com/questions/35423531/java-writing-a-string-to-a-jpg-file
	 */
	private static void loadImage(InputStream is) {
		messageRecived = System.currentTimeMillis();
		try {
			File imgFile;
		    byte[] bytes;
		    FileOutputStream fos;
		    String temp = "";
			
            bytes = read(is);
            
            
             //Remove irrelevant information.
            int removeBefore = 83;
            int removeAfter = 40;
            
            for(int j = 0; j < removeBefore; j++) {
                temp += (char) bytes[j];
            }
            
            for (int j = 0; j < removeAfter; j++) {
            	temp += (char) bytes[bytes.length - 1 - j];
            }
            
            imgFile = new File(FILE_NAME);
            if (i < 100) i++; //Sometimes used to save several images for testing.

            fos = new FileOutputStream(imgFile);            
            fos.write(bytes, removeBefore, bytes.length - (removeBefore + removeAfter));
            fos.close();
			
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}
	
	//Copied from https://stackoverflow.com/questions/35423531/java-writing-a-string-to-a-jpg-file
	private static byte[] read(InputStream ios) throws IOException {
	    ByteArrayOutputStream ous = null;
	    try {
	        byte[] buffer = new byte[4096];
	        ous = new ByteArrayOutputStream();
	        int read = 0;
	        while ((read = ios.read(buffer)) != -1) {
	            ous.write(buffer, 0, read);
	        }
	    } finally {
	        try {
	            if (ous != null)
	                ous.close();
	        } catch (IOException e) {
	        }

	        try {
	            if (ios != null)
	                ios.close();
	        } catch (IOException e) {
	        }
	    }

	    return ous.toByteArray();
	}
	
	/* We want the answer in Java, therefore a new post request is sent instead of
	 * just adding it to the response of the received post request.
	 * Sends the post request to the MOPED.
	 */
	private static void sendAnswer(String adress, String message) throws Exception{
	    String url = "http://" + adress + ":9090/response";
	    URL obj = new URL(url);
	    HttpURLConnection con = (HttpURLConnection) obj.openConnection();
	    
	    con.setRequestMethod("POST");
	    //Make the message easy to locate in the inputstream.
		message = "startM" + message + "endM";
	    // Send post request
	    con.setDoOutput(true);
	    DataOutputStream wr = new DataOutputStream(con.getOutputStream());
	    wr.write(message.getBytes(Charset.forName("UTF-8")));
	    wr.flush();
	    wr.close();
	    System.out.println("sent");

	    con.getResponseCode();
	    
	}

}