// Programa Java com RMI
//-------------------------------------------------------------

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface Calculadora extends Remote {

  public int soma (int a, int b) throws RemoteException;

  public int multiplica (int a, int b) throws RemoteException;

  public int divide (int a, int b) throws RemoteException;

  public int subtrai (int a, int b) throws RemoteException;
}
