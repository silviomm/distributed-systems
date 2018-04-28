// Programa Java com RMI
//-------------------------------------------------------------

import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;

public class CalculadoraClient {
  private CalculadoraClient() {}
  // Metodo principal
  public static void main (String[] args) {
    int pagina;

    // Especifica o nome do servidor e do objeto para obter um stub para acessar o objeto servidor
    String host = (args.length < 1) ? null : args[0];
    try {
       Registry registry = LocateRegistry.getRegistry(host);
       Calculadora stub = (Calculadora) registry.lookup("Calculadora");
       for(;;) {
          Thread.sleep(500);
          System.out.println ("vou fazer uma conta. ");
 
          System.out.println ("2+2 = " + stub.soma(2,2));
          System.out.println ("2-2 = " + stub.subtrai(2,2));
          System.out.println ("2*2 = " + stub.multiplica(2,2));
          System.out.println ("2/2 = " + stub.divide(2,2));




          Thread.sleep(500);


       }
    } catch (Exception e) {
       System.err.println("Erro cliente: " + e.toString());
       e.printStackTrace();
    }
  }
}
