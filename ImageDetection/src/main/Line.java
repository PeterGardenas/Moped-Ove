package main;

/*
 * A vertical line of a specified color.
 */

public class Line {
	private final int yValue;
	private int xStartValue;
	private int xEndValue;
	
	public Line(int yValue, int xStartValue, int endValue) {
		this.yValue = yValue;
		this.xStartValue = xStartValue;
		this.xEndValue = endValue;
	}
	
	public boolean contains(int x) {
		return this.xStartValue <= x && x <= this.xEndValue;
	}
	
	public int getSize() {
		return (xEndValue - xStartValue + 1);
	}
	
	public boolean hasConnection(Line otherLine) {
		if (Math.abs(this.yValue - otherLine.getYValue()) == 1) {
			return checkConnection(otherLine.getXStartValue(), otherLine.getEndValue());
		} else if (otherLine.getYValue() == this.yValue) {
			return checkConnection(otherLine.getXStartValue() - 1, otherLine.getEndValue() + 1);
		}
		return false;
	}
	
	//Sees if the lines connects at any point.
	private boolean checkConnection(int x1, int x2) {
		return (xStartValue <= x1 &&  xEndValue >= x1) ||  (xStartValue <= x2 &&  xEndValue >= x2) ||
				(x1 <= xStartValue &&  x2 >= xStartValue) ||  (x1 <= xEndValue &&  x2 >= xEndValue);
		
	}
	
	public int getYValue() {
		return this.yValue;
	}
	
	public int getXStartValue() {
		return this.xStartValue;
	}
	
	public int getEndValue() {
		return this.xEndValue;
	}
	
	//Merges a line if there is a connection.
	public boolean mergeIfPossible(Line other) {
		if (hasConnection(other)) {
			this.xEndValue = xEndValue > other.xEndValue ? xEndValue : other.xEndValue;
			this.xStartValue = xStartValue < other.xStartValue ? xStartValue : other.xStartValue;
			return true;
		}
		
		return false;
	}
}
