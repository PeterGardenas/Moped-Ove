package tests;

import main.ImageDetector;
import main.Shape;

import static org.junit.Assert.assertTrue;

import java.util.List;

import org.junit.Test;

/**
 * @author Peter Gärdenäs, Hanna Carlsson, Miranda Bånnsgård
 */
public class FindCircleTest {

    @Test
    public void goThroughImages() {
        int[] numberOfCircles = {1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
        int totalNumberOfCircles = 31;
        double marginOfError = 0.1;
        int numberOfErrors = 0;
        int count = 0;
		for (int i = 0; i < 30; i++) {
            int count2 = 0;
            ImageDetector imageDetector = new ImageDetector("red2/test" + i + ".jpg");
            List<Shape> shapes = imageDetector.getFinalShapes();

            for (int y = 0; y < shapes.size(); y++) {
                if (shapes.get(y).isCircle(null)) count2++;
            }

            System.out.println("Picture: " + i + " had " + count2 + " circles. It gave the result: " + imageDetector.getResult());

            if (count2 != numberOfCircles[i]) {
                numberOfErrors += 1;
            }

            count += count2;
            System.out.println();
        }
        System.out.println("Total amount of red circles: " + count);
        System.out.println("Number of errors: " + numberOfErrors);
        assertTrue(numberOfErrors <= totalNumberOfCircles * marginOfError);
    }




}

