import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.*;
import java.util.concurrent.Semaphore;

/**
 * @author Erik Kallberg (kalerik@student.chalmers.se)
 * @author Hugo Frost
 * Singleton class CAN for interfacing with can-utils's candump and cansend, mimicking MOPED python
 * code
 */
public final class CAN {

  private static CAN instance;

  private static String CAN_INTERFACE = "vcan0";
  private static String DUMP_COMMAND = "candump";
  private static String SEND_COMMAND = "cansend";

  private static String VCU_COMMAND_CAN_ID = "101";
  private static String VCU_ODOMETER_CAN_ID = "Not known at this time";
  private static String SCU_ULTRASONIC_CAN_ID = "Not known at this time";
  private static byte motorValue = 0;
  private static byte steerValue = 0;

  private static long VCU_COOL_DOWN = 1000; //TODO find out how fast one can switch command

  private Thread outputWorkerThread;
  private Thread inputWorkerThread;

  private OutputWorker outputWorker;

  private ArrayList<Short> UltraSonicSensorData = new ArrayList<Short>(); //TODO
  private ArrayList<Short> OrdometerData = new ArrayList<Short>(); //TODO

  /**
   * CAN singleton constructor starts CAN input and output worker threads
   *
   * @throws IOException when raised by either input or output worker constructors
   */
  private CAN() throws IOException {
    outputWorker = new OutputWorker(VCU_COOL_DOWN);
    outputWorkerThread = new Thread(outputWorker);
    inputWorkerThread = new Thread(new InputWorker());
    inputWorkerThread.start();
    outputWorkerThread.start();
  }

  /**
   * Initiates an instance if one does not exist
   *
   * @return the one and only instance
   * @throws IOException when raised by Runtime::exec
   */
  public static CAN getInstance() throws IOException {
    if (instance == null) {
      instance = new CAN();
    }
    return instance;
  }

  /**
   * Converts each byte to hex code with zero padding
   *
   * @param data byte-string to convert to hex-string
   * @return zero padded hex-string
   */
  private static String byteToHexString(byte[] data) {
    StringBuilder sb = new StringBuilder();

    for (byte b : data) {
      sb.append(String.format("%02X", b));
    }

    return sb.toString();
  }

  /**
   * Schedules a frame to be sent by output worker thread
   *
   * @param frame to be sent
   * @throws InterruptedException from OutputWorker::queueFrame
   */
  public void sendCANFrame(CANFrame frame) throws InterruptedException {
    outputWorker.queueFrame(frame);
  }

  /**
   * Automates sending of motor and steer packets to VCU
   *
   * @param motor value to be sent to VCU
   * @param steer value to be sent to VCU
   * @throws IOException from sendCANFrame
   * @throws InterruptedException fromSendCANFrame
   */
  public void sendMotorAndSteerValue(byte motor, byte steer)
      throws IOException, InterruptedException {
    byte[] motorAndSteerBytes = new byte[2];
    motorAndSteerBytes[0] = motor;
    motorAndSteerBytes[1] = steer;

    CANFrame frame = new CANFrame(VCU_COMMAND_CAN_ID, motorAndSteerBytes);

    sendCANFrame(frame);

    motorValue = motor;
    steerValue = steer;
  }

  /**
   * Motor and Steer value must be sent at the same time this method sends the old steer value along
   * with the new motor value
   *
   * @param motor value to be sent to VCU
   * @throws IOException from sendMotorAndSteerValue
   * @throws InterruptedException from sendMotorAndSteerValue
   */
  public void sendMotorValue(byte motor) throws IOException, InterruptedException {
    sendMotorAndSteerValue(motor, steerValue);
  }

  /**
   * Motor and Steer value must be sent at the same time this method sends the old motor value along
   * with the new steer value
   *
   * @param steer value to be sent to VCU
   * @throws IOException from sendMotorAndSteerValue
   * @throws InterruptedException from sendMotorAndSteerValue
   */
  public void sendSteerValue(byte steer) throws IOException, InterruptedException {
    sendMotorAndSteerValue(motorValue, steer);
  }

  /**
   * Basically a container class (think C structure) for CAN frames received and sent
   */
  private class CANFrame {

    private String identity;
    private double time;
    private byte[] data;

    public CANFrame(String identity, double time, byte[] data) {
      this.identity = identity;
      this.time = time;
      this.data = data;
    }

    public CANFrame(String identity, byte[] data) {
      this(identity, -1, data);
    }

    public String getID() {
      return identity;
    }

    public byte[] getData() {
      return data;
    }

    public double getTime() {
      return time;
    }
  }

  /**
   * Runnable, launched by parent (CAN), that reads CAN packets into can frames by launching candump
   * and continuously parsing it's standard output into sensor frames that are put into queues
   * accessible by parent (CAN) object. Uses semaphores for mutex because the java keyword
   * synchronized is confusing
   */
  private class InputWorker implements Runnable {

    private Semaphore odometerQueueLock;
    private Queue<CANFrame> odometerQueue;

    private Semaphore usSensorQueueLock;
    private Queue<CANFrame> usSensorQueue;

    private Process canDumpProcess;
    private InputStream canDumpStandardOutput;

    public InputWorker() throws IOException {
      odometerQueueLock = new Semaphore(1);
      odometerQueue = new ArrayDeque<>();

      usSensorQueueLock = new Semaphore(1);
      usSensorQueue = new ArrayDeque<>();

      String[] argv = new String[4];
      argv[0] = CAN.DUMP_COMMAND;
      argv[1] = "-t";
      argv[2] = "z";
      argv[3] = CAN.CAN_INTERFACE;

      canDumpProcess = Runtime.getRuntime().exec(argv);
      canDumpStandardOutput = canDumpProcess.getInputStream();
    }

    /**
     * Beware super lazy parsing. Hardcoded for candump run with flag '-t' option 'z'
     *
     * @return can frame from parsed candump standard output
     */
    private CANFrame readFrame() throws IOException {
      BufferedReader reader = new BufferedReader(new InputStreamReader(canDumpStandardOutput));
      String canDataString = reader.readLine().trim();

      //For example canDataString = "(003.602137)  vcan0  535   [8]  04 14 C7 30 3C 96 C5 4B"

      canDataString = canDataString.replace("   ", " ");

      //now canDataString = "(003.602137)  vcan0  535 [8]  04 14 C7 30 3C 96 C5 4B"

      canDataString = canDataString.replace("  ", " ");

      //now canDataString = "(003.602137) vcan0 535 [8] 04 14 C7 30 3C 96 C5 4B"

      String[] tokens = canDataString.split(" ");

      //now tokens = {"(003.602137)", "vcan0", "558", "[8]", "04", "14", ..., "4B"}

      String canTimeString = tokens[0].replace("(", "").replace(")", "");
      String canInterfaceString = tokens[1];
      String canIdString = tokens[2];
      String dataLengthString = tokens[3].replace("[", "").replace("]", "");

      double time = Double.parseDouble(canTimeString);
      int dataLength = Integer.parseInt(dataLengthString);

      byte[] data = new byte[dataLength];

      for (int i = 0; i < dataLength; i++) {
        byte token = (byte) (16 * Character.digit(tokens[i + 3].charAt(0), 16));
        data[i] = token;
        token = (byte) Character.digit(tokens[i + 3].charAt(1), 16);
        data[i] += token;

      }

      return new CANFrame(canIdString, time, data);
    }

    @Override
    public void run() {
      while (true) {
        try {
          CANFrame frame = readFrame();

          if (frame.identity.equals(CAN.VCU_ODOMETER_CAN_ID)) {
            System.out.println("Received Odometer frame");
            odometerQueueLock.acquire();

            odometerQueue.add(frame);

            odometerQueueLock.release();
          } else if (frame.identity.equals(CAN.SCU_ULTRASONIC_CAN_ID)) {
            System.out.println("Received ultrasonic sensor frame");
            usSensorQueueLock.acquire();

            usSensorQueue.add(frame);

            usSensorQueueLock.release();
          } else if (frame.identity.equals(CAN.VCU_COMMAND_CAN_ID)) {
            System.out.println(String
                .format("Detected VCU command: motor: %d, steer: %d", frame.data[0],
                    frame.data[1]));
          } else {
            System.out.println(String.format("Unknown CAN frame: id=%s; data=%s", frame.identity,
                byteToHexString(frame.data)));
          }
        } catch (IOException | InterruptedException e) {
          e.printStackTrace();
          break;
        }
      }
    }
  }

  /**
   * Runnable, launched by parent (CAN), that schedules sending of CAN frames put in queue by parent
   * When experimenting with the VCU we've found that it ignores commands if they are sent too
   * quickly. Uses semaphores for shared queues for the same reason as described above InputWorker.
   */
  private class OutputWorker implements Runnable {

    private Semaphore queueLock;
    private Queue<CANFrame> frameOutputQueue;
    private long recoveryTime;

    public OutputWorker(long recoveryTime) {
      queueLock = new Semaphore(1);
      this.recoveryTime = recoveryTime;
      frameOutputQueue = new ArrayDeque<>();
    }

    public void queueFrame(CANFrame frame) throws InterruptedException {
      queueLock.acquire();

      frameOutputQueue.add(frame);

      queueLock.release();
    }

    private void sendFrame(CANFrame frame) throws InterruptedException, IOException {
      String[] argv = new String[3];
      argv[0] = SEND_COMMAND;
      argv[1] = CAN_INTERFACE;
      argv[2] = String.format("%s#%s", frame.identity, byteToHexString(frame.data));
      Process csProcess = Runtime.getRuntime().exec(argv);
      csProcess.waitFor();
    }

    @Override
    public void run() {
      while (true) {
        try {
          queueLock.acquire();

          if (!frameOutputQueue.isEmpty()) {
            sendFrame(frameOutputQueue.poll());
          }

          queueLock.release();

          Thread.sleep(recoveryTime);
        } catch (InterruptedException | IOException e) {
          e.printStackTrace();
        }
      }
    }
  }
}
