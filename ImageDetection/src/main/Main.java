package main;

import java.util.List;

//https://stackoverflow.com/questions/22391353/get-color-of-each-pixel-of-an-image-using-bufferedimages
public class Main {
	private static String standardFileName = "tempIMG/test0.jpg";

	public static void main(String[] args) {
goThroughImages();
		//new ImageDetector("red/test25Bug.jpg").draw();
		//new ImageDetector("red/test" + 58 + ".jpg").draw();
		//new ImageDetector("bilder/checkdraw.jpg").draw();
		//new ImageDetector("img.jpg").draw();


	}
	
	private static void goThroughImages() {
		int count = 0;
		for (int i = 9; i < 100; i++) {
			int count2 = 0;
			ImageDetector imageDetector = new ImageDetector("red/test" + i + ".jpg");
			List<Shape> shapes = imageDetector.getFinalShapes();
			
			for (int y = 0; y < shapes.size(); y++) {
				if (shapes.get(y).isCircle(null)) count2++;
			}
			System.out.println("Picture: "  + i + " had " + count2 + " circles. It gave the result: " + imageDetector.getResult());
			if (count2 > 1) {
				imageDetector.draw();
				break;
			}
			count += count2;
			
		}
		System.out.println("count " + count);
	}
}
