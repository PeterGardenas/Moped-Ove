package main;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;


//https://stackoverflow.com/questions/22391353/get-color-of-each-pixel-of-an-image-using-bufferedimages

public class Main extends JPanel {
	private static BufferedImage image = null;
	private static Map<Integer, List<Line>> coleredLines = new HashMap<>();
	private static List<Shape> finalShapes = new LinkedList<>();
	private static long startTime = System.currentTimeMillis();
	private static String standardFileName = "test0.jpg";


	public static void main(String[] args) {
		init(standardFileName);
		System.out.println(System.currentTimeMillis() - startTime);
	}
	
	private static void goThroughImages() {
		for (int i = 0; i < 40; i++) {
			reset();
			init("test" + i + ".jpg");
			
			int count = 0;
			for (int y = 0; y < finalShapes.size(); y++) {
				if (finalShapes.get(y).isCircle(null) || finalShapes.get(y).isEllipse(null)) count++;
			}
			System.out.println("Picture: "  + i + "had " + count + " circles.");
			if (count == 0) {
				draw();
				break;
			}
			
		}

	}
	
	private static void reset() {
		image = null;
		coleredLines = new HashMap<>();
		finalShapes = new LinkedList<>();
		startTime = System.currentTimeMillis();
	}

	public static void init(String fileName) {

		loadImage(fileName);
		System.out.println("It took: " + (System.currentTimeMillis() - ServerTest.messageRecived));
		createArrayLists();
		createColorLines();
		mergeLinesToShapes();
		/*
        for (int i = 0; i < finalShapes.size(); i++) {
			Long circleTime = System.currentTimeMillis();
			finalShapes.get(i).isCircle(null);
			//fSystem.out.println("is Circle done in: " + (System.currentTimeMillis() - circleTime) + " ms.");
		}
		System.out.println("Logic done in: " + (System.currentTimeMillis() - startTime) + " ms.");
		*/
		//draw();

		//Both seems far to slow.
		//HoughTransform transform = new HoughTransform(finalShapes);
	}

    private static void createArrayLists() {
        for (int i = 0; i < image.getHeight(); i++) {
            coleredLines.put(i, new LinkedList<Line>());
        }
    }

    private static void createColorLines() {
        for (int i = 0; i < image.getWidth(); i++) {
            boolean wasPreviousCorrectColor = false;
            int startOfInterval = 0;

            for (int y = 0; y < image.getHeight(); y++) {
                if (isCorrectColor(image.getRGB(i, y))) {
                    if (!wasPreviousCorrectColor) startOfInterval = y;
                    wasPreviousCorrectColor = true;
                } else {
                    if (wasPreviousCorrectColor) {
                        coleredLines.get(i).add(new Line(i, startOfInterval, y));
                    }
                    wasPreviousCorrectColor = false;
                }
            }

            if (wasPreviousCorrectColor) {
                coleredLines.get(i).add(new Line(i, startOfInterval, image.getHeight() - 1));
            }
        }

    }

    private static void mergeLinesToShapes() {
        int matchesThisLine;
        List<Shape> activeShapes = new LinkedList<>();

        for (int i = 0; i < coleredLines.size(); i++) {
        	matchesThisLine = 0;
            for (int y = 0; y < coleredLines.get(i).size(); y++) {
                Line line = coleredLines.get(i).get(y);

                List<Integer> matches = new LinkedList<>();
                for (int z = 0; z < activeShapes.size(); z++) {
                    if (activeShapes.get(z).hasConnection(line)) {
                        matches.add(z);
                        matchesThisLine++;
                    }
                }


                if (matches.size() == 0) {
                    Shape newShape = new Shape(line);
                    activeShapes.add(newShape);
                } else {
                    activeShapes.get(matches.get(0)).addLine(line);
                    //If more than 1 shape touches a line they should be merge since they are both connected and
                    //therefore the same shape.
                    if (matches.size() > 1) {
                        int removedCount = 0;
                        for (int z = 1; z < matches.size(); z++) {
                            activeShapes.get(matches.get(0)).merge(activeShapes.get(matches.get(z - removedCount)));
                            activeShapes.remove(matches.get(z) - removedCount);
                            removedCount++;
                        }
                    }

                }


            }
            if (!activeShapes.isEmpty() && matchesThisLine == 0) {
				while (!activeShapes.isEmpty()) {
					finalShapes.add(activeShapes.get(0));
					activeShapes.remove(0);
				}
			}
        }

        while (!activeShapes.isEmpty()) {
            finalShapes.add(activeShapes.get(0));
            //activeShapes.get(0).mergeLines();
            activeShapes.remove(0);
        }


        System.out.println("Size: " + finalShapes.size());

    }

	private static void loadImage(String fileName) {
		try {
			image = ImageIO.read(new File(fileName));
			System.out.println("Image load speed: " + (System.currentTimeMillis() - startTime));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

    private static boolean isCorrectColor(int clr) {
        int red = (clr & 0x00ff0000) >> 16;
        int green = (clr & 0x0000ff00) >> 8;
        int blue = clr & 0x000000ff;
       
        return red > 100 && green > 100 && blue < 50;
        //return (Math.abs(red - 240) + Math.abs(green - 240) + blue) < 260;
    }

    private static void draw() {
        JFrame frame = new JFrame("Test");
        int width = image.getWidth();
        int height = image.getHeight();

        frame.setVisible(true);
        frame.setSize(width, height);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(new Main());
    }


    public void paint(Graphics g) {
        for (int i = 0; i < finalShapes.size(); i++) {

            if (finalShapes.get(i).isEllipse(g) || finalShapes.get(i).isCircle(g)) {
                g.setColor(Color.green);
            }
            else {
                g.setColor(Color.RED);
            }
            finalShapes.get(i).paint(g);
        }
    }


    private static void circleDistanceFromCenter() {
        for (int i = 0; i < finalShapes.size(); i++) {
            if (finalShapes.get(i).isCircle(null)) {
                distanceFromCenter(finalShapes.get(i));
            }
        }
    }

	public static byte[] read(File file) throws IOException {
	    ByteArrayOutputStream ous = null;
	    InputStream ios = null;
	    try {
	        byte[] buffer = new byte[4096];
	        ous = new ByteArrayOutputStream();
	        ios = new FileInputStream(file);
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


    //Negative values if circle is to the left of image center, positive if circle is to the right
    private static double distanceFromCenter(Shape shape) {
        double centerX = image.getWidth() / 2;
        double shapePosX = shape.getCirclePosX(shape);
        double distance = shapePosX - centerX;
        System.out.println("Distance from center: " + distance);
        return distance;
    }
}
