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

public class ImageDetector extends JPanel {
	private BufferedImage image = null;
	private Map<Integer, List<Line>> coleredLines = new HashMap<>();
	private List<Shape> finalShapes = new LinkedList<>();
	private long startTime = System.currentTimeMillis();
	
	public ImageDetector(String fileName) {

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
	}
	
	private void createArrayLists() {
        for (int i = 0; i < image.getHeight(); i++) {
            coleredLines.put(i, new LinkedList<Line>());
        }
    }

    private void createColorLines() {
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
	
	private void mergeLinesToShapes() {
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

	
	private void loadImage(String fileName) {
		try {
			image = ImageIO.read(new File(fileName));
			System.out.println("Image load speed: " + (System.currentTimeMillis() - startTime));
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private boolean isCorrectColor(int clr) {
		int red = (clr & 0x00ff0000) >> 16;
		int green = (clr & 0x0000ff00) >> 8;
		int blue = clr & 0x000000ff;   
		return red > 100 && green > 100 && blue < 50;
	        //return (Math.abs(red - 240) + Math.abs(green - 240) + blue) < 260;
	    }
	
	public void draw() {
        JFrame frame = new JFrame("Test");
        int width = image.getWidth();
        int height = image.getHeight();

        frame.setVisible(true);
        frame.setSize(width, height);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(this);
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
	
	public List<Shape> getFinalShapes() {
		return finalShapes;
	}
	
	private void circleDistanceFromCenter(List<Shape> finalShapes) {
        for (int i = 0; i < finalShapes.size(); i++) {
            if (finalShapes.get(i).isCircle(null)) {
            	finalShapes.get(i).distanceFromCenter(image.getWidth());
            }
        }
    }
}
