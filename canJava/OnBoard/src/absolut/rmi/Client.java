package absolut.rmi;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.Scanner;

// Example client
// Commandline interface
public class Client {

    private IMessageHandler comp;

    public Client() {
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }
        Scanner scanner = new Scanner(System.in);
        boolean running = true;
        while (running) {
            try {
                String in = scanner.nextLine();
                if ("-1".equals(in)) {
                    running = false;
                    continue;
                }
                sendMessage(in);
                System.out.println("Message sent to server");
            } catch (RemoteException e) {
                System.out.println("Not connected to server, ignoring input");
                comp = null;
            }
        }
    }

    private boolean setComp() {
        try {
            String name = "AbsolutRMI";
            Registry registry = LocateRegistry.getRegistry(null);
            comp = (IMessageHandler) registry.lookup(name);
            return true;
        } catch (Exception ignored) {
            return false;
        }
    }

    private void sendMessage(String message) throws RemoteException {
        if (comp != null) {
            comp.messageTask(message);
        } else if (setComp()){
            comp.messageTask(message);
        }
    }


    public static void main(String[] args) {
        new Client();
    }

}
