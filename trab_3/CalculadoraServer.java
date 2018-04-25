// Programa Java com RMI
//-------------------------------------------------------------

import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;


// Classe servidora de metodos remotos
public class CalculadoraServer implements Calculadora { 

  // Construtor
  public CalculadoraServer() {}
   
  public int soma (int a, int b){
    return a+b;
  }

  public int multiplica (int a, int b){
    return a*b;
  }

  public int divide (int a, int b){
    return a/b;
  }

  public int subtrai (int a, int b){
    return a-b;
  }


  // Funcao principal
  public static void main(String args[]) {
     try {
        CalculadoraServer obj = new CalculadoraServer();
        Calculadora stub = (Calculadora) UnicastRemoteObject.exportObject(obj, 0);

        // Vincula o stub do objeto remoto ao servico de registro
        Registry registry = LocateRegistry.getRegistry();
        registry.bind("Calculadora", stub);

        System.err.println("Servidor de contas pronto!");
     } catch (Exception e) {
        System.err.println("Erro servidor de contas!: " + e.toString());
        e.printStackTrace();
     }
  }
}

