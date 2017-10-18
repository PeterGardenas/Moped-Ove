package tests;

import main.ImageDetector;
import main.Shape;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

import java.util.List;

/**
 * @author Peter Gärdenäs, Hanna Carlsson, Miranda Bånnsgård
 */
public class FindCircleTest {

    private static final String FILE_NAME= "red2/test";

    //Checks how many circles are found in each picture, test passes if the number of errors are less than 10%
    @Test
    public void findCircles() {
        int[] numberOfCircles = {1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
        int totalNumberOfCircles = 31;
        double marginOfError = 0.1;
        int numberOfErrors = 0;
		for (int i = 0; i < 30; i++) {
            int count2 = 0;
            ImageDetector imageDetector = new ImageDetector(FILE_NAME + i + ".jpg");
            List<Shape> shapes = imageDetector.getFinalShapes();
            for (int y = 0; y < shapes.size(); y++) {
                if (shapes.get(y).isCircle(null)) count2++;
            }
            //System.out.println("Picture: " + i + " had " + count2 + " circles. It gave the result: " + imageDetector.getResult());
            if (count2 != numberOfCircles[i]) {
                numberOfErrors += 1;
            }
        }
        assertTrue(numberOfErrors <= totalNumberOfCircles * marginOfError);
    }

    //Checks if the offset of the circle found in the image are more than 97% accurate
    @Test
    public void findOffset() {
        int[] estimatedOffset = {9, -32, -23, -42, -4, -83, -50, 35, -9, 0, -13, -6, -2, 31, 30, -50, 85, 62};
        double marginOfError = 3;
        int numberOfErrors = 0;

        for (int i = 0; i < estimatedOffset.length; i++) {
            ImageDetector imageDetector = new ImageDetector(FILE_NAME + i + ".jpg");
            double offset = Double.parseDouble(imageDetector.getResult());;
            if ((offset > (estimatedOffset[i] + marginOfError)) || (offset < (estimatedOffset[i] - marginOfError))) {
                numberOfErrors++;
            }
        }
        assertTrue(numberOfErrors == 0);
    }

}

