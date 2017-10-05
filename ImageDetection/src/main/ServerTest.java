package main;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

public class ServerTest {
	public static long messageRecived = System.currentTimeMillis();
	public static int i = 0;

	
	/* To kill the server on windows:
	 * 1. netstat -ano | findstr :8080
	 * 2. taskkill /PID typeyourPIDhere /F
	 */
	public static void main(String[] args) throws Exception {
		//Copied from some stack overflow thread.
	    try {
	    	HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
		    server.createContext("/processimage", new ImageHandler());
		    server.setExecutor(null); // creates a default executor
		    server.start();
	    } catch (RuntimeException e) {
	    	e.printStackTrace();
	    }
	}
	
	//Receives a post request, handles it and sends a response. 
	static class ImageHandler implements HttpHandler {
	    public void handle(HttpExchange t) throws IOException {
	    	System.out.println("Message recived");
	        String response = "hello world";
	        loadImage(t.getRequestBody());
	        new ImageDetector("test" + i + ".jpg");
	        t.sendResponseHeaders(200, response.length());
	    }
	}
	
	//Modification of: https://stackoverflow.com/questions/35423531/java-writing-a-string-to-a-jpg-file
	private static void loadImage(InputStream is) {
		messageRecived = System.currentTimeMillis();
		try {
			File imgFile;
		    byte[] bytes;
		    FileOutputStream fos;
		    String temp = "";
			
			// 'read' method can be found below
            bytes = read(is);
            
            
            /*
             * The input stream contains irrelevant information which shouldn't be loaded to the image,
             * since it destroys the image's format.
             */
            int removeBefore = 83;
            int removeAfter = 40;
            
            for(int j = 0; j < removeBefore; j++) {
                temp += (char) bytes[j];
            }
            
            for (int j = 0; j < removeAfter; j++) {
            	temp += (char) bytes[bytes.length - 1 - j];
            }
            
            //Store a sample slice of 100 images for testing.
            imgFile = new File("test" + i + ".jpg");
            if (i < 100) i++;

            fos = new FileOutputStream(imgFile);            
            fos.write(bytes, removeBefore, bytes.length - (removeBefore + removeAfter));
            fos.close();
			
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}
	
	//Copied from https://stackoverflow.com/questions/35423531/java-writing-a-string-to-a-jpg-file
	public static byte[] read(InputStream ios) throws IOException {
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

}