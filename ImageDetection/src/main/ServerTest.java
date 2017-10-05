package main;

import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.net.HttpURLConnection;
import java.net.InetSocketAddress;
import java.net.URL;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;

public class ServerTest {
	public static long messageRecived = System.currentTimeMillis();
	

	public static void main(String[] args) throws Exception {
		/* To kill the server on windows:
		 * 1. netstat -ano | findstr :8080
		 * 2. taskkill /PID typeyourPIDhere /F
		 * 
		 * 
		 */
        
		
	    try {
	    	HttpServer server = HttpServer.create(new InetSocketAddress(8080), 0);
		    server.createContext("/requests", new MyHandler());
		    server.createContext("/processimage", new MyHandler());
		    System.out.println("hai");
		    server.setExecutor(null); // creates a default executor
		    server.start();
	    } catch (RuntimeException e) {
	    	e.printStackTrace();
	    }
	   
	   
	   
	    //sendPost();
	    
	}

	static class MyHandler implements HttpHandler {
	    public void handle(HttpExchange t) throws IOException {
	    	System.out.println("Humm");
	        String response = "hello world";
	        
	        loadImage(t.getRequestBody());
	        Main.init("test.jpg");
	
	        
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

            // read the 8 byte string from the beginning of the file
            for(int j = 0; j < 106; j++) {
                temp += (char) bytes[j];
            }
            
            for (int j = 0; j < 38; j++) {
            	temp += (char) bytes[bytes.length - 1 - j];
            }

            imgFile = new File("test.jpg");

            // points to './img.jpg'
            fos = new FileOutputStream(imgFile);
            
            // write from offset 8 to end of 'bytes'
            fos.write(bytes, 106, bytes.length - (106 + 38));

            fos.close();
			
			
			
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}
	
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