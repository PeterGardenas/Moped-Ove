package main;

import java.util.List;

//https://stackoverflow.com/questions/22391353/get-color-of-each-pixel-of-an-image-using-bufferedimages
public class Main {
	private static String standardFileName = "tempIMG/test0.jpg";

	public static void main(String[] args) {
		new ImageDetector(standardFileName);
	}
	
	private static void goThroughImages() {
		for (int i = 0; i < 40; i++) {
			ImageDetector imageDetector = new ImageDetector("test" + i + ".jpg");
			List<Shape> shapes = imageDetector.getFinalShapes();
			
			int count = 0;
			for (int y = 0; y < shapes.size(); y++) {
				if (shapes.get(y).isCircle(null) || shapes.get(y).isEllipse(null)) count++;
			}
			System.out.println("Picture: "  + i + "had " + count + " circles.");
			if (count == 0) {
				imageDetector.draw();
				break;
			}
			
		}
	}
}
