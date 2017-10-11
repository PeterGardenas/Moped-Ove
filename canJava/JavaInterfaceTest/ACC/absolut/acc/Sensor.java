package absolut.acc;

import absolut.can.CanReader;
import java.util.Arrays;

public class Sensor{

    public Sensor(){}

    public double getDistance(){
        int[] a = getData();
        Arrays.sort(a);
        double median;
        System.out.println(Arrays.toString(a));
        if (a.length % 2 == 1) {
            return ((double)a[a.length/2] + (double)a[a.length/2 - 1]) / 2.0; 
        } else {
            return (double)a[a.length/2];
        }
    }

    public String getRawData() {
        return CanReader.getInstance().getData();
    }

    public int[] getData() {
        String data = getRawData();
        String[] sData = data.split(" ");
        int[] iData = new int[sData.length-2];
        for (int i = 2; i < sData.length; i++) {
            iData[i-2] = Integer.parseInt(sData[i]);
        }
        return iData;
    }
}