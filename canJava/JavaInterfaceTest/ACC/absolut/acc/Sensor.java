package absolut.acc;

import absolut.can.CanReader;
import java.util.Arrays;

public class Sensor{

    public Sensor(){}

    public double getDistance(){
        int[] a = getData();
	//System.out.println(Arrays.toString(a));	
	//if (a != null && a.length > 0) {
	//System.out.println("In noll if");
        Arrays.sort(a);
        double median;
	//System.out.println(Integer.toString(a[0]));
	//System.out.println(Arrays.toString(a));
        if (a.length % 2 == 1) {
	//System.out.println("in if");
	return ((double)a[a.length/2] + (double)a[a.length/2 - 1]) / 2.0; 
        } else {
	//System.out.println("in else");
            return (double)a[a.length/2];
        }
	//}
	//System.out.println("Default value. Failed to enter if");
	//return 40;
	}

    public String getRawData() {
        return CanReader.getInstance().getData();
    }

    public int[] getData() {
//	int[] iData = new int[3];
//	for (int j = 1; j <4; j++){
        String data = getRawData();
	int[] iData;
	//System.out.println("1 " + data);
        String[] sData = data.split(" ");
	iData = new int[sData.length-2];
//	System.out.println("2 " + iData.length);
//	System.out.println("3 " + sData.length);
//	System.out.println("4 " + Arrays.toString(iData));
	if (!data.contains("x")){
	for (int i = 2; i < sData.length; i++) {
            iData[i-2] = Integer.parseInt(sData[i]);
        }
	}
//	}
	System.out.println("Final array " + Arrays.toString(iData));
        return iData;
}
	//int[] intA = {40};
	//return intA;
    }
