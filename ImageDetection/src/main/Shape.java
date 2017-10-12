package main;

import java.util.List;
import java.awt.Color;
import java.awt.Graphics;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;

/* 
 * A shape is a group of pixels of the same colour with direct or indirect connection to each other.
 * The pixels are grouped together as lines.
 */

public class Shape {
	//The circle is never exact. Settings is neccsary to find circles.
	private static final double MAX_HIGHT_WIDTH_DIFFERENCE = 0.15;
	private static final double MIN_RADIUS = 15;
	private static final double MAX_RADIUS = 1000;
	private static final double EXPECTED_FULL_CIRCLE = 0.95;
	private static final double CHECK_POINTS = 100;
	private static final double MAX_WEIGHT_IN_CIRCLE_DIFFERENCE = 0.05;

	
    private HashMap<Integer, List<Line>> lines = new HashMap<>();
    private HashMap<String, Boolean> cordinates;
    private double startX;
    private double endX;
    private double startY;
    private double endY;
    private boolean match = false;

    public Shape(Line line) {
        lines.put(line.getYValue(), new LinkedList<>());
        lines.get(line.getYValue()).add(line);
        this.startX = line.getXStartValue();
        this.endX = line.getEndValue();
        this.endY = this.startY = line.getYValue();
    }

    //Adds a line to shapes and sets new start and end values. 
    public void addLine(Line line) {
        if (lines.get(line.getYValue()) == null) lines.put(line.getYValue(), new LinkedList<>());
        if (line.getYValue() > this.endY) this.endY = line.getYValue();
        if (line.getYValue() < this.startY) this.startY = line.getYValue();
        if (line.getEndValue() > this.endX) this.endX = line.getEndValue();
        if (line.getXStartValue() < this.startX) this.startX = line.getXStartValue();
        lines.get(line.getYValue()).add(line);

    }
    
    //Checks if the shape has a direct connection with a line.
    public boolean hasConnection(Line otherLine) {
        int startY = otherLine.getYValue() - 1;
        for (int i = startY; i < (startY + 3); i++) {
            List<Line> currentLines = lines.get(i);
            if (i == -1 || currentLines == null) continue;
            for (int y = 0; y < currentLines.size(); y++) {
                if (currentLines.get(y).hasConnection(otherLine)) return true;
            }
        }
        return false;
    }

    //Combines two difference Shapes with each other.
    public void merge(Shape other) {
        Iterator<Integer> iterator = other.lines.keySet().iterator();
        while (iterator.hasNext()) {
            int key = iterator.next();
            for (int y = 0; y < other.lines.get(key).size(); y++) {
                this.addLine(other.lines.get(key).get(y));
            }
        }
    }

    //Draws the Shapes, useful for debugging.
    public void paint(Graphics g) {
        Iterator<Integer> iterator = lines.keySet().iterator();
        while (iterator.hasNext()) {
            int key = iterator.next();
            for (int y = 0; y < lines.get(key).size(); y++) {
                Line line = lines.get(key).get(y);
                //g.drawLine(line.getXStartValue(), line.getYValue(), line.getEndValue(), line.getYValue());
                g.drawLine(line.getXStartValue(), line.getYValue(), line.getEndValue(), line.getYValue());

            }
        }
    }


    public boolean isCircle(Graphics g) {
        if (Math.abs(endX - startX) / (endY - startY) - 1 > MAX_HIGHT_WIDTH_DIFFERENCE) return false;
        if ((endX - startX) < MIN_RADIUS || (endY - startY) < MIN_RADIUS) return false;
        if ((endX - startX) > MAX_RADIUS || (endY - startY) > MAX_RADIUS) return false;
        if (Math.abs(weightInCircle() - 1) > MAX_WEIGHT_IN_CIRCLE_DIFFERENCE) return false;
        double radius = Math.sqrt((startX - endX) / 2 * (startY - endY) / 2) * EXPECTED_FULL_CIRCLE;
        double middleX = (endX + startX) / 2;
        double middleY = (endY + startY) / 2;
        int count = 0;
        double percentage = 0.5;
        for (int i = 0; i < CHECK_POINTS; i++) {
            //if (true)break;
            int x = (int) (middleX + radius * Math.cos(2 * Math.PI / 100 * i));
            int y = (int) (middleY + radius * Math.sin(2 * Math.PI / 100 * i));
            if (!cordinates.containsKey(x + ":" + y)) {
                count++;
            }

            if (g != null) {
            	g.setColor(Color.black);
                g.drawRect(x, y, 2, 2);
            }
        }
        return (count/100) <= percentage;
    }

    public boolean isEllipse(Graphics g) {
    	if ((endX - startX) < MIN_RADIUS || (endY - startY) < MIN_RADIUS) return false;
        if ((endX - startX) > MAX_RADIUS || (endY - startY) > MAX_RADIUS) return false;
        if (Math.abs(endX - startX) >= endY - startY) return false;
        if (Math.abs(weightInCircle() - 1) > MAX_WEIGHT_IN_CIRCLE_DIFFERENCE) return false;
        return true;
    }

    /* 
     * Compares a ellipse the calculated radius with the actual amount of pixels of the shape.
     * Divides the two values with each other to see if Shapes has the correct size. Eg, a square would get a
     * higher value but a hollow circle a smaller value.
     */
    private double weightInCircle() {
        cordinates = new HashMap<>();
        Iterator<Integer> iterator = lines.keySet().iterator();
        while (iterator.hasNext()) {
            int key = iterator.next();
            for (int i = 0; i < lines.get(key).size(); i++) {
                Line line = lines.get(key).get(i);
                for (int x = line.getXStartValue(); x <= line.getEndValue(); x++) {
                    cordinates.put(x + ":" + line.getYValue(), true);
                }
            }
        }
        return Math.abs(Math.PI * (startX - endX) / 2 * (startY - endY) / 2) / cordinates.size();

    }
    
    //Calculates the size of the Shape by checking the size of each line.
    public int calculateSize() {
        int count = 0;
        Iterator<Integer> iterator = lines.keySet().iterator();
        while (iterator.hasNext()) {
            int key = iterator.next();
            for (int i = 0; i < lines.get(key).size(); i++) {
                count += lines.get(key).get(i).getSize();
            }
        }
        return count;
    }

    //Some lines might overlap, this merge them together.
    public void mergeLines() {
        Iterator<Integer> iterator = lines.keySet().iterator();
        while (iterator.hasNext()) {
            int key = iterator.next();
            for (int y = 0; y < lines.get(key).size(); y++) {
                Line line = lines.get(key).get(y);
                for (int z = y + 1; z < lines.get(key).size(); z++) {
                    if (line.mergeIfPossible(lines.get(key).get(z))) {
                        lines.get(key).remove(z);
                        z--;
                    }
                }
            }
        }
    }

    public double getCirclePosX() {
        return (endX + startX) / 2;
    }

	//Negative values if circle is to the left of image center, positive if circle is to the right
    public double distanceFromCenter(int imageWidth) {
        double centerX = imageWidth / 2;
        double distance = getCirclePosX() - centerX;
        System.out.println("Distance from center: " + distance);
        return 2 * distance / imageWidth;
    }

	public void setMatch(boolean b) {
		this.match = b;
	}

	public boolean wasThereAMatch() {
		return match;
	}

	public int getWidth() {
		return Math.abs((int) (startX - endX));
	}
	
}
