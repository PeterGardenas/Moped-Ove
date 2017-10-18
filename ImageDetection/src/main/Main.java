package main;

import java.util.List;

/* Author: Peter Gärdenäs 
 * Description: Rarely used class, mostly useful for debugging.
 */

public class Main {
	private static String standardFileName = "tempIMG/test0.jpg";

	public static void main(String[] args) {
		//goThroughImages();
		new ImageDetector("red2/test" + 19 + ".jpg").draw();
		//new ImageDetector("bilder/checkdraw.jpg").draw();
		//new ImageDetector("img.jpg").draw();


	}
	
	/*
	 * Goes through a group images and stops if no circle is found, 
	 * draws the result of the first image without a circle.
	 */
	private static void goThroughImages() {
		int count = 0;
		for (int i = 0; i < 29; i++) {
			int count2 = 0;
			ImageDetector imageDetector = new ImageDetector("red2/test" + i + ".jpg");
			List<Shape> shapes = imageDetector.getFinalShapes();
			
			for (int y = 0; y < shapes.size(); y++) {
				if (shapes.get(y).isCircle(null)) count2++;
			}
			System.out.println("Picture: "  + i + " had " + count2 + " circles. It gave the result: " + imageDetector.getResult());
			if (count2 == 0) {
				imageDetector.draw();
				System.out.println(count2);
				break;
			}
			count += count2;
			
		}
		System.out.println("count " + count);
	}
}
