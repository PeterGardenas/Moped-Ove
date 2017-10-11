package absolut.rmi;

import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;

public class RMIHandler implements IMessageHandler {

    private static RMIHandler instance;
    private ArrayList<IMessageReceiver> receivers;

    private RMIHandler(){
        super();
        this.receivers = new ArrayList<>();
        if (System.getSecurityManager() == null) {
            System.setSecurityManager(new SecurityManager());
        }
    }

    public static RMIHandler getInstance() {
        if (instance == null) {
            instance = new RMIHandler();
            try {
                IMessageHandler stub = (IMessageHandler) UnicastRemoteObject.exportObject(instance, 0);
                Registry registry = LocateRegistry.getRegistry();
                registry.rebind("AbsolutRMI", stub);
                System.out.println("RMIHandler bound");
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return instance;
    }

    public void addReceiver(IMessageReceiver receiver) {
        receivers.add(receiver);
    }

    //Example usage
    public static void main(String[] args) {
        RMIHandler.getInstance().addReceiver(new IMessageReceiver() {
            @Override
            public void messageReceived(String msg) {
                if ("AbsolutACCE".equals(msg)) {
                    System.out.println("Enabled ACC");
                } else if ("AboslutACCD".equals(msg)) {
                    System.out.println("Disabled ACC");
                }
            }
        });
    }

    @Override
    public void messageTask(String message) throws RemoteException {
        for (IMessageReceiver receiver: receivers)
            receiver.messageReceived(message);
    }
}
