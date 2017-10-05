package main;

import java.util.List;
import java.awt.Color;
import java.awt.Graphics;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;

/* 
 * A shape is a group of pixels of the same color with direct or indirect connection to each other.
 * A shape is builed by lines.
 */

public class Shape {
	//The circle is never exact. Settings is neccsary to find circles.
	private static double maxHightWidthDifference = 0.1;
	private static double minRadius = 15;
	private static double maxRadius = 1000;
	private static double expectedFullCircle = 0.95;
	private static double checkPoints = 100;
	private static double maxWeightInCircleDifference = 0.05;

	
    private HashMap<Integer, List<Line>> lines = new HashMap<>();
    private HashMap<String, Boolean> cordinates;
    private double startX;
    private double endX;
    private double startY;
    private double endY;

    public Shape(Line line) {
        lines.put(line.getYValue(), new LinkedList<>());
        lines.get(line.getYValue()).add(line);
        this.startX = line.getXStartValue();
        this.endX = line.getEndValue();
        this.endY = this.startY = line.getYValue();
    }

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
                g.drawLine(line.getXStartValue(), line.getYValue(), line.getEndValue(), line.getYValue());
            }
        }
    }


    public boolean isCircle(Graphics g) {
        if (Math.abs(endX - startX) / (endY - startY) - 1 > maxHightWidthDifference) return false;
        if ((endX - startX) < minRadius || (endY - startY) < minRadius) return false;
        if ((endX - startX) > maxRadius || (endY - startY) > maxRadius) return false;
        if (Math.abs(weightInCircle() - 1) > maxWeightInCircleDifference) return false;
        double radius = Math.sqrt((startX - endX) / 2 * (startY - endY) / 2) * expectedFullCircle;
        double middleX = (endX + startX) / 2;
        double middleY = (endY + startY) / 2;
        for (int i = 0; i < checkPoints; i++) {
            int x = (int) (middleX + radius * Math.cos(2 * Math.PI / 100 * i));
            int y = (int) (middleY + radius * Math.sin(2 * Math.PI / 100 * i));
            if (!cordinates.containsKey(x + ":" + y)) return false;
            if (g == null) {
            	g.setColor(Color.black);
                g.drawRect(x, y, 2, 2);
            }
        }
        return true;
    }

    public boolean isEllipse(Graphics g) {
    	if ((endX - startX) < minRadius || (endY - startY) < minRadius) return false;
        if ((endX - startX) > maxRadius || (endY - startY) > maxRadius) return false;
        if (Math.abs(endX - startX) >= endY - startY) return false;
        if (Math.abs(weightInCircle() - 1) > maxWeightInCircleDifference) return false;
        return true;
    }

    /* 
     * Compares a ellipse the calculated radius with the actual amount of pixels.
     * Divides the two values with each other to see if Shapes has the correct size.
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

	// TODO Auto-generated method stub
	//Negative values if circle is to the left of image center, positive if circle is to the right
    public double distanceFromCenter(int imageWidth) {
        double centerX = imageWidth / 2;
        double distance = getCirclePosX() - centerX;
        System.out.println("Distance from center: " + distance);
        return distance;
    }
	
}
