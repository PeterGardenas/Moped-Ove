package main;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JPanel;

/* Author: Miranda Bånnsgård, Peter Gärdenäs, Madeleine Lexén, Elina Olsson
 * Description: Loads an image and locates circles of a given color.
 * Gives the offset of the largest found circle.
 */

public class ImageDetector extends JPanel {
	private BufferedImage image = null;
	private Map<Integer, List<Line>> coleredLines = new HashMap<>();
	private List<Shape> finalShapes = new LinkedList<>();
	private long startTime = System.currentTimeMillis();
	
	public ImageDetector(String fileName) {
		startTime = System.currentTimeMillis();
		loadImage(fileName);
		System.out.println("It took: " + (System.currentTimeMillis() - ImageDetectionServer.messageRecived));
		createArrayLists();
		createColorLines();
		mergeLinesToShapes();
	}
	
	private void createArrayLists() {
        for (int i = 0; i < image.getHeight(); i++) {
            coleredLines.put(i, new LinkedList<Line>());
        }
    }

	//Creates horizontal lines of a specified color which are later merge to shape.
    private void createColorLines() {
        for (int y = 0; y < image.getHeight(); y++) {
            boolean wasPreviousCorrectColor = false;
            int startOfInterval = 0;

            for (int x = 0; x < image.getWidth(); x++) {
                if (isCorrectColor(image.getRGB(x, y))) {
                	//printColor(image.getRGB(x, y));
                	
                    if (!wasPreviousCorrectColor) startOfInterval = x;
                    wasPreviousCorrectColor = true;
                } else {
                	/*There seems to be problems if startOfInterval + 1 != x
                	 * Lines of size 1 can't create a circle, might as well ignore them and improve performance.
                	 */
                    if (wasPreviousCorrectColor &&  startOfInterval + 1 != x) {
                        coleredLines.get(y).add(new Line(y, startOfInterval, x));
                    }
                    wasPreviousCorrectColor = false;
                }
            }

            if (wasPreviousCorrectColor) {
                coleredLines.get(y).add(new Line(y, startOfInterval, image.getHeight() - 1));
                wasPreviousCorrectColor = false;
            }
        }

    }
	
    /* Algorithms to merge lines into shapes.
     * 1. Start with the first row of the image by setting each line as a shape.
     * 2. Check the row bellow by looking for connections between the lines and the pre-existing shapes.
     * A couple of scenarios is possible:
     * 		a) No matches, the line becomes a Shape.
     * 		b) One match, add the line to the matched shape.
     * 		c) Several matches, merge the matching shapes and line.
     * 3. Push all shapes with no new connections to finishedShapes.
     * 4. Repeat to step 2 until all rows are done.
     */
	private void mergeLinesToShapes() {
        List<Shape> activeShapes = new LinkedList<>();
        List<Shape> previousShapes = new LinkedList<>();

        for (int i = 0; i < coleredLines.size(); i++) {
        	previousShapes.addAll(activeShapes);
        	
            for (int y = 0; y < coleredLines.get(i).size(); y++) {
                Line line = coleredLines.get(i).get(y);

                List<Integer> matches = new LinkedList<>();
                for (int z = 0; z < activeShapes.size(); z++) {
                    if (activeShapes.get(z).hasConnection(line)) {
                        matches.add(z);
                        activeShapes.get(z).setMatch(true);
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
            
            for (int z = 0; z < previousShapes.size(); z++) {
            	if (!previousShapes.get(z).wasThereAMatch()) {
            		finalShapes.add(previousShapes.get(z));
					activeShapes.remove(previousShapes.get(z));
					previousShapes.remove(z);
            		z--;
            	}
        	}
            previousShapes.removeAll(activeShapes);
            
            for (int z = 0; z < activeShapes.size(); z++) {
        		activeShapes.get(z).setMatch(false);
        	}
        }

        while (!activeShapes.isEmpty()) {
            finalShapes.add(activeShapes.get(0));
            //activeShapes.get(0).mergeLines();
            activeShapes.remove(0);
        }


        System.out.println("Size: " + finalShapes.size());

    }

	
	private void loadImage(String fileName) {
		try {
			image = ImageIO.read(new File(fileName));
			System.out.println("Image load speed: " + (System.currentTimeMillis() - startTime));
		} catch (IOException e) {
			System.out.println(fileName);
			e.printStackTrace();
		}
	}
	
	//Tells if a pixel is of a specified color.
	private boolean isCorrectColor(int clr) {
		int red = (clr & 0x00ff0000) >> 16;
		int green = (clr & 0x0000ff00) >> 8;
		int blue = clr & 0x000000ff;
		
//		return (red > 160 && green < 140 && blue < 140);
//		return (red > 110 && green < 100 && blue < 100) || (red > 130 && green < 130 && blue < 130); //PG's choice
		//return (red > 160 && green < 150 && blue < 150) || (red > 110 && green < 80 && blue < 80);
//		return (red > 90 && green < 40 && blue < 50); //för vinröda bilder = bilder i mörker
		return (red > 115 && green < 80 && blue < 80)  || (red > 130 && green < 100 && blue < 120) || (red > 70 && green < 45 && blue < 20); //PG's choice
//		

	  //return (Math.abs(red - 240) + Math.abs(green - 240) + blue) < 260;
	}
	
	//Gives the offset in a value between -100 to 100 of the largest detected circle. Gives false if no circle is detected.
	public String getResult() {
		Shape finalShape = null;
		for (int i = 0; i < finalShapes.size(); i++) {
			if (finalShapes.get(i).isCircle(null)) {
				if (finalShape == null || finalShapes.get(i).getWidth() > finalShape.getWidth()) {
					finalShape = finalShapes.get(i);
				}
			}	
		}
		return finalShape == null ? "false" : "" + finalShape.distanceFromCenter(image.getWidth()) * 100;
	}
	
	public List<Shape> getFinalShapes() {
		return finalShapes;
	}
	
	/*  Debugging tools */
	
	//Draws the shape.
	public void draw() {
        JFrame frame = new JFrame("Test");
        int width = image.getWidth();
        int height = image.getHeight();

        frame.setVisible(true);
        frame.setSize(width, height);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(this);
    }
	
	//Called by the draw method.
	public void paint(Graphics g) {
        for (int i = 0; i < finalShapes.size(); i++) {
            if (finalShapes.get(i).isCircle(g)) {
                g.setColor(Color.green);
            }
            else {
                g.setColor(Color.RED);
            	g.setColor(new Color((float)Math.random() , (float) Math.random() * 1, (float) Math.random() * 1));

            }
            finalShapes.get(i).paint(g);
        }
    }
	
	//Prints the offset of all circles.
	private void circleDistanceFromCenter(List<Shape> finalShapes) {
        for (int i = 0; i < finalShapes.size(); i++) {
            if (finalShapes.get(i).isCircle(null)) {
            	System.out.println(finalShapes.get(i).distanceFromCenter(image.getWidth()));
            }
        }
    }
	
	//Prints the value of a pixel, useful for debugging the color range.
	private void printColor(int clr) {
		int red = (clr & 0x00ff0000) >> 16;
		int green = (clr & 0x0000ff00) >> 8;
		int blue = clr & 0x000000ff;
		System.out.println("red:" + red);
		System.out.println("green:" + green);
		System.out.println("blue:" + blue);


	}
	
}
