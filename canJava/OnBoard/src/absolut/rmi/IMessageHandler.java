package absolut.rmi;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface IMessageHandler extends Remote {

    void messageTask(String message) throws RemoteException;
}
