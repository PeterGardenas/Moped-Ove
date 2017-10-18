package absolut.acc;

import absolut.can.CanReader;
import java.util.Arrays;

/**
 * This code is borrowed from group Absolut, but slightly modified by Julia Ortheden and Johan Wennerbeck.
 */

public class  Sensor{

    public Sensor(){}

    public double getDistance(){
        int[] a = getData();
        Arrays.sort(a);
        double median;
        if (a.length % 2 == 1) {
            System.out.println(Double.toString(((double)a[a.length/2] + (double)a[a.length/2 - 1]) / 2.0));
            return ((double)a[a.length/2] + (double)a[a.length/2 - 1]) / 2.0;
        } else {
            System.out.println(Double.toString((double)a[a.length/2]));
            return (double)a[a.length/2];
        }
    }

    public String getRawData() {
        return CanReader.getInstance().getData();
    }

    public int[] getData() {
        String data = getRawData();
        int[] iData;
        String[] sData = data.split(" ");
        iData = new int[sData.length-2];
        try {
            for (int i = 2; i < sData.length; i++) {
                iData[i-2] = Integer.parseInt(sData[i]);
            }
        } catch (NumberFormatException e) {
            e.printStackTrace();
        }
        return iData;
    }
}
